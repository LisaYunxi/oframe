#include "KernelApp.h"
#include "PipeEventHandler.h"

CKernelApp::CKernelApp(int nKernelid, COutFogServiceCallback* pCallback)
{
	//PrintGodBless();
	m_bRuning = true;
	m_nLatencySums = 0.0;
	m_nLatencyTimes = 0;
	//�����callback��˵����Ҫ���ⲿ����Ϣ
	//���û��callback��˵����һ�������ͨѶ�м����ֱ�ӽ�input��ˮ���Է�����
	m_pCallback = NULL;
	m_pPackageCallback = NULL;

	m_nFlowCacheSize = 0x100000;
	int nFlowCacheSize = GetConfigInt(INI_FILE_NAME, "FlowCacheSize");
	if (nFlowCacheSize > 0)
		m_nFlowCacheSize = nFlowCacheSize;

	makedir(GetConfigString(INI_FILE_NAME, "FlowPath").c_str());

	REPORT_EVENT(LOG_CRITICAL, "OMQ", "OMQ version is %s-%s", INNER_BASE_VERSION, INNER_DUE_DATE);
	time_t dwCurrTime = time(NULL);
	char szLocalDate[9] = { '\0' };
	struct tm tt;
	LOCALTIME(&tt, &dwCurrTime);
	strftime(szLocalDate, sizeof(szLocalDate), "%Y%m%d", &tt);
	int nToday = atoi(szLocalDate);
	int nDueDate = atoi(INNER_DUE_DATE);
	int nDueDays = nToday - nDueDate;
	if (nDueDays > 0)
	{
		//datediff(nToday, nDueDate, nDueDays);
		REPORT_EVENT(LOG_CRITICAL, "OMQ", "OMQ Expire, Exit!");
		exit(-1);
	}

	m_nKernelid = nKernelid;
	m_CompStateID = CS_INIT;

	m_pPreResult = NULL;
	m_pTradeResult = NULL;
	m_pResultPubFlow = NULL;

	m_pPreSeries = NULL;
	m_pTradeSeries = NULL;

	m_pubPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
	m_pPackage = new CXTPPackage();
	m_pPackage->ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);

	m_bStartAddin = false;
	m_pLinkManager = NULL;
	m_pGatewayFront = NULL;
	m_bFinishInitInstance = false;
	m_pKernelFront = NULL;
	m_pHttpFront = NULL;
	m_pServiceLoad = new COutFogService();

	m_pLinkManager = new CLinkManager();
	m_pFlowManager = new CFlowManager();

	m_nReliableMode = GetConfigInt(INI_FILE_NAME, "ReliableMode");

	memset(&m_OutFogMetaRef, 0, sizeof(TOutFogMetaRef));
	m_nCount = UF_INT8_ZERO;
	m_nReDoID = UF_INT8_ZERO;

	m_nCPUMode = GetConfigInt(INI_FILE_NAME, "CPUMode");

	// ����һЩ���Դ�����

	m_pTradeSeries = new CCachedFlow(true, "inputall_cache", m_nFlowCacheSize, 200000);

	if (GetConfigString(INI_FILE_NAME, "PersistInputFlow") == "yes")
	{
		CFileFlow* pFlow = new CFileFlow("inputall", GetConfigString(INI_FILE_NAME, "FlowPath").c_str(), true, GetConfigInt(INI_FILE_NAME, "PersistGap"));
		m_pTradeSeries->AttachUnderFlow(pFlow);
	}

	if (m_nReliableMode == ReliableMode_HotStandby)
	{
		m_pPreSeries = new CCachedFlow(false, "Inputall_pre", m_nFlowCacheSize * 2, 200000);
		m_pPreSeries->AttachUnderFlow(m_pTradeSeries);
	}
	else
	{
		m_pPreSeries = m_pTradeSeries;
	}

	m_pCallback = pCallback;
	if (m_pCallback != NULL)
	{
		m_pPreResult = new CCachedFlow(true, "outputall_cache", m_nFlowCacheSize * 4, 200000);
		if (GetConfigString(INI_FILE_NAME, "PersistOtherFlows") == "yes")
		{
			CFileFlow* pFlow = new CFileFlow("outputall", GetConfigString(INI_FILE_NAME, "FlowPath").c_str(), true, GetConfigInt(INI_FILE_NAME, "PersistGap"));
			m_pPreResult->AttachUnderFlow(pFlow);
		}
	}
	else	//���û��callback��˵������Ҫ�����ģ�ֱ�ӷ���input��ˮ������Ҫoutput��ˮ
	{
		m_pPreResult = m_pTradeSeries;
	}

	//����ģʽ��Result��ˮ��Ҫȷ��֮����ܱ��ַ�
	if (m_nReliableMode == ReliableMode_HotStandby)
	{
		//����ģʽ���棬���뵽һ�����ļ�������ַܷ�������ʱ������֪ͨ�����Ǵ�������һ���½�
		m_pResultPubFlow = new CPubFlow(m_pPreResult);
		m_pTradeResult = m_pResultPubFlow;
		m_pFlowDispatcher = new CFlowDispatcher(m_pTradeResult, m_pFlowManager, m_nKernelid);
		//Ϊ����߷ַ�Ч��ʹ���ź������±��ĵ���ʱ�����ѷַ��̹߳���
		//CPipeEventHandler *pPipe_TResult_Front = new CPipeEventHandler(m_pFlowDispatcher);
		//m_pTradeResult->AttachObserver(pPipe_TResult_Front);
	}
	else
	{
		//����ʱģʽ��Ϊ�������ʱ�����߳�ֱ�ӷַ������̴߳�������������
		//�ǵ���ʱ���棬��Ҫ�����ַ��̣߳����߳��ܴ�����ಢ��
		m_pTradeResult = m_pPreResult;
		m_pFlowDispatcher = new CFlowDispatcher(m_pTradeResult, m_pFlowManager, m_nKernelid);
		if (!g_bLowLatency)
		{
			//CPipeEventHandler *pPipe_TResult_Front = new CPipeEventHandler(m_pFlowDispatcher);
			//m_pTradeResult->AttachObserver(pPipe_TResult_Front);
			m_pFlowDispatcher->Create();
		}
	}
}

