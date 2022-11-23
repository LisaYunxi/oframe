#include "FtdFront.h"
#include "printinfo.h"

const int SESSION_CHECK_TIMER_ID = 9;
const int SESSION_CHECK_INTERVAL = 1000;

bool CFrontWriteLogReactor::OpenChannelLogFile()
{
	if (m_fpChannelLog)
		fclose(m_fpChannelLog);
	getLogMinTime(m_fDate);
	char szFullFilename[512];
	sprintf(szFullFilename, "%s_%d.%s.nlog", m_strFlowPathName.c_str(), m_dwFrontNo, m_fDate);
	m_fpChannelLog = fopen(szFullFilename, "ab");
	if (m_fpChannelLog == NULL)
		return false;
	return true;
}

void CFrontWriteLogReactor::Run()
{
	char *pBuff;

	char sBuff[8192];
	int nDataLen = 8192;

	int iLen = 0;
	for (;;)
	{
		if (m_reader.GetFlow()->GetFlowType() == FLOW_TYPE_ONETIMEFLOW)
		{
			pBuff = (char *)m_reader.GetNext(iLen);
			if (pBuff == NULL)
			{
				SleepMs(10);
				continue;
			}
		}
		else
		{
			iLen = m_reader.GetNext(sBuff, nDataLen);
			if (iLen <= 0)
			{
				SleepMs(10);
				continue;
			}
			pBuff = sBuff;
		}

		//д���ļ�ͷ���������������ģ����Ա���Ҫż����Ҫ��Ȼͷ��������������ļ��У�����޷�����
		if (m_nCount > m_nGap && m_nCount % 2 == 0)
		{
			OpenChannelLogFile();
			m_nCount = 0;
		}

		if (NULL != m_fpChannelLog)
		{
			fwrite(pBuff, 1, iLen, m_fpChannelLog);
			fflush(m_fpChannelLog);
			m_nCount++;
		}
	}
}

CFtdSessionFactory::CFtdSessionFactory(CReactor* pReactor, CFlow* pSeriesFlow, CFlowManager *pFlowManager, const char* pFrontAddress, int nFrontid)
	:CSessionFactory(pReactor, 500)
{
	m_pReactor = pReactor;
	m_pSeriesFlow = pSeriesFlow;
	m_rspPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);

	strcpy(m_FrontAddress, pFrontAddress);
	m_dwFrontNo = nFrontid;

	m_pFlowManager = pFlowManager;

	//if (!strcmp(m_pConfig->getConfig("FrontLogs"), "no"))
	if (GetConfigString(INI_FILE_NAME, "FrontLogs") == "no")
	{
		m_pSlogResult = NULL;
		m_pChannelLogFlow = NULL;
		m_pFrontWriteLogReactor = NULL;
	}
	else
	{
		m_pSlogResult = new COneTimeFlow();
		//int nGap = atoi(m_pConfig->getConfig("PersistGap")) * 2;
		int nGap = GetConfigInt(INI_FILE_NAME, "PersistGap") * 2;
		if (nGap == 0)
			nGap = 1000000;
		m_pFrontWriteLogReactor = new CFrontWriteLogReactor(m_pSlogResult, GetConfigString(INI_FILE_NAME, "FlowPath").c_str(), nFrontid, nGap);
		m_pFrontWriteLogReactor->Create();
		m_pChannelLogFlow = new CSlogFlow(m_pSlogResult);
	}

	//m_nMaxSpeed = atoi(m_pConfig->getConfig("MaxSpeed"));
	//REPORT_EVENT(LOG_CRITICAL, "CKernelFront", "FtdFront Max Speed is:%d/s", m_nMaxSpeed);

	RegisterListener(m_FrontAddress);
	REPORT_EVENT(LOG_CRITICAL, "CFront", "Open Port:%s", m_FrontAddress);
	EnableListener(false);

	m_reqPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);

	m_nFlowCacheSize = 0x100000;
	//int nFlowCacheSize = atoi(pConfig->getConfig("FlowCacheSize"));
	int nFlowCacheSize = GetConfigInt(INI_FILE_NAME, "FlowCacheSize");
	if (nFlowCacheSize > 0)
		m_nFlowCacheSize = nFlowCacheSize;
}

