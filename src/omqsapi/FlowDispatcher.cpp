#include "public.h"
#include "FlowDispatcher.h"
#include "Config.h"

CFlowDispatcher::CFlowDispatcher(CReadFlow* pTradeResult, CFlowManager *pFlowManager, int nKernelid)
{
	m_pFlowManager = pFlowManager;
	m_rspPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
	m_Package.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
	m_ResultReader.AttachFlow(pTradeResult, UF_INT8_ZERO);
	m_nKernelid = nKernelid;
}

CFlowDispatcher::~CFlowDispatcher()
{
}

void CFlowDispatcher::Run()
{
	while (true)
	{
		if (!m_ResultReader.GetNext(&m_Package))
		{
			//SleepMs(1);
#ifdef WIN32
			::Sleep(1);
#else
			::usleep(10);
#endif
			continue;
		}
		HandleMessage(&m_Package);
	}
}

//处理kernel产生的结果流
//bool CFlowDispatcher::HandleOtherTask()
//{
//	//for (int i = 0; i < 100; i++)
//	//{
//	//	if (m_ResultReader.GetNext(&m_Package))
//	//		HandleMessage(&m_Package);
//	//	else
//	//		break;
//	//}
//	//return true;
//	//return CSelectReactor::HandleOtherTask();
//
//	while (true)
//	{
//		if (!m_ResultReader.GetNext(&m_Package))
//		{
//			SleepMs(1);
//			continue;
//		}
//		HandleMessage(&m_Package);
//	}
//}