void CKernelApp::SetKernelAppCallback(CKernelAppCallback* pCallback)
{
	m_pPackageCallback = pCallback;
}

bool CKernelApp::InitInstance()
{
	//�ж����redoid������ˮ����
	//����ΪʲôҪ��1�أ�����Ǳ�ϵͳ���ļ�����ʱ�ļ���Count��m_nReDoID��1�����ں����Ჹ�䵽Count���棬���Բ��õ���û�и�����������
	if (m_nReDoID > m_pPreSeries->GetCount() + 1)
	{
		PRINT_TO_STD("RedoID[%0.f] is bigger than input flow last id[%0.f], exit!", m_nReDoID, m_pPreSeries->GetCount());
		exit(-1);
	}

	//�ڴ��������²���ˮ��ȡ���ݵ�����£�CachedFlow 115��������£���Ҫ�ж�ReDoID��FirstID�Ժ�
	//if (m_nReDoID < m_pPreSeries->GetFirstID())
	//{
	//	REPORT_EVENT(LOG_CRITICAL, "KernelApp", "RedoID[%0.f] is smaller than input flow first id[%0.f], exit!", m_nReDoID, m_pPreSeries->GetFirstID());
	//	exit(-1);
	//}

	//��ʱ�Ѿ���ȡ���ϲ��redoid������ͨ�����id�������ļ�
	m_reader.AttachFlow(m_pPreSeries, m_nReDoID);
	FirstReDo();
	return true;
}

void CKernelApp::StartAddin()
{
	//������������������������߳�-ǰ�û���api����
	char FrontName[256];
	sprintf(FrontName, "front_%s_%d", GetConfigString(INI_FILE_NAME, "AppName").c_str(), m_nKernelid);
	string pFrontAddress = GetConfigString(NetworkConfigFile, FrontName);
	if (pFrontAddress.size() >= 15)
	{
		int nFrontNum = 1;
		vector<string> sSplitFirst = Txtsplit(pFrontAddress, "#");
		if (sSplitFirst.size() >= 2)
			nFrontNum = atoi(sSplitFirst[1].c_str());
		for (int i = 1; i <= nFrontNum; i++)
		{
			CFlow *pFlow = new COneTimeFlow();
			int nFrontID = m_nKernelid * 100 - LINK_KERNEL_FTDFRONT + i - 1;
			if (GetConfigString(INI_FILE_NAME, "PersistOtherFlows") == "yes")
			{
				char FrontFlowName[256];
				sprintf(FrontFlowName, "input_front_%d", -nFrontID);
				CFileFlow* pFileFlow = new CFileFlow(FrontFlowName, GetConfigString(INI_FILE_NAME, "FlowPath").c_str(), false, GetConfigInt(INI_FILE_NAME, "PersistGap"));
				pFlow->AttachUnderFlow(pFileFlow);
			}
			CFlow *pFrontFlow = m_pFlowManager->RegisterTopic(-nFrontID, "front", new COneTimeFlow());
			m_pKernelFront = new CFtdFront(pFrontFlow, pFlow, m_pFlowManager, sSplitFirst[0].c_str(), -nFrontID, i);
			m_pLinkManager->setLinkApi(LINK_KERNEL_FTDFRONT + i - 1, m_pKernelFront, pFlow, NULL);
		}
	}
	else
	{
		REPORT_EVENT(LOG_CRITICAL, "Front", "Error: No Front Port!");
	}

	//����������������·�ɷ����߳�-�ڲ�����
	char GateWayName[256];
	sprintf(GateWayName, "gateway_%s_%d", GetConfigString(INI_FILE_NAME, "AppName").c_str(), m_nKernelid);
	string pGatewayAddress = GetConfigString(NetworkConfigFile, GateWayName);
	if (pGatewayAddress.size() >= 15)
	{
		int nFrontNum = 1;
		vector<string> sSplitFirst = Txtsplit(pGatewayAddress, "#");
		if (sSplitFirst.size() >= 2)
			nFrontNum = atoi(sSplitFirst[1].c_str());
		int sSplitThreads = 3;
		if (sSplitFirst.size() >= 3)
			sSplitThreads = atoi(sSplitFirst[2].c_str());
		for (int i = 1; i <= nFrontNum; i++)
		{
			CFlow *pFlow = new COneTimeFlow("input_gateway");
			if (GetConfigString(INI_FILE_NAME, "PersistOtherFlows") == "yes")
			{
				CFileFlow* pFileFlow = new CFileFlow("input_gateway", GetConfigString(INI_FILE_NAME, "FlowPath").c_str(), false, GetConfigInt(INI_FILE_NAME, "PersistGap"));
				pFlow->AttachUnderFlow(pFileFlow);
			}
			m_pGatewayFront = new CGateWayFront(pFlow, pGatewayAddress.c_str(), i, sSplitThreads);
			//ͨ��gateway����TradeResult��ˮ
			if (GetConfigString(INI_FILE_NAME, "GateWayOutputFlow") == "yes")
			{
				m_pGatewayFront->RegisterFlow(m_pTradeResult, XtpLinkFlowOutput);
				//����Tresult֮����GatewayFront֮���Pipe
				//CPipeEventHandler *pPipe_TResult_GateFront = new CPipeEventHandler(m_pGatewayFront);
				//m_pTradeResult->AttachObserver(pPipe_TResult_GateFront);

			}
			//ͨ��gateway����TradeSeries��ˮ
			if (GetConfigString(INI_FILE_NAME, "GateWayInputFlow") == "yes")
			{
				m_pGatewayFront->RegisterFlow(m_pPreSeries, XtpLinkFlowInput);
			}
			m_pLinkManager->setLinkApi(LINK_KERNEL_GATEWAY + i - 1, m_pGatewayFront, pFlow, NULL);
		}
	}
	else
	{
		PRINT_TO_STD("CGateWayFront 0 : Error: No GateWay Port!");
	}

	//������������������������߳�-ǰ�û���api����
	char HttpFrontName[256];
	sprintf(HttpFrontName, "httpfront_%s_%d", GetConfigString(INI_FILE_NAME, "AppName").c_str(), m_nKernelid);
	string pHttpFrontAddress = GetConfigString(NetworkConfigFile, HttpFrontName);
	if (pHttpFrontAddress.size() >= 1)
	{
		int nFrontNum = 1;
		vector<string> sSplitFirst = Txtsplit(pHttpFrontAddress, "#");
		if (sSplitFirst.size() >= 2)
			nFrontNum = atoi(sSplitFirst[1].c_str());
		for (int i = 1; i <= nFrontNum; i++)
		{
			CFlow *pFlow = new COneTimeFlow();
			int nFrontID = m_nKernelid * 100 - LINK_KERNEL_HTTPFRONT + i - 1;
			if (GetConfigString(INI_FILE_NAME, "PersistOtherFlows") == "yes")
			{
				char FrontFlowName[256];
				sprintf(FrontFlowName, "input_httpfront_%d", -nFrontID);
				CFileFlow* pFileFlow = new CFileFlow(FrontFlowName, GetConfigString(INI_FILE_NAME, "FlowPath").c_str(), false, GetConfigInt(INI_FILE_NAME, "PersistGap"));
				pFlow->AttachUnderFlow(pFileFlow);
			}
			CFlow *pFrontFlow = m_pFlowManager->RegisterTopic(-nFrontID, "httpfront", new COneTimeFlow());
			m_pHttpFront = new CXHttpFront(pFrontFlow, pFlow, m_pFlowManager, sSplitFirst[0].c_str(), -nFrontID, i);

			char DataswapName[256];
			sprintf(DataswapName, "dataswap_%s_%d", GetConfigString(INI_FILE_NAME, "AppName").c_str(), m_nKernelid);
			string sDataswapDll = GetConfigString(NetworkConfigFile, DataswapName);
			if (sDataswapDll == "")
				sDataswapDll = "dataswap:2";
			COutFogServiceApi *pDataSwapDll = m_pServiceLoad->getOutFogService(sDataswapDll.c_str());
			if (pDataSwapDll == NULL)
			{
				PRINT_TO_STD("HttpFront: Error: Cannnot get %s.dll/lib%s.so", sDataswapDll.c_str(), sDataswapDll.c_str());
				exit(-1);
			}
			else
			{
				PRINT_TO_STD("HttpFront: get %s.dll/lib%s.so", sDataswapDll.c_str(), sDataswapDll.c_str());
				m_pHttpFront->setDataSwapApi(pDataSwapDll);
			}
			m_pLinkManager->setLinkApi(LINK_KERNEL_HTTPFRONT + i - 1, m_pHttpFront, pFlow, NULL);
		}
	}
	else
	{
		PRINT_TO_STD("Front: Error: No HttpFront Port!");
	}

	//������������������������߳�-ǰ�û���api����
	char WsFrontName[256];
	sprintf(WsFrontName, "wsfront_%s_%d", GetConfigString(INI_FILE_NAME, "AppName").c_str(), m_nKernelid);
	string pWsFrontAddress = GetConfigString(NetworkConfigFile, WsFrontName);
	if (pWsFrontAddress.size() >= 1)
	{
		int nFrontNum = 1;
		vector<string> sSplitFirst = Txtsplit(pWsFrontAddress, "#");
		if (sSplitFirst.size() >= 2)
			nFrontNum = atoi(sSplitFirst[1].c_str());
		int sSplitThreads = 1;
		if (sSplitFirst.size() >= 3)
			sSplitThreads = atoi(sSplitFirst[2].c_str());
		for (int i = 1; i <= nFrontNum; i++)
		{
			CFlow *pFlow = new COneTimeFlow();
			int nFrontID = m_nKernelid * 100 - LINK_KERNEL_WSFRONT + i - 1;
			if (GetConfigString(INI_FILE_NAME, "PersistOtherFlows") == "yes")
			{
				char FrontFlowName[256];
				sprintf(FrontFlowName, "input_wsfront_%d", -nFrontID);
				//CFileFlow* pFileFlow = new CFileFlow(FrontFlowName, m_pConfig->getConfig("FlowPath"), false, atoi(m_pConfig->getConfig("PersistGap")));
				CFileFlow* pFileFlow = new CFileFlow(FrontFlowName, GetConfigString(INI_FILE_NAME, "FlowPath").c_str(), false, GetConfigInt(INI_FILE_NAME, "PersistGap"));
				pFlow->AttachUnderFlow(pFileFlow);
			}
			CFlow *pFrontFlow = m_pFlowManager->RegisterTopic(-nFrontID, "wsfront", NULL);
			m_pWsFront = new CWSFront(pFrontFlow, pFlow, m_pFlowManager, sSplitFirst[0].c_str(), -nFrontID, i, sSplitThreads);

			char DataswapName[256];
			sprintf(DataswapName, "dataswap_%s_%d", GetConfigString(INI_FILE_NAME, "AppName").c_str(), m_nKernelid);
			string sDataswapDll = GetConfigString(NetworkConfigFile, DataswapName);
			if (sDataswapDll == "")
				sDataswapDll = "dataswap:2";
			COutFogServiceApi *pDataSwapDll = m_pServiceLoad->getOutFogService(sDataswapDll.c_str());
			if (pDataSwapDll == NULL)
			{
				PRINT_TO_STD("WsFront: Error: Cannnot get %s.dll/lib%s.so", sDataswapDll.c_str(), sDataswapDll.c_str());
				exit(-1);
			}
			else
			{
				PRINT_TO_STD("WsFront: get %s.dll/lib%s.so", sDataswapDll.c_str(), sDataswapDll.c_str());
				m_pWsFront->setDataSwapApi(pDataSwapDll);
			}
			m_pLinkManager->setLinkApi(LINK_KERNEL_WSFRONT + i - 1, m_pWsFront, pFlow, NULL);
		}
	}
	else
	{
		PRINT_TO_STD("Front: Error: No WsFront Port!");
	}

	//�������������������������߳�-�����ڲ��ϲ�����
	char LinkNumName[256];
	//sprintf(LinkNumName, "links_%s_%d", m_pConfig->getConfig("AppName"), m_nKernelid);
	sprintf(LinkNumName, "links_%s_%d", GetConfigString(INI_FILE_NAME, "AppName").c_str(), m_nKernelid);
	vector<string> slinks = Txtsplit(GetConfigString(NetworkConfigFile, LinkNumName), "^");
	int linkInnerBegin = LINK_INNER_BEGIN;

	for (int i = 0; i < slinks.size(); i++)
	{
		vector<string> slinkport = Txtsplit(slinks[i], "|");
		if (slinkport.size() < 2)
		{
			PRINT_TO_STD("Front: Error: %s is error not enough param!", slinks[i].c_str());
			exit(-1);
		}
		UF_INT8 nSequenceNo = 0.0;
		if (slinkport.size() == 3)
			nSequenceNo = atoi(slinkport[2].c_str());
		m_pLinkManager->initOfferLink(linkInnerBegin, slinkport[1].c_str(), atoi(slinkport[0].c_str()), nSequenceNo);
		linkInnerBegin--;
	}

	//��ʱ���趨
	int nTimerInterval = GetConfigInt(INI_FILE_NAME, "TimerInterval");
	if (nTimerInterval > 0)
	{
		CFlow *pFlow = new COneTimeFlow();
		if (GetConfigString(INI_FILE_NAME, "PersistOtherFlows") == "yes")
		{
			//CFileFlow* pFileFlow = new CFileFlow("input_timer", m_pConfig->getConfig("FlowPath"), false, atoi(m_pConfig->getConfig("PersistGap")));
			CFileFlow* pFileFlow = new CFileFlow("input_timer", GetConfigString(INI_FILE_NAME, "FlowPath").c_str(), false, GetConfigInt(INI_FILE_NAME, "PersistGap"));
			pFlow->AttachUnderFlow(pFileFlow);
		}
		CSelectReactor* pSelectReactor = new CSelectReactor();
		CKernelTimer* pKernelTimer = new CKernelTimer(pSelectReactor, nTimerInterval, pFlow);
		m_pLinkManager->setLinkApi(LINK_KERNEL_TIMER, pKernelTimer, pFlow, NULL);
	}

	m_pLinkManager->startWork();
}

bool CKernelApp::RspField(const TOutFogMetaRef* pOutFogRspRef, const void *pStream)
{
	//REPORT_EVENT(LOG_CRITICAL, "OMQ", "Handel %0.f:rsp_begin", m_nCount);
	switch (pOutFogRspRef->Direction)
	{
	case OUTFOG_DIRECTION_TO_FRONT:
	{
		switch (pOutFogRspRef->Chain)
		{
		case OUTFOG_CHAIN_FIRST:
		{
			m_pPackage->PreparePublish(pOutFogRspRef->MessageNo, OUTFOG_CHAIN_FIRST);
			CopyHeadMeta(m_pPackage->GetXTPHeader(), pOutFogRspRef);
			m_pPackage->GetXTPHeader()->GlobalNo = m_nCount;
			m_pPackage->AddStream(pOutFogRspRef->FieldNo, (char *)pStream, pOutFogRspRef->Length);
			break;
		}
		case OUTFOG_CHAIN_CONTINUE:
		{
			if (!m_pPackage->AddStream(pOutFogRspRef->FieldNo, (char *)pStream, pOutFogRspRef->Length))
			{
				//CopyHeadMeta(m_pPackage->GetXTPHeader(), pOutFogRspRef);
				m_pPackage->GetXTPHeader()->GlobalNo = m_nCount;
				AppendToFlow(m_pPackage);
				m_pPackage->PreparePublish(pOutFogRspRef->MessageNo, OUTFOG_CHAIN_CONTINUE);
				CopyHeadMeta(m_pPackage->GetXTPHeader(), pOutFogRspRef);
				m_pPackage->GetXTPHeader()->GlobalNo = m_nCount;
				m_pPackage->AddStream(pOutFogRspRef->FieldNo, (char *)pStream, pOutFogRspRef->Length);
			}
			break;
		}
		case OUTFOG_CHAIN_LAST:
		{
			if (m_pPackage->AddStream(pOutFogRspRef->FieldNo, (char *)pStream, pOutFogRspRef->Length))
			{
				if (m_pPackage->GetXTPHeader()->Chain == OUTFOG_CHAIN_FIRST)
					m_pPackage->GetXTPHeader()->Chain = OUTFOG_CHAIN_SINGLE;
				if (m_pPackage->GetXTPHeader()->Chain == OUTFOG_CHAIN_CONTINUE)
					m_pPackage->GetXTPHeader()->Chain = OUTFOG_CHAIN_LAST;

				m_pPackage->GetXTPHeader()->GlobalNo = m_nCount;
				AppendToFlow(m_pPackage);
			}
			else
			{
				if (m_pPackage->GetXTPHeader()->Chain == OUTFOG_CHAIN_LAST)
					m_pPackage->GetXTPHeader()->Chain = OUTFOG_CHAIN_CONTINUE;
				if (m_pPackage->GetXTPHeader()->Chain == OUTFOG_CHAIN_SINGLE)
					m_pPackage->GetXTPHeader()->Chain = OUTFOG_CHAIN_FIRST;
				m_pPackage->GetXTPHeader()->GlobalNo = m_nCount;
				AppendToFlow(m_pPackage);

				m_pPackage->PreparePublish(pOutFogRspRef->MessageNo, OUTFOG_CHAIN_LAST);
				CopyHeadMeta(m_pPackage->GetXTPHeader(), pOutFogRspRef);
				m_pPackage->GetXTPHeader()->GlobalNo = m_nCount;
				m_pPackage->AddStream(pOutFogRspRef->FieldNo, (char *)pStream, pOutFogRspRef->Length);
				AppendToFlow(m_pPackage);
			}
			break;
		}
		case OUTFOG_CHAIN_SINGLE:
		{
			m_pPackage->PreparePublish(pOutFogRspRef->MessageNo, OUTFOG_CHAIN_SINGLE);
			CopyHeadMeta(m_pPackage->GetXTPHeader(), pOutFogRspRef);
			m_pPackage->GetXTPHeader()->GlobalNo = m_nCount;
			m_pPackage->AddStream(pOutFogRspRef->FieldNo, (char *)pStream, pOutFogRspRef->Length);
			AppendToFlow(m_pPackage);
			break;
		}
		default:
		{
			REPORT_EVENT(LOG_CRITICAL, "ServiceCallback", "Error: Service Package[SequenceNo:%.0f,FieldNo:%d] No Chain", pOutFogRspRef->SequenceNo, pOutFogRspRef->FieldNo);
		}
		}
		break;
	}
	case OUTFOG_DIRECTION_TO_OFFER:
	{
		m_pLinkManager->ReqField(pOutFogRspRef, pStream);
		break;
	}
	case OUTFOG_DIRECTION_TO_KERNEL:
	default:
	{
		if (pOutFogRspRef->Type == OUTFOG_TYPE_INIT)
		{
			//SetReDoID(pOutFogRspRef->SequenceNo);
			m_nReDoID = pOutFogRspRef->SequenceNo;
		}
		else
		{
			REPORT_EVENT(LOG_CRITICAL, "ServiceCallback", "Error: Service Package[SequenceNo:%.0f,FieldNo:%d] No Direction", pOutFogRspRef->SequenceNo, pOutFogRspRef->FieldNo);
		}
	}
	}
	return true;
}

bool CKernelApp::RspField(CXTPPackage *pMessage)
{
	//REPORT_EVENT(LOG_CRITICAL, "OMQ", "Handel %0.f:rsp_begin", m_nCount);
	switch (pMessage->GetXTPHeader()->Direction)
	{
	case OUTFOG_DIRECTION_TO_FRONT:
	{
		AppendToFlow(pMessage);
		break;
	}
	case OUTFOG_DIRECTION_TO_OFFER:
	{
		m_pLinkManager->ReqField(pMessage);
		break;
	}
	case OUTFOG_DIRECTION_TO_KERNEL:
	default:
	{
		REPORT_EVENT(LOG_CRITICAL, "ServiceCallback", "Error: Service Package[SequenceNo:%.0f,TNo:%d] No Direction",
			pMessage->GetXTPHeader()->SequenceNo, pMessage->GetXTPHeader()->MessageNo);
	}
	}
	return true;
}

bool CKernelApp::AppendToFlow(CXTPPackage *pMessage)
{
	//	REPORT_EVENT(LOG_CRITICAL, "OMQ", "Handel %0.f:rsp_3", pMessage->GetXTPHeader()->GlobalNo);
	pMessage->MakePackage();
	// ������������������ͬһ����ˮ��û�б�Ҫ�ټ�¼һ��
	if (m_pPreResult != NULL && m_pPreResult != m_pTradeSeries)
		m_pPreResult->Append(pMessage->Address(), pMessage->Length());

	//����ģʽͨ�������̷ַ߳�
	//����ģʽ����£������˵���ʱ����ֱ�ӷַ�
	if (m_nReliableMode != ReliableMode_HotStandby && g_bLowLatency)
	{
		pMessage->ValidPackage();
		m_pFlowDispatcher->HandleMessage(pMessage);
	}

	//int a = 0;
	//GET_ONLY_MSEC_TIME(a);
	//REPORT_EVENT(LOG_CRITICAL, "AppendToFlow", "%0.f:%d", pMessage->GetXTPHeader()->GlobalNo, a);

	//Ϊ�˷�ֹpackage��Append֮�󻹱�����ʹ�ã�������Ҫ�ָ�һЩ��Ҫ�ı���������core
	//�������һ��Last����֮�󣬴�����ͣ���һ��������continue��Ҳ�ܴ���������
	//Package��Ҫ�������Ҫ��Ȼ�������û�з���First��Single��Щ��ʼ������(��Щ��omq�޷����Ƶ�)��package���ҵ�
	pMessage->PreparePublish(0, OUTFOG_CHAIN_FIRST);
	//	REPORT_EVENT(LOG_CRITICAL, "OMQ", "Handel %0.f:rsp_end", pMessage->GetXTPHeader()->GlobalNo);
	return true;
}

//void CKernelApp::SetReDoID(UF_INT8 nID)
//{
//	m_nReDoID = nID;
//}

void CKernelApp::FirstReDo()
{
	PRINT_TO_STD("Redo,from %0.f !", m_nReDoID);
	int nBeginMS = time(NULL);
	//GET_ACCURATE_MSEC_TIME(nBeginMS);
	int nCount = Redo();
	int nEndMS = time(NULL);
	//GET_ACCURATE_MSEC_TIME(nEndMS);
	//UF_INT8 toCount = m_nReDoID + nCount;
	PRINT_TO_STD("Redo,to %0.f !", m_nCount);
	//double nGapMS = nEndMS - nBeginMS;
	//double seconds = nGapMS / 1000;
	int seconds = nEndMS - nBeginMS;
	if (seconds != 0.0)
	{
		double speed = nCount / seconds;
		PRINT_TO_STD("use %d seconds to deal %0.f packages of %d fields,speed is %f /s", seconds, m_nCount - m_nReDoID + 1, nCount, speed);
	}

	//���ݽ���֪ͨ�ϲ㣬�����ϲ������ж�
	m_OutFogMetaRef.IsReplay = false;
	m_OutFogMetaRef.Type = OUTFOG_TYPE_REDOOVER;
	if (m_pCallback != NULL)
		m_pCallback->RspField(&m_OutFogMetaRef, NULL);

	if (m_nReliableMode == ReliableMode_HotStandby)
	{
		//������series��id���������Ϣ�ڴ�Ͻ������������Ϣ��id�Ķ�Ӧ��ϵ
		ConfirmQueued(m_pPreSeries->GetCount());
		m_pResultPubFlow->SetSIDAndRID(m_pPreSeries->GetCount(), m_pPreResult->GetCount());
	}

	if (m_nReliableMode != ReliableMode_HotStandby)
	{
		StartAddin();
	}

	m_bFinishInitInstance = true;
	//redo����֮�����Ҫд��־���������Բ���ԭϵͳ����־��ͻ
	CMonitorIndex::init(CFilelogService::CreateInstance(GetConfigString(INI_FILE_NAME, "SyslogPath").c_str()), GetConfigString(INI_FILE_NAME, "LogLevel").c_str());
}