CFtdSessionFactory::~CFtdSessionFactory()
{
	m_mapSessionInfo.Clear();
	if (m_pChannelLogFlow != NULL)
	{
		CSessionMap::iterator itor = m_mapSession.Begin();
		for (; !itor.IsEnd(); itor++)
		{
			(*itor)->GetChannel()->SetLogFlow(NULL);
		}
	}
}

void CFtdSessionFactory::OnTimer(int nIDEvent)
{
	switch (nIDEvent)
	{
	case SESSION_CHECK_TIMER_ID:
	{
		CSessionMap::iterator itor = m_mapSession.Begin();
		while (!itor.IsEnd())
		{
			TFrontSessionInfo *pFind = m_mapSessionInfo.Find((*itor)->GetSessionID());
			if (pFind != NULL)
			{
				CheckSession(*itor, pFind);
			}
			itor++;
		}
		break;
	}
	default:
	{
	}
	}
}

void CFtdSessionFactory::CheckSession(CSession *pSession, TFrontSessionInfo *pSessionInfo)
{
	///���¿�ʼ��������
	pSessionInfo->nCommFlux -= pSessionInfo->nMaxCommFlux;
	if (pSessionInfo->nCommFlux < 0)
		pSessionInfo->nCommFlux = 0;

	///�����������涨�������ûỰ�Ķ�����
	pSession->SuspendRead(pSessionInfo->nCommFlux >= pSessionInfo->nMaxCommFlux);
}

void CFtdSessionFactory::CalcFlux(CSession *pSession, const int nFlux)
{
	TFrontSessionInfo *pSessionInfo = m_mapSessionInfo.Find(pSession->GetSessionID());
	if (pSessionInfo != NULL)
	{
		//		pSessionInfo->nCommFlux += nFlux;
		pSessionInfo->nCommFlux++; //�������Ƹ�Ϊ����
		///�����������涨�������ûỰ�Ķ�����
		if (pSessionInfo->nCommFlux >= pSessionInfo->nMaxCommFlux)
		{
			pSession->SuspendRead(true);
		}
	}
}

/*	���Ự���ӽ�����ʱ�򣬴���ǰ�ûỰ��Ϣ����һ��ʼ��ʱ�򣬻Ự״̬��û�е�¼��
//	��������Ự����־�ļ���
*/
void CFtdSessionFactory::OnSessionConnected(CSession *pSession)
{
	//�ȵ��û���ķ���
	CSessionFactory::OnSessionConnected(pSession);

	TFrontSessionInfo sinfo;
	memset(&sinfo, 0, sizeof(TFrontSessionInfo));
	sinfo.nSessionID = pSession->GetSessionID();
	sinfo.bIsLogin = false;
	sinfo.nConnectTime = m_pReactor->Time();
	sinfo.nCommFlux = 0;
	sinfo.nMaxCommFlux = g_nMaxSpeed;
	m_mapSessionInfo.Insert(sinfo.nSessionID, sinfo);

	pSession->GetChannel()->SetLogFlow(m_pChannelLogFlow);

	REPORT_EVENT(LOG_WARNING, "Front", "Session [%s][%d] connected", pSession->GetChannel()->GetRemoteName(), pSession->GetSessionID());

	m_reqPackage.PrepareRequest(OUTFOG_TNO_NULL, OUTFOG_CHAIN_LAST);
	m_reqPackage.GetXTPHeader()->SessionNo = pSession->GetSessionID();
	m_reqPackage.GetXTPHeader()->RequestNo = 0;
	m_reqPackage.GetXTPHeader()->Type = OUTFOG_TYPE_ADDSESSION;

	char *pIPAddress = pSession->GetChannel()->GetRemoteName();
	strcpy(m_reqPackage.GetXTPHeader()->Remark, pIPAddress);
	m_reqPackage.GetXTPHeader()->FrontNo = m_dwFrontNo;
	m_reqPackage.GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
	m_reqPackage.MakePackage();
	m_pSeriesFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());
}

///	���Ự�Ͽ���ʱ��ɾ��ǰ�ûỰ��Ϣ���󡣲���������ϵͳ�����ǳ�����
void CFtdSessionFactory::OnSessionDisconnected(CSession *pSession, int nReason)
{
	//������ڸûỰ����Ϣ������ɾ��
	m_mapSessionInfo.Erase(pSession->GetSessionID());

	//	REPORT_EVENT(LOG_EMERGENCY, "Session", "Send User Logout [%d]", pSession->GetSessionID());
		//�������淢���ý���Ա�ĵǳ�����
		//�����������޷���֪���Ѷ��ߣ��ý���Ա����Ҳ�޷���¼��
	m_reqPackage.PrepareRequest(OUTFOG_TNO_NULL, OUTFOG_CHAIN_LAST);
	m_reqPackage.GetXTPHeader()->Type = OUTFOG_TYPE_DELSESSION;
	m_reqPackage.GetXTPHeader()->SessionNo = pSession->GetSessionID();
	char *pIPAddress = pSession->GetChannel()->GetRemoteName();
	strcpy(m_reqPackage.GetXTPHeader()->Remark, pIPAddress);
	m_reqPackage.GetXTPHeader()->FrontNo = m_dwFrontNo;
	m_reqPackage.GetXTPHeader()->RequestNo = 0;
	m_reqPackage.GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
	m_reqPackage.MakePackage();
	m_pSeriesFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());

	REPORT_EVENT(LOG_WARNING, "Front", "Session [%s][%d] disconnected", pSession->GetChannel()->GetRemoteName(), pSession->GetSessionID());

	//	pSession->CloseChannelLogFile();

		//�����û���ķ���
	CSessionFactory::OnSessionDisconnected(pSession, nReason);
}

//static void DescribeMemberOfCFTDDisseminationField()
//{
//	CFTDDisseminationField f;
//	f.DescribeMembers();
//}
//CFieldDescribe CFTDDisseminationField::m_Describe(FTD_FID_Dissemination, sizeof(CFTDDisseminationField), "CFTDDisseminationField", "��Ϣ�ַ�", &DescribeMemberOfCFTDDisseminationField);

CSession *CFtdSessionFactory::CreateSession(CChannel *pChannel, unsigned int dwMark)
{
	CFTDCSession *pSession = new CFTDCSession(m_pReactor, pChannel);
	pSession->RegisterPackageHandler(this);

	pSession->SetCompressMethod(CRPCM_ZERO);		//����ѹ���㷨
	pSession->SetHeartbeatMode(XMPHBT_SLAVE_MODE);

	return pSession;
}

bool CFtdSessionFactory::HandleResponse(CXTPPackage *pMessage)
{
	CFTDCSession *pSession = NULL;
	pSession = (CFTDCSession *)GetSession(pMessage->GetXTPHeader()->SessionNo);
	if (pSession == NULL)
		return false;

	m_rspPackage.PreparePackage(pMessage->GetXTPHeader()->MessageNo, pMessage->GetXTPHeader()->Chain);
	m_rspPackage.GetFTDCHeader()->RequestNo = pMessage->GetXTPHeader()->RequestNo;
	m_rspPackage.GetFTDCHeader()->SubjectNo = pMessage->GetXTPHeader()->SubjectNo;
	//����������Ϣ
	//memcpy(m_rspPackage.GetFTDCHeader()->Remark, pMessage->GetXTPHeader()->Remark, 62);
	strncpy(m_rspPackage.GetFTDCHeader()->Remark, pMessage->GetXTPHeader()->Remark, 61);//Ϊ�˽�������õ����⣬��ͬSubIndexNameһ�𿽱�

	switch (pMessage->GetXTPHeader()->Chain)
	{
	case OUTFOG_CHAIN_FIRST:
	case OUTFOG_CHAIN_CONTINUE:
	{
		m_rspPackage.GetFTDCHeader()->Chain = FTDC_CHAIN_CONTINUE;
		break;
	}
	case OUTFOG_CHAIN_LAST:
	case OUTFOG_CHAIN_SINGLE:
	{
		m_rspPackage.GetFTDCHeader()->Chain = FTDC_CHAIN_LAST;
		break;
	}
	}

	m_rspPackage.DupPackage(pMessage);
	//m_rspPackage.AttachBuffer(pMessage->GetPackageBuffer());

	//ToDo: 20210309 ���Կ��ܵ����⣬������Ƿ����ע�͵���ֻʹ���������
	//SendToDialogFlow(pSession, &m_rspPackage);

	switch (pMessage->GetXTPHeader()->SubjectNo)
	{
		// 	case OUTFOG_SUBJECTID_QUERY:
		// 		{
		// 			SendToQueryFlow(pSession, &m_rspPackage);
		// 			break;
		// 		}
	case 1:
	{
		SendToDialogFlow(pSession, &m_rspPackage);
		break;
	}
	default:
	{
		SendRsponser(pSession, &m_rspPackage);
	}
	}

	//	NET_IO_LOG0("CFtdEngine::HandleMessage\n");
	// 	XTP_PACKAGE_DEBUG(pMessage);	

	return true;
}

int CFtdSessionFactory::HandlePackage(CFTDCPackage *pFTDCPackage, CFTDCSession *pSession)
{
	//NET_IO_LOG0("CFtdEngine::HandlePackage\n");
	//Ӱ��Ч�ʣ�ɾ��
	//FTDC_PACKAGE_DEBUG(pFTDCPackage);
	//û�����أ���ʱȡ���ô���,	ͳ�������ǿ���
	//CalcFlux(pSession, pFTDCPackage->Length());

	m_reqPackage.PreparePublish(pFTDCPackage->GetFTDCHeader()->MessageNo, OUTFOG_CHAIN_LAST);
	m_reqPackage.GetXTPHeader()->SessionNo = pSession->GetSessionID();
	m_reqPackage.GetXTPHeader()->RequestNo = pFTDCPackage->GetFTDCHeader()->RequestNo;
	m_reqPackage.GetXTPHeader()->SubjectNo = pFTDCPackage->GetFTDCHeader()->SubjectNo;
	m_reqPackage.DupPackage(pFTDCPackage);
	m_reqPackage.GetXTPHeader()->FrontNo = m_dwFrontNo;
	m_reqPackage.GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
	m_reqPackage.MakePackage();
	m_pSeriesFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());
	return 0;
}