void CFlowDispatcher::HandleMessage(CXTPPackage *pMessage)
{
	switch (pMessage->GetXTPHeader()->Type)
	{
	case OUTFOG_TYPE_SUBTOPIC:
	{
		//由于front进程处理rsp流水和notify流水不是串型的，导致无法处理resume=-1的流水的连贯性问题
		//处理方式就是在这里的串行处理中获取resumeNo=-1的具体续传号，这样就不会丢失数据，保证数据的强一致性
		if (pMessage->GetXTPHeader()->SequenceNo <= 0)
		{
			CReadFlow* pFlow = NULL;
			if (pMessage->GetXTPHeader()->SubIndexName[0] != '\0')
				pFlow = m_pFlowManager->GetIndexFlow(pMessage->GetXTPHeader()->SubjectNo, pMessage->GetXTPHeader()->SubIndexName);
			else
				pFlow = m_pFlowManager->GetFlow(pMessage->GetXTPHeader()->SubjectNo);

			if (pFlow != NULL)
				pMessage->GetXTPHeader()->SequenceNo += pFlow->GetCount();
		}
	}
	case OUTFOG_TYPE_DELSESSION:
	case OUTFOG_TYPE_UNSUBTOPIC:
	case OUTFOG_TYPE_RESPONSE:
	{
		//int a = 0;
		//GET_ONLY_MSEC_TIME(a);
		//REPORT_EVENT(LOG_CRITICAL, "HandleMessage1", "%0.f:%d", pMessage->GetXTPHeader()->GlobalNo, a);
		CFlow *pFlow = m_pFlowManager->GetFlow(pMessage->GetXTPHeader()->FrontNo);
		if (pFlow != NULL)
		{
			pMessage->MakePackage();
			pFlow->Append(pMessage->Address(), pMessage->Length());
		}
		//GET_ONLY_MSEC_TIME(a);
		//REPORT_EVENT(LOG_CRITICAL, "HandleMessage2", "%0.f:%d", pMessage->GetXTPHeader()->GlobalNo, a);
		break;
	}
	case OUTFOG_TYPE_ADDAPIKEY:
	{
		char sApiKey[129], sSecretKey[129];
		strcpy(sApiKey, pMessage->GetXTPHeader()->Remark);
		sApiKey[128] = '\0';
		//UserID,sSecretKey
		//先处理UserID
		char* pUser = pMessage->Address() + sizeof(TFieldHeader);
		char* pSecretKey = pUser;
		while (true)
		{
			if (*pSecretKey == ',')
			{
				*pSecretKey = '\0';
				pSecretKey++;
				break;
			}
			if (*pSecretKey == '\0')
				break;
			pSecretKey++;
		}
		strcpy(sSecretKey, pSecretKey);
		sSecretKey[128] = '\0';
		SHttpSession HttpSession;
		HttpSession.HitTimes = 0;
		HttpSession.HitBeginTime = 0;
		HttpSession.AccessKey = sApiKey;
		HttpSession.SecretKey = sSecretKey;
		HttpSession.UserID = pUser;

		if (pMessage->GetXTPHeader()->SessionNo == 0)
			HttpSession.MaxSpeed = g_nMaxSpeed;
		else
			HttpSession.MaxSpeed = pMessage->GetXTPHeader()->SessionNo;
		HttpSession.ExpireTime = pMessage->GetXTPHeader()->BusinessNo;

		CHttpSessionMap::iterator iter = g_SessionMap.find(sApiKey);
		if (iter != g_SessionMap.end())
		{
			iter->second.AccessKey = HttpSession.AccessKey;
			iter->second.SecretKey = HttpSession.SecretKey;
			iter->second.UserID = HttpSession.UserID;
			iter->second.MaxSpeed = HttpSession.MaxSpeed;
			iter->second.ExpireTime = HttpSession.ExpireTime;
			REPORT_EVENT(LOG_ERROR, "UpdateApiKey", "%s:%s:%s:%s:%d", iter->second.AccessKey.c_str(), iter->second.SecretKey.c_str(),
				iter->second.UserID.c_str(), LargeNumberToString(HttpSession.ExpireTime).c_str(), iter->second.MaxSpeed);
		}
		else
		{
			g_SessionMap.insert(CHttpSessionMap::value_type(HttpSession.AccessKey, HttpSession));
			REPORT_EVENT(LOG_ERROR, "InsertApiKey", "%s:%s:%s:%s:%d", HttpSession.AccessKey.c_str(), HttpSession.SecretKey.c_str(),
				HttpSession.UserID.c_str(), LargeNumberToString(HttpSession.ExpireTime).c_str(), HttpSession.MaxSpeed);
		}
		break;
	}
	case OUTFOG_TYPE_DELAPIKEY:
	{
		char sApiKey[129];
		strcpy(sApiKey, pMessage->GetXTPHeader()->Remark);
		sApiKey[128] = '\0';
		CHttpSessionMap::iterator iter = g_SessionMap.find(sApiKey);
		if (iter != g_SessionMap.end())
		{
			REPORT_EVENT(LOG_ERROR, "DeleteApiKey", "%s:%s", sApiKey, iter->second.UserID.c_str());
			g_SessionMapRWLock.WriteLock();
			g_SessionMap.erase(iter);
			g_SessionMapRWLock.UnLock();
		}
		else
		{
			REPORT_EVENT(LOG_ERROR, "DeleteApiKey", "Can not Find %s", sApiKey);
		}
		break;
	}
	case OUTFOG_TYPE_ADDAPIKEYALLOWIP:
	{
		char sApiKey[129];
		strcpy(sApiKey, pMessage->GetXTPHeader()->Remark);
		sApiKey[128] = '\0';
		CHttpSessionMap::iterator iter = g_SessionMap.find(sApiKey);
		if (iter != g_SessionMap.end())
		{
			iter->second.IPAddress.insert(pMessage->Address() + sizeof(TFieldHeader));
			REPORT_EVENT(LOG_ERROR, "AddApiKeyIP", "%s:%s", sApiKey, pMessage->Address() + sizeof(TFieldHeader));
		}
		else
		{
			REPORT_EVENT(LOG_ERROR, "AddApiKeyIP", "%s:%s -> Error Not Found!", sApiKey, pMessage->Address() + sizeof(TFieldHeader));
		}
		break;
	}
	case OUTFOG_TYPE_DELAPIKEYALLOWIP:
	{
		char sApiKey[129];
		strcpy(sApiKey, pMessage->GetXTPHeader()->Remark);
		sApiKey[128] = '\0';
		CHttpSessionMap::iterator iter = g_SessionMap.find(sApiKey);
		if (iter != g_SessionMap.end())
		{
			iter->second.IPAddress.erase(pMessage->Address() + sizeof(TFieldHeader));
			REPORT_EVENT(LOG_ERROR, "DeleteApiKeyIP", "%s:%s", sApiKey, pMessage->Address() + sizeof(TFieldHeader));
		}
		else
		{
			REPORT_EVENT(LOG_ERROR, "DeleteApiKeyIP", "%s:%s -> Error Not Found!", sApiKey, pMessage->Address() + sizeof(TFieldHeader));
		}
		break;
	}
	case OUTFOG_TYPE_REGTOPIC:
	{
		m_pFlowManager->RegisterTopic(pMessage->GetXTPHeader()->SubjectNo);
		break;
	}
	case OUTFOG_TYPE_PUBLISH:
	case OUTFOG_TYPE_INSERT:
	case OUTFOG_TYPE_UPDATE:
	case OUTFOG_TYPE_DELETE:
	case OUTFOG_TYPE_INSUPD:
	case OUTFOG_TYPE_TIMER:
	case OUTFOG_TYPE_NULL:
	{
		HandleNotify(pMessage);
		break;
	}
	default:
	{
		REPORT_EVENT(LOG_CRITICAL, "Front", "HandleMessage Error: NO XTPHeader Type");
		break;
	}
	}
}