int CKernelApp::Redo()
{
	m_OutFogMetaRef.IsReplay = true;
	int i = 1;
	int nFieldCount = 0;

	//���û��callback��Ҫ���ã�����Ҫ���κζ���
	if (m_pCallback != NULL)
	{
		for (;; i++)
		{
			if (!m_reader.GetNext(&m_pubPackage))
				break;
			m_nCount = m_pubPackage.GetXTPHeader()->SequenceNo;
			if (m_pCallback != NULL)
				nFieldCount += handleMessage(&m_pubPackage);
			if (m_pPackageCallback != NULL)
				m_pPackageCallback->RspPackage(&m_pubPackage);
		}
	}

	//��ֹ���ȱ�ģʽ�¿�ת̫����
	if (1 == i)
	{
		SleepMs(250);
	}
	return nFieldCount;
}

void CKernelApp::RealDo()
{
	//static int nOmqSpeed = 0;
	static char cSpeedSecond = '0';
	m_OutFogMetaRef.IsReplay = false;

	while (true)
	{
		///��̽�Ƿ���Զ���ˮ
		if (!m_pLinkManager->GetNextMessage(&m_pubPackage))
		{
			if (m_nCPUMode >= 0)
			{
#ifdef WINDOWS
				if (m_nCPUMode < 1000)
					Sleep(1);
				else
					Sleep(m_nCPUMode / 1000);
#else
				if (m_nCPUMode == 0)
					::usleep(1000);
				else
					::usleep(m_nCPUMode);
#endif
			}
			break;
		}

		//��ϵͳ���Ա��ģ�Ϊ��ֹͣϵͳ��ʱ�򣬲�������������ģ����·��ݳ�����
		if (m_pubPackage.GetXTPHeader()->Type == OUTFOG_TYPE_START)
		{
			m_bRuning = true;
			REPORT_EVENT(LOG_EMERGENCY, "OMQ", "Start to Deal Messages");
		}

		if (!m_bRuning)
			break;

		if (m_pubPackage.GetXTPHeader()->Type == OUTFOG_TYPE_STOP)
		{
			m_bRuning = false;
			REPORT_EVENT(LOG_EMERGENCY, "OMQ", "Stop Deal Messages");
		}

		GET_ONLY_USEC_TIME(m_CurrPackageTime);

		if (m_pPreSeries != NULL)
			m_nCount = m_pPreSeries->GetCount();

		m_pubPackage.GetXTPHeader()->SequenceNo = m_nCount;

		if (m_pubPackage.GetXTPHeader()->Type == OUTFOG_TYPE_TIMER)
		{
			//			if (m_pubPackage.GetXTPHeader()->UserID[7] == '0')
			//			{
			if (cSpeedSecond != m_pubPackage.GetXTPHeader()->UserID[7])
			{
				cSpeedSecond = m_pubPackage.GetXTPHeader()->UserID[7];
				if (m_nLatencyTimes > 0)
					REPORT_EVENT(LOG_WARNING, "OMQ", "Deal Messages in 1s: %d of %0.f as %0.f us", m_nLatencyTimes, m_nCount, m_nLatencySums / m_nLatencyTimes);
				else
					REPORT_EVENT(LOG_WARNING, "OMQ", "Deal Messages in 1s: %d of %0.f", m_nLatencyTimes, m_nCount);
				//nOmqSpeed = 0;
				m_nLatencyTimes = 0;
				m_nLatencySums = 0.0;
				if (m_pubPackage.GetXTPHeader()->UserID[7] == '5')
					m_pLinkManager->OnTimer();
			}
			//			}

			if (strcmp(m_pubPackage.GetXTPHeader()->SubIndexName, INNER_DUE_DATE) > 0)
			{
				//datediff(nToday, nDueDays, nDueDays);
				//int nSleepTime = nDueDays / 50 + 1;
				//REPORT_EVENT(LOG_CRITICAL, "OMQ", "OMQ Expire��Expire Date Is :%d��Sleep %d ms(windows) / %d us(linux) !", nDueDate, nSleepTime, nDueDays);
				REPORT_EVENT(LOG_CRITICAL, "OMQ", "OMQ Expire,Exit!");
				exit(-1);
			}
		}

		if (m_pPreSeries != NULL)
		{
			m_pubPackage.MakePackage();
			//��ֹ����û�п�����ˮ�źͷ��ݺŲ�һ���������Ӧ245�е���һ���Ĵ���
			if (m_pPreSeries->Append(m_pubPackage.Address(), m_pubPackage.Length()) < m_nReDoID)
				break;
			m_pubPackage.ValidPackage();
		}

		//���û��callback���򲻴���
		if (m_pCallback != NULL)
			m_nLatencyTimes += handleMessage(&m_pubPackage);

		//���������Ҫ�ŵ���ˮ���棬������Ҫ��handleMessage�ĺ���
		if (m_pPackageCallback != NULL)
			m_pPackageCallback->RspPackage(&m_pubPackage);

		int dealend;
		GET_ONLY_USEC_TIME(dealend);

		if (dealend < m_CurrPackageTime)
			m_nLatencySums += dealend + 1000000 - m_CurrPackageTime;
		else
			m_nLatencySums += dealend - m_CurrPackageTime;

		//m_nLatencyTimes++;
		//nOmqSpeed++;

		//if (m_pPreSeries != NULL)
		//{
		//	int dealend;
		//	GET_ONLY_USEC_TIME(dealend);
		//	if (dealend < m_CurrPackageTime)
		//	{
		//		m_pubPackage.GetXTPHeader()->DealUsec = dealend + 60000000 - m_CurrPackageTime;
		//	}
		//	else
		//	{
		//		m_pubPackage.GetXTPHeader()->DealUsec = dealend - m_CurrPackageTime;
		//	}
		//	m_pubPackage.MakePackage();
		//	m_pPreSeries->Append(m_pubPackage.Address(), m_pubPackage.Length());
		//	//m_pubPackage.ValidPackage();
		//}

		// 		long ltm112;
		// 		GET_ONLY_USEC_TIME(ltm112);
		// 		REPORT_EVENT(LOG_CRITICAL,"LoopTime","TID:%08x,ltm112: %d",m_pubPackage.GetTid(),ltm112);	

		//���㹤��ʱӦ���Լ�ȷ��result�����Ա�������������ǰ�û�
		if (CS_SINGLE == m_CompStateID)
		{
			for (UF_INT8 nKey = m_pTradeSeries->GetCount(); nKey < m_pPreSeries->GetCount(); nKey++)
			{
				if (nKey < m_pPreSeries->GetUnderCount())	//�Ѿ�ȷ�Ϲ��ˣ�ֱ�ӷ���				
					return;

				if (m_pPreSeries->SyncUnderFlow(nKey))
					ConfirmQueued(nKey);
				else
					REPORT_EVENT(LOG_CRITICAL, "InternalError", "Confirm queued failed [%d]", nKey);
			}
		}
		if (m_nReliableMode == ReliableMode_HotStandby)
			m_pResultPubFlow->SetSIDAndRID(m_pPreSeries->GetCount(), m_pPreResult->GetCount());
	}
}

UF_INT8 CKernelApp::GetHandleCount()
{
	return m_reader.GetId();
}

void CKernelApp::ExitInstance()
{
	delete m_pTradeResult;
	delete m_pTradeSeries;
}

int CKernelApp::handleMessage(CXTPPackage *pPackage)
{
	CopyHeadMeta(&m_OutFogMetaRef, pPackage->GetXTPHeader());
	m_OutFogMetaRef.Chain = pPackage->GetXTPHeader()->Chain;

	CFieldTypeIterator iter = pPackage->GetFieldTypeIterator();
	if (iter.IsEnd())
	{
		m_pCallback->RspField(&m_OutFogMetaRef, NULL);
		return 1;
	}
	else
	{
		int nCount = 0;
		while (!iter.IsEnd())
		{
			nCount++;
			TFieldHeader iterHeader = iter.Retrieve(pStreamBuf);
			m_OutFogMetaRef.FieldNo = iterHeader.FieldID;
			m_OutFogMetaRef.Length = iterHeader.Size;
			m_pCallback->RspField(&m_OutFogMetaRef, pStreamBuf);
			iter.Next();
		}
		return nCount;
	}
}

void CKernelApp::Run()
{
	while (true)
	{
		if (m_nReliableMode == ReliableMode_HotStandby)
		{
			switch (m_CompStateID)
			{
			case CS_INIT:
			case CS_PREPARE:
			{
				SleepMs(100);
				break;
			}
			case CS_SECONDARY:
			{
				Redo();
				break;
			}
			case CS_PRIMARY:
			case CS_SINGLE:
			{
				RealDo();
				break;
			}
			default:
			{
				REPORT_EVENT(LOG_CRITICAL, "kernel", "this state don't work");
				break;
			}
			}
		}
		else
		{
			RealDo();
		}
	}
}

bool CKernelApp::EnterSlaver()
{
	m_CompStateID = CS_SECONDARY;
	REPORT_EVENT(LOG_CRITICAL, "QTradeApp", "Enter into CS_SECONDARY");
	return true;
}

bool CKernelApp::EnterSingle()
{
	if (!m_bStartAddin)
	{
		//��ֹ��CS_PRIMARY��CS_SINGLEʱ����һ��
		StartAddin();
		m_bStartAddin = true;
	}

	m_CompStateID = CS_SINGLE;
	REPORT_EVENT(LOG_CRITICAL, "QTradeApp", "Enter into CS_SINGLE");
	return true;
}

bool CKernelApp::EnterMaster()
{
	m_CompStateID = CS_PRIMARY;
	REPORT_EVENT(LOG_CRITICAL, "QTradeApp", "Enter into CS_PRIMARY");
	return true;
}

TCompStateID CKernelApp::GetCompStateID()
{
	return m_CompStateID;
}

void CKernelApp::ConfirmQueued(UF_INT8 nKey)
{
	m_pResultPubFlow->ConfirmQueued(nKey);
}