void CFtdSessionFactory::SubTopic(CXTPPackage *pMessage)
{
	CFTDCSession *pSession = (CFTDCSession *)GetSession(pMessage->GetXTPHeader()->SessionNo);
	if (pSession == NULL)
		return;

	//	PublishFlow(pSession,pMessage->GetXTPHeader()->SubIndexName,pMessage->GetXTPHeader()->UserID,pMessage->GetXTPHeader()->SubjectNo,pMessage->GetXTPHeader()->SequenceNo);

	// 	TFrontSessionInfo *pSessionInfo = m_mapSessionInfo.Find(pSession->GetSessionID());
	// 	if (pSessionInfo == NULL)
	// 	{
	// 		return;
	// 	}

	CReadFlow *pFlow = NULL;

	if (pMessage->GetXTPHeader()->SubIndexName[0] != '\0')
	{
		pFlow = m_pFlowManager->GetIndexFlow(pMessage->GetXTPHeader()->SubjectNo, pMessage->GetXTPHeader()->SubIndexName);
	}
	else
	{
		//if (pMessage->GetXTPHeader()->UserID[0] != '\0')
		//	pFlow = m_pFlowManager->GetUserFlow(pMessage->GetXTPHeader()->SubjectNo, pMessage->GetXTPHeader()->UserID);
		//else
			pFlow = m_pFlowManager->GetFlow(pMessage->GetXTPHeader()->SubjectNo);
	}

	if (pFlow == NULL)
		return;

	//UF_INT8 nPubStartId = atof(pMessage->GetXTPHeader()->Remark);
	UF_INT8 nPubStartId = pMessage->GetXTPHeader()->SequenceNo;

	//if (nPubStartId <= 0 || nPubStartId > pFlow->GetCount())
	//{
	//	nPubStartId = pFlow->GetCount();
	//}
	//else
	//{
	//	if (nPubStartId < pFlow->GetFirstID())
	//	{
	//		nPubStartId = pFlow->GetFirstID();
	//	}
	//}

	if (nPubStartId <= 0)
		nPubStartId = pFlow->GetCount() + nPubStartId;

	if (nPubStartId > pFlow->GetCount())
		nPubStartId = pFlow->GetCount();

	if (nPubStartId < pFlow->GetFirstID())
		nPubStartId = pFlow->GetFirstID();

	int nLocalNo = pMessage->GetXTPHeader()->MessageNo;

	pSession->Publish(pFlow, nLocalNo, nPubStartId, pMessage->GetXTPHeader()->BusinessNo,pMessage->GetXTPHeader()->RequestNo);

	//m_rspPackage.PreparePackage(FTD_TID_IntlRtnDissemination, FTDC_CHAIN_LAST);
	//CFTDDisseminationField field;
	//field.SubjectNo = nLocalNo;
	//field.SequenceNo = nPubStartId;
	//m_rspPackage.AddField(&(field.m_Describe), &field);
	//SendToDialogFlow(pSession, &m_rspPackage);
}

void CFtdSessionFactory::UnSubTopic(CXTPPackage *pMessage)
{
	CFTDCSession *pSession = (CFTDCSession *)GetSession(pMessage->GetXTPHeader()->SessionNo);
	if (pSession == NULL)
		return;

	if (!strcmp(pMessage->GetXTPHeader()->Remark, OUTFOG_REMARK_UNSUBTOPIC_ALL))
	{
		pSession->UnPublishAll();
	}
	else
	{
		int nLocalNo = pMessage->GetXTPHeader()->MessageNo;
		pSession->UnPublish(nLocalNo);
	}
}

void CFtdSessionFactory::DeleteSession(CXTPPackage *pMessage)
{
	CSession *pSession = GetSession(pMessage->GetXTPHeader()->SessionNo);
	if (pSession == NULL)
		return;
	pSession->Disconnect(0);
}

void CFtdSessionFactory::SendToDialogFlow(CFTDCSession *pSession, CFTDCPackage *pPackage)
{
	TFrontSessionInfo *pSessionInfo = m_mapSessionInfo.Find(pSession->GetSessionID());
	if (pSessionInfo == NULL)
		return;

	TFTDCHeader  *pHeader = pPackage->GetFTDCHeader();
	pHeader->SubjectNo = 1;
	pHeader->SequenceNo = ++(pSessionInfo->nDialogFlowSeqNo);
	pSession->SendRequestPackage(pPackage);
}

void CFtdSessionFactory::SendRsponser(CFTDCSession *pSession, CFTDCPackage *pPackage)
{
	TFrontSessionInfo *pSessionInfo = m_mapSessionInfo.Find(pSession->GetSessionID());
	if (pSessionInfo == NULL)
		return;

	pSession->SendRequestPackage(pPackage);
}