bool CFlowDispatcher::HandleNotify(CXTPPackage *pMessage)
{
	switch (pMessage->GetXTPHeader()->SubjectNo)
	{
	case OUTFOG_SUBJECTID_NULL:
	{
		break;
	}
	case OUTFOG_SUBJECTID_MULTI:
	{
		//vector<string> subs = Txtsplit(pMessage->GetXTPHeader()->Remark, ",");
		//for (int i = 0; i < subs.size(); i++)
		char* pTopic = pMessage->GetXTPHeader()->Remark;
		char* pSplit = getSplit(pMessage->GetXTPHeader()->Remark, ',');
		while (*pTopic != '\0')
		{
			//int topicid = atoi(subs[i].c_str());
			int topicid = atoi(pTopic);
			CFlow *pFlow = m_pFlowManager->GetFlow(topicid);
			if (pFlow != NULL)
			{
				//m_rspPackage.PreparePackage(pMessage->GetXTPHeader()->TNo, pMessage->GetXTPHeader()->Chain);
				//由于流水是可以并行发送的，这里很难对不同的package进行组包，所以只能是单个package
				m_rspPackage.PreparePackage(pMessage->GetXTPHeader()->MessageNo, FTDC_CHAIN_LAST);
				m_rspPackage.GetFTDCHeader()->RequestNo = pMessage->GetXTPHeader()->Type;
				m_rspPackage.GetFTDCHeader()->SubjectNo = pMessage->GetXTPHeader()->SubjectNo;
				m_rspPackage.GetFTDCHeader()->BusinessNo = pMessage->GetXTPHeader()->BusinessNo;
				strncpy(m_rspPackage.GetFTDCHeader()->SubIndexName, pMessage->GetXTPHeader()->SubIndexName, 47);
				m_rspPackage.DupPackage(pMessage);
				m_rspPackage.MakePackage();
				UF_INT8 nIndex = pFlow->Append(m_rspPackage.Address(), m_rspPackage.Length());
				if (pMessage->GetXTPHeader()->Type != OUTFOG_TYPE_TIMER)
				{
					char* pIndex = pMessage->GetXTPHeader()->SubIndexName;
					char* pSplit2 = getSplit(pMessage->GetXTPHeader()->SubIndexName, ',');
					while (*pIndex != '\0')
					{
						CIndexFlow *pIndexFlow = m_pFlowManager->GetIndexFlow(topicid, pIndex);
						if (pIndexFlow != NULL)
							pIndexFlow->Append(nIndex);
						if (pSplit2 == NULL)
							break;
						pIndex = pSplit2;
						pSplit2 = getSplit(pSplit2, ',');
					}
				}
			}
			if (pSplit == NULL)
				break;
			pTopic = pSplit;
			pSplit = getSplit(pSplit, ',');
		}
		break;
	}
	default:
	{
		CFlow *pFlow = m_pFlowManager->GetFlow(pMessage->GetXTPHeader()->SubjectNo);
		if (pFlow != NULL)
		{
			//m_rspPackage.PreparePackage(pMessage->GetXTPHeader()->TNo, pMessage->GetXTPHeader()->Chain);
			m_rspPackage.PreparePackage(pMessage->GetXTPHeader()->MessageNo, FTDC_CHAIN_LAST);
			m_rspPackage.GetFTDCHeader()->RequestNo = pMessage->GetXTPHeader()->Type;
			m_rspPackage.GetFTDCHeader()->SubjectNo = pMessage->GetXTPHeader()->SubjectNo;
			m_rspPackage.GetFTDCHeader()->BusinessNo = pMessage->GetXTPHeader()->BusinessNo;
			strncpy(m_rspPackage.GetFTDCHeader()->SubIndexName, pMessage->GetXTPHeader()->SubIndexName, 48);
			m_rspPackage.DupPackage(pMessage);
			m_rspPackage.MakePackage();
			UF_INT8 nIndex = pFlow->Append(m_rspPackage.Address(), m_rspPackage.Length());
			if (pMessage->GetXTPHeader()->Type != OUTFOG_TYPE_TIMER)
			{
				char* pIndex = pMessage->GetXTPHeader()->SubIndexName;
				char* pSplit2 = getSplit(pMessage->GetXTPHeader()->SubIndexName, ',');
				while (*pIndex != '\0')
				{
					CIndexFlow *pIndexFlow = m_pFlowManager->GetIndexFlow(pMessage->GetXTPHeader()->SubjectNo, pIndex);
					if (pIndexFlow != NULL)
						pIndexFlow->Append(nIndex);
					if (pSplit2 == NULL)
						break;
					pIndex = pSplit2;
					pSplit2 = getSplit(pSplit2, ',');
				}
			}
		}
	}
	}
	return true;
}