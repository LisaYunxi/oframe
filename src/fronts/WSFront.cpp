#include "WSFront.h"
#include "platform.h"
#include "BaseFunction.h"

CWSFront* g_WsFront;
CWSFrontMap g_XWSFronts;

static void ev_websocket_handler(struct mg_connection* nc, int ev, void* ev_data)
{
	switch (ev)
	{
		//case MG_EV_ACCEPT:
		//{
		//	CWSFrontMap::iterator iter = g_XWSFronts.find(nc->mgr);
		//	if (iter != g_XWSFronts.end())
		//		iter->second->onAccept(nc);
		//	break;
		//}
		//case MG_EV_CONNECT:
		//{
		//	g_WsFront->onBeforeConnect(nc);
		//	break;
		//}
	case MG_EV_HTTP_REQUEST:
	{
		struct http_message* hm = (struct http_message*)ev_data;
		CWSFrontMap::iterator iter = g_XWSFronts.find(nc->mgr);
		if (iter != g_XWSFronts.end())
			iter->second->onHttpRequest(nc, hm);
		break;
	}
	case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
	{
		struct http_message* hm = (struct http_message*)ev_data;
		CWSFrontMap::iterator iter = g_XWSFronts.find(nc->mgr);
		if (iter != g_XWSFronts.end())
			iter->second->onConnect(nc, hm);
		break;
	}
	case MG_EV_CLOSE:
	{
		CWSFrontMap::iterator iter = g_XWSFronts.find(nc->mgr);
		if (iter != g_XWSFronts.end())
			iter->second->onDisConnect(nc);
		break;
	}
	case MG_EV_WEBSOCKET_FRAME:
	{
		struct websocket_message* wm = (struct websocket_message*)ev_data;
		CWSFrontMap::iterator iter = g_XWSFronts.find(nc->mgr);
		if (iter != g_XWSFronts.end())
			iter->second->HandleMessage(nc, wm);
		break;
	}
	case WEBSOCKET_OP_PING:
	{
		CWSFrontMap::iterator iter = g_XWSFronts.find(nc->mgr);
		if (iter != g_XWSFronts.end())
		{
			int nWsRspFront = nc->session_id % iter->second->m_WsFrontRsps.size();
			CWsSession* pWsSession = iter->second->m_WsFrontRsps[nWsRspFront]->getSession(nc);
			if (pWsSession != NULL)
				pWsSession->SendSimpleMessage("P", 1);
		}
		break;
	}
	case MG_EV_WEBSOCKET_CONTROL_FRAME:
	{
		break;
	}
	default:
	{
		//REPORT_EVENT(LOG_CRITICAL, "WSFront", "websocket_handler:default:%d", ev);
	}
	}
}

CWsSession::CWsSession(COutFogServiceApi* pDataTransfer, mg_connection* nc, CFrontLogger* pCallback)
{
	m_pFlow = new COneTimeFlow();
	m_reader.AttachFlow(m_pFlow, 0);
	m_rspPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
	m_pnc = nc;
	m_pDataTransfer = pDataTransfer;
	m_pCallback = pCallback;
	m_nZipType = 0;
	memset(m_sUserID, 0, sizeof(m_sUserID));
	memset(m_sToken, 0, sizeof(m_sToken));
}

CWsSession::~CWsSession()
{
	//CWsFlowReaderMap::iterator iter = m_WsFlowReaderMap.begin();
	//while (iter != m_WsFlowReaderMap.end())
	//{
	//	CFlowReader* pFlowReader = iter->second;
	//	m_WsFlowReaderMap.erase(iter++);
	//	delete pFlowReader;
	//}
	//if (m_pnc != NULL)
	//{
	//	mg_send_websocket_frame(m_pnc, WEBSOCKET_OP_CLOSE, "", 0);
	//	m_pnc->flags |= MG_F_SEND_AND_CLOSE;
	//}
	delete m_pFlow;
}

//处理kernel产生的结果流
int CWsSession::HandleOtherTask(bool* pSleep)
{
	int nDealCount = 0;
	*pSleep = true;
	CWsFlowReaderMap::iterator iter = m_WsFlowReaderMap.begin();
	while (iter != m_WsFlowReaderMap.end())
	{
		CFlowReader* pFlowReader = iter->second;
		int i = 0;
		bool bSleepIn = false;
		while (i < g_nMaxSendPackages)
		{
			if (!pFlowReader->GetNext(&m_rspPackage))
			{
				bSleepIn = true;
				break;
			}

			if (pFlowReader->GetBusinessNo() != UF_INT8_MAX)
			{
				if (m_rspPackage.GetFTDCHeader()->BusinessNo < pFlowReader->GetBusinessNo())
					continue;
				else
					pFlowReader->SetBusinessNo(UF_INT8_MAX);
			}
			m_rspPackage.GetFTDCHeader()->SubjectNo = iter->first;
			m_rspPackage.GetFTDCHeader()->SequenceNo = pFlowReader->GetId();
			nDealCount += HandleNotify(&m_rspPackage);
			i++;
		}
		if (!bSleepIn)
			*pSleep = false;
		if (i == g_nMaxSendPackages)
		{
			if (m_pCallback != NULL)
				m_pCallback->WriteLog(m_pnc, "Read [%s] Not Over %0.f/%0.f for %d EachTime", pFlowReader->GetFlow()->GetName(), pFlowReader->GetId(), pFlowReader->GetCount(), g_nMaxSendPackages);
		}
		iter++;
	}

	bool bSleepIn = false;
	int iLen = 0;
	while (true)
	{
		char* pBuff = (char *)m_reader.GetNext(iLen);
		if (pBuff == NULL)
		{
			bSleepIn = true;
			break;
		}

		nDealCount++;
		if (m_pCallback != NULL)
			m_pCallback->WriteLogBase(m_pnc, pBuff, iLen);

		if (iLen == 1)
		{
			switch (*pBuff)
			{
			case 'P':
			{
				mg_send_websocket_frame(m_pnc, WEBSOCKET_OP_PONG, pBuff, 0);
				break;
			}
			}
		}
		else
		{
			mg_send_websocket_frame(m_pnc, WEBSOCKET_OP_TEXT, pBuff, iLen);
		}
		mg_if_can_send_cb(m_pnc);
	}

	if (!bSleepIn)
		*pSleep = false;

	return nDealCount;
}

int CWsSession::GetSessionID()
{
	return m_pnc->session_id;
}

bool CWsSession::HandleResponse(CXTPPackage* pPackage)
{
	if (m_pCallback != NULL)
		m_pCallback->WriteLogBase(m_pnc, "4");
	m_FieldMetaInput.Type = OUTFOG_TYPE_NULL;
	m_FieldMetaInput.MessageNo = pPackage->GetXTPHeader()->MessageNo;
	m_FieldMetaInput.FrontNo = m_nZipType;
	m_pDataTransfer->SyncReqField(&m_FieldMetaInput, NULL, &m_FieldMetaOutput, m_fieldStruct);
	if (m_pCallback != NULL)
		m_pCallback->WriteLogBase(m_pnc, "5");
	bool bFirst = false;

	if (pPackage->GetXTPHeader()->Chain == OUTFOG_CHAIN_FIRST || pPackage->GetXTPHeader()->Chain == OUTFOG_CHAIN_SINGLE)
	{
		if (m_RspString.capacity() > OUTFOG_MAX_PACKAGE_SIZE * 128)
		{
			if (m_pCallback != NULL)
				m_pCallback->WriteLog(m_pnc, "RspString[%s]IsTooLarge,SwapTo0", m_RspString.capacity());
			string().swap(m_RspString);
		}
		char m_SendBuff[4 * OUTFOG_MAX_PACKAGE_SIZE];
		switch (m_nZipType)
		{
		case 1:
			sprintf(m_SendBuff, "{\"a\":\"%s\",\"n\":%d,\"c\":%d,\"m\":\"%s\",\"r\":[",
				m_fieldStruct, pPackage->GetXTPHeader()->RequestNo, pPackage->GetXTPHeader()->SubjectNo, pPackage->GetXTPHeader()->Remark);
			break;
		case 0:
		default:
			sprintf(m_SendBuff, "{\"action\":\"%s\",\"requestNo\":%d,\"errorCode\":%d,\"errorMsg\":\"%s\",\"result\":[",
				m_fieldStruct, pPackage->GetXTPHeader()->RequestNo, pPackage->GetXTPHeader()->SubjectNo, pPackage->GetXTPHeader()->Remark);
		}
		bFirst = true;
		m_RspString = m_SendBuff;
	}

	CFieldTypeIterator iter = pPackage->GetFieldTypeIterator();
	while (!iter.IsEnd())
	{
		TFieldHeader iterHeader = iter.Retrieve(m_fieldStream);
		m_FieldMetaInput.FieldNo = iterHeader.FieldID;
		m_FieldMetaInput.Length = iterHeader.Size;

		m_FieldMetaInput.Type = OUTFOG_TYPE_REQUEST;
		m_FieldMetaInput.Chain = OUTFOG_DATAFORMAT_STREAM;
		m_FieldMetaOutput.Chain = OUTFOG_DATAFORMAT_JSON;
		bool ret = m_pDataTransfer->SyncReqField(&m_FieldMetaInput, m_fieldStream, &m_FieldMetaOutput, m_fieldStruct);
		if (ret)
		{
			if (m_pDataTransfer != NULL)
				m_pDataTransfer->SyncReqField(&m_FieldMetaInput, m_fieldStream, &m_FieldMetaOutput, m_fieldStruct);
			char m_SendBuff[4 * OUTFOG_MAX_PACKAGE_SIZE];
			if (bFirst)
			{
				switch (m_nZipType)
				{
				case 1:
					sprintf(m_SendBuff, "{\"t\":\"%s\",\"d\":{%s}}", m_FieldMetaOutput.SubIndexName, m_fieldStruct);
					break;
				case 0:
				default:
					sprintf(m_SendBuff, "{\"table\":\"%s\",\"data\":{%s}}", m_FieldMetaOutput.SubIndexName, m_fieldStruct);
				}
				bFirst = false;
			}
			else
			{
				switch (m_nZipType)
				{
				case 1:
					sprintf(m_SendBuff, ",{\"t\":\"%s\",\"d\":{%s}}", m_FieldMetaOutput.SubIndexName, m_fieldStruct);
					break;
				case 0:
				default:
					sprintf(m_SendBuff, ",{\"table\":\"%s\",\"data\":{%s}}", m_FieldMetaOutput.SubIndexName, m_fieldStruct);
				}
			}
			m_RspString += m_SendBuff;
		}
		iter.Next();
	}

	if (pPackage->GetXTPHeader()->Chain == OUTFOG_CHAIN_LAST || pPackage->GetXTPHeader()->Chain == OUTFOG_CHAIN_SINGLE)
	{
		m_RspString += "]}";
		if (m_pnc != NULL)
		{
			mg_send_websocket_frame(m_pnc, WEBSOCKET_OP_TEXT, m_RspString.c_str(), m_RspString.size());
			mg_if_can_send_cb(m_pnc);
			//m_pCallback->WriteLogByLenth(m_pnc, m_RecvSendString.c_str(), m_RecvSendString.size());
			if (m_pCallback != NULL)
				m_pCallback->WriteLogBase(m_pnc, m_RspString.c_str(), m_RspString.size());
		}
	}
	return true;
}

int CWsSession::HandleNotify(CFTDCPackage* pPackage)
{
	m_FieldMetaInput.Type = OUTFOG_TYPE_NULL;
	m_FieldMetaInput.MessageNo = pPackage->GetFTDCHeader()->MessageNo;
	m_FieldMetaOutput.MessageNo = m_FieldMetaInput.MessageNo;
	m_FieldMetaInput.FrontNo = m_nZipType;
	m_pDataTransfer->SyncReqField(&m_FieldMetaInput, NULL, &m_FieldMetaOutput, m_fieldStruct);
	//sprintf(m_SendBuff, "{\"action\":\"%s\",\"localNo\":%d,\"sequenceNo\":%0.f,\"changeType\":\"%c\",\"result\":[",
		//m_fieldStruct, pPackage->GetFTDCHeader()->SubjectNo, pPackage->GetFTDCHeader()->SequenceNo, cChangeType);

	char m_SendBuff[4 * OUTFOG_MAX_PACKAGE_SIZE];
	switch (m_nZipType)
	{
	case 1:
		sprintf(m_SendBuff, "{\"a\":\"%s\",\"r\":[", m_fieldStruct);
		break;
	case 0:
	default:
		char cChangeType = pPackage->GetFTDCHeader()->RequestNo;
		sprintf(m_SendBuff, "{\"action\":\"%s\",\"localNo\":%d,\"seq\":%0.f,\"changeType\":\"%c\",\"result\":[",
			m_fieldStruct, pPackage->GetFTDCHeader()->SubjectNo, pPackage->GetFTDCHeader()->SequenceNo, cChangeType);
	}

	string m_PushSendString = m_SendBuff;

	bool bFirst = true;
	int nCount = 0;
	CFieldTypeIterator iter = pPackage->GetFieldTypeIterator();
	while (!iter.IsEnd())
	{
		TFieldHeader iterHeader = iter.Retrieve(m_fieldStream);
		m_FieldMetaInput.FieldNo = iterHeader.FieldID;
		m_FieldMetaOutput.FieldNo = iterHeader.FieldID;
		m_FieldMetaInput.Length = iterHeader.Size;

		m_FieldMetaInput.Type = OUTFOG_TYPE_REQUEST;
		m_FieldMetaInput.Chain = OUTFOG_DATAFORMAT_STREAM;
		m_FieldMetaOutput.Chain = OUTFOG_DATAFORMAT_JSON;
		//m_FieldMetaInput.FrontNo = m_nZipType;

		bool ret = m_pDataTransfer->SyncReqField(&m_FieldMetaInput, m_fieldStream, &m_FieldMetaOutput, m_fieldStruct);
		if (ret)
		{
			if (bFirst)
			{
				switch (m_nZipType)
				{
				case 1:
					sprintf(m_SendBuff, "{\"t\":\"%s\",\"d\":{%s}}", m_FieldMetaOutput.SubIndexName, m_fieldStruct);
					break;
				case 0:
				default:
					sprintf(m_SendBuff, "{\"table\":\"%s\",\"data\":{%s}}", m_FieldMetaOutput.SubIndexName, m_fieldStruct);
				}
				bFirst = false;
			}
			else
			{
				switch (m_nZipType)
				{
				case 1:
					sprintf(m_SendBuff, ",{\"t\":\"%s\",\"d\":{%s}}", m_FieldMetaOutput.SubIndexName, m_fieldStruct);
					break;
				case 0:
				default:
					sprintf(m_SendBuff, ",{\"table\":\"%s\",\"data\":{%s}}", m_FieldMetaOutput.SubIndexName, m_fieldStruct);
				}
			}
			m_PushSendString += m_SendBuff;
		}
		nCount++;
		iter.Next();
	}

	m_PushSendString += "]}";
	if (m_pnc != NULL)
	{
		mg_send_websocket_frame(m_pnc, WEBSOCKET_OP_TEXT, m_PushSendString.c_str(), m_PushSendString.size());
		mg_if_can_send_cb(m_pnc);
	}
	return nCount;
}

bool CWsSession::SendSimpleMessage(char *pMessage, int nLength)
{
	//PRINT_TO_STD(pMessage);
	m_pFlow->Append((void*)pMessage, nLength);
	return true;
}

bool CWsSession::Publish(CReadFlow* pFlow, int nSequenceSeries, UF_INT8 nStartId, UF_INT8 nBusinessNo, int nReaderTimes)
{
	if (m_WsFlowReaderMap.size() > g_nMaxSubTopics)
	{
		if (m_pCallback != NULL)
			m_pCallback->WriteLog(m_pnc, "Error: SubFlowSize()[%d] > g_nMaxSubTopics[%d]", m_WsFlowReaderMap.size(), g_nMaxSubTopics);
		return false;
	}

	CWsFlowReaderMap::iterator iter = m_WsFlowReaderMap.find(nSequenceSeries);
	if (iter == m_WsFlowReaderMap.end())
	{
		CFlowReader* pFlowReader = new CFlowReader();
		pFlowReader->AttachFlow(pFlow, nStartId, nBusinessNo, nReaderTimes);
		m_WsFlowReaderMap.insert(CWsFlowReaderMap::value_type(nSequenceSeries, pFlowReader));
	}
	else
	{
		CFlowReader* pFlowReader = iter->second;
		pFlowReader->AttachFlow(pFlow, nStartId, nBusinessNo, nReaderTimes);
	}
	if (m_pCallback != NULL)
		m_pCallback->WriteLog(m_pnc, "PublishFlow[%d]From[%0.f]As[%d]ReaderTimes", nSequenceSeries, nStartId, nReaderTimes);
	return true;
}

bool CWsSession::UnPublish(int nSequenceSeries)
{
	CWsFlowReaderMap::iterator iter = m_WsFlowReaderMap.find(nSequenceSeries);
	if (iter == m_WsFlowReaderMap.end())
		return false;

	CFlowReader* pFlowReader = iter->second;
	m_WsFlowReaderMap.erase(iter);
	delete pFlowReader;
	return true;
}

void CWsSession::UnPublishAll()
{
	CWsFlowReaderMap::iterator iter = m_WsFlowReaderMap.begin();
	while (iter != m_WsFlowReaderMap.end())
	{
		CFlowReader* pFlowReader = iter->second;
		m_WsFlowReaderMap.erase(iter++);
		delete pFlowReader;
	}
}

void CWsSession::DisConnect()
{
	if (m_pnc != NULL)
	{
		mg_send_websocket_frame(m_pnc, WEBSOCKET_OP_CLOSE, "", 0);
		mg_if_can_send_cb(m_pnc);
	}
}

CWSFront::CWSFront(CReadFlow* pFlow, CFlow* pSeriesFlow, CFlowManager* pFlowManager, const char* pFrontAddress, int nFrontid, int nPortNumber, int nThreads) :COutFogServiceApi()
{
	//m_nLastPackages = 0;
	//m_nLastCountTime = 0;
	//m_nLastCounts = 0;
	m_pDataTransfer = NULL;
	//	m_ResultReader.AttachFlow(pFlow, 0);
	m_pSeriesFlow = pSeriesFlow;
	//	m_pFlowManager = pFlowManager;
	m_bManger = false;
	m_nSessionID = 1;
	//m_rspPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
	m_reqPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);

	vector<string> sSplitFirst = Txtsplit(pFrontAddress, "|");
	if (sSplitFirst.size() == 2)
	{
		if (!strcmp(sSplitFirst[0].c_str(), "manager"))
			m_bManger = true;
		sprintf(m_FrontAddress, "%d", atoi(sSplitFirst[1].c_str()) + nPortNumber - 1);
	}
	else
	{
		sprintf(m_FrontAddress, "%d", atoi(pFrontAddress) + nPortNumber - 1);
	}

	//strcpy(m_FrontAddress, pFrontAddress);

	m_nFrontNo = nFrontid;
	//	m_pWsSession = NULL;

	PRINT_TO_STD("CWsFront: Open Port:%s", m_FrontAddress);

	struct mg_connection* nc = NULL;

	mg_mgr_init(&mgr, NULL);
	nc = mg_bind(&mgr, m_FrontAddress, ev_websocket_handler);
	if (nc == NULL)
	{
		//	printf("Error starting server %s: %s\n", pFrontAddress,*bind_opts.error_string);
		exit(1);
	}
	mg_set_protocol_http_websocket(nc);
	//mg_if_can_send_cb(nc);

	//	printf("Starting WS server on %s\n", address);
	//	fflush(stdout);

	//	s_http_server_opts.document_root = ".";  // Serve current directory
	//	s_http_server_opts.enable_directory_listing = "yes";
	g_WsFront = this;

	//	m_nCount = 0;
		//m_pLogFile = NULL;
		//m_nGap = atoi(pConfig->getConfig("PersistGap"));
		//m_nGap = GetConfigInt(INI_FILE_NAME, "PersistGap");
		//if (m_nGap == 0)
		//	m_nGap = 1000000;

		//if (!strcmp(pConfig->getConfig("FrontLogs"), "yes"))
		//if (GetConfigString(INI_FILE_NAME, "FrontLogs") == "yes")
		//{
		//	m_strFlowPathName = GetConfigString(INI_FILE_NAME, "FlowPath");
		//	m_strFlowPathName += "wsfront";
		//	getLogMinTime(m_fDate);
		//	char szFullFilename[512];
		//	sprintf(szFullFilename, "%s_%d.%s.tlog", m_strFlowPathName.c_str(), m_nFrontNo, m_fDate);
		//	m_pLogFile = fopen(szFullFilename, "at");
		//}
		//m_nCPUMode = atoi(pConfig->getConfig("CPUMode"));
		//if (m_nCPUMode < 0)
		//	m_nCPUMode = 0;
	g_XWSFronts.insert(CWSFrontMap::value_type(&mgr, this));

	if (GetConfigString(INI_FILE_NAME, "FrontLogs") == "no")
	{
		m_pFrontLogger = NULL;
	}
	else
	{
		int nGap = GetConfigInt(INI_FILE_NAME, "PersistGap") * 2;
		if (nGap == 0)
			nGap = 1000000;
		m_pFrontLogger = new CFrontLogger((GetConfigString(INI_FILE_NAME, "FlowPath") + "wsfront").c_str(), nFrontid, nGap);
		m_pFrontLogger->Create();
	}

	if (nThreads <= 0)
		nThreads = 1;
	PRINT_TO_STD("CWsFront: CWsFront's nThreads:%d", nThreads);

	g_nMaxSessions = g_nMaxSessions / nThreads;
	for (int i = 0; i < nThreads; i++)
	{
		m_WsFrontRsps.push_back(new CWsFrontRsp(pFlow, m_pFrontLogger, pFlowManager));
		m_WsFrontRsps[i]->Create();
	}
}

CWSFront::~CWSFront()
{
	mg_mgr_free(&mgr);
	//CWsSessionMap::iterator iter = m_WsSessionMap.begin();
	//while (iter != m_WsSessionMap.end())
	//{
	//	CWsSession* pWsSession = iter->second;
	//	m_WsSessionMap.erase(iter++);
	//	delete pWsSession;
	//}
}

void CWSFront::Run()
{
	while (true)
	{
		//mg_mgr_poll(&mgr, m_nCPUMode);
		mg_mgr_poll(&mgr, 1);
	}
}

bool CWsFrontRsp::HandleResponse(CXTPPackage* pMessage)
{
	CWsSessionMap::iterator iter = m_WsSessionMap.find(pMessage->GetXTPHeader()->SessionNo);
	if (iter == m_WsSessionMap.end())
		return false;

	CWsSession* pWsSession = iter->second;
	switch (pMessage->GetXTPHeader()->Type)
	{
	case OUTFOG_TYPE_DELSESSION:
	{
		if (m_pFrontLogger != NULL)
			m_pFrontLogger->WriteLogBase(pWsSession->m_pnc, "Session To Be Closed From Kernel");
		//m_pWsSessionMap->erase(iter);
		pWsSession->DisConnect();
		//delete pWsSession;
		break;
	}
	case OUTFOG_TYPE_SUBTOPIC:
	{
		CReadFlow* pFlow = NULL;

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
			break;

		//UF_INT8 nPubStartId = atof(pMessage->GetXTPHeader()->Remark);
		UF_INT8 nPubStartId = pMessage->GetXTPHeader()->SequenceNo;

		if (nPubStartId <= 0)
			nPubStartId = pFlow->GetCount() + nPubStartId;
		if (nPubStartId > pFlow->GetCount())
			nPubStartId = pFlow->GetCount();
		if (nPubStartId < pFlow->GetFirstID())
		{
			if (m_pFrontLogger != NULL)
				m_pFrontLogger->WriteLog(pWsSession->m_pnc, "Error:PublishFlow[%d]PubStartID[%0.f]<FirstID[%0.f]", pMessage->GetXTPHeader()->SubjectNo, nPubStartId, pFlow->GetFirstID());
			SendError(pWsSession->m_pnc, NULL, WS_ERROR_SubTopic_ID, "Error:PublishFlow[%d]PubStartID[%0.f]<FirstID[%0.f]", pMessage->GetXTPHeader()->SubjectNo, nPubStartId, pFlow->GetFirstID());
			nPubStartId = pFlow->GetFirstID();
		}

		int nLocalNo = pMessage->GetXTPHeader()->MessageNo;
		if (!pWsSession->Publish(pFlow, nLocalNo, nPubStartId, pMessage->GetXTPHeader()->BusinessNo, pMessage->GetXTPHeader()->RequestNo))
			SendError(pWsSession->m_pnc, NULL, WS_ERROR_SubTopic_ID, "MaxSubTopicsLimit");
		break;
	}
	case OUTFOG_TYPE_UNSUBTOPIC:
	{
		if (!strcmp(pMessage->GetXTPHeader()->Remark, OUTFOG_REMARK_UNSUBTOPIC_ALL))
		{
			pWsSession->UnPublishAll();
		}
		else
		{
			int nLocalID = pMessage->GetXTPHeader()->MessageNo;
			pWsSession->UnPublish(nLocalID);
		}
		break;
	}
	case OUTFOG_TYPE_RESPONSE:
	{
		pWsSession->HandleResponse(pMessage);
		break;
	}
	default:
	{
		REPORT_EVENT(LOG_CRITICAL, "WsFront", "HandleMessage Error: NO XTPHeader Type");
		break;
	}
	}
	return true;
}

bool CWSFront::ReqField(const TOutFogMetaRef* pOutFogReqRef, const void* pStream)
{
	if (pOutFogReqRef->Type == OUTFOG_TYPE_START)
	{
		PRINT_TO_STD("WsFrontStartWork: Open Port!");
		Create();
	}
	return true;
}

const char* CWSFront::getVersion()
{
	return "WsFront";
}

bool CWSFront::onHttpRequest(mg_connection* nc, http_message* hm)
{
	if (m_pFrontLogger != NULL)
		m_pFrontLogger->WriteLogBase(nc, hm->message.p, hm->message.len);

	const char* szContextType = "application/json; charset=gbk";
	mg_printf(nc, "HTTP/1.1 %d %s\r\nContent-Type: %.*s\r\nTransfer-Encoding: chunked\r\n\r\n", 200, "OK", strlen(szContextType), szContextType);

	if (!strncmp(hm->uri.p, "/ping", 5))
	{
		mg_send_http_chunk(nc, "pong", 4);
		mg_send_http_chunk(nc, "", 0);
		mg_if_can_send_cb(nc);
	}
	else
	{
		nc->last_usec = -1;
		mg_send_http_chunk(nc, "ErrorHttpMessage", 16);
		mg_send_http_chunk(nc, "", 0);
		mg_if_can_send_cb(nc);
	}
	return true;
}

//bool CWSFront::onAccept(mg_connection* nc)
//{
//	char CloseMessage[128] = "";
//	//if (m_nLastPackages > g_nMaxPackages)
//	//{
//	//	sprintf(CloseMessage, "MaxPackagesLimit:%0.f>%d", m_nLastPackages, g_nMaxPackages);
//	//	nc->flags |= MG_F_CLOSE_IMMEDIATELY;
//	//}
//
//	if (m_pFrontLogger != NULL && CloseMessage[0] != '\0')
//	{
//		m_pFrontLogger->WriteLog(nc, "Not Accepted: %s", CloseMessage);
//	}
//	return true;
//}

bool CWSFront::onConnect(mg_connection* nc, http_message* hm)
{
	nc->session_id = m_nSessionID;
	m_nSessionID++;
	if (m_nSessionID > INT_32_MAX / 2)
		m_nSessionID = 1;

	CWsSession* pWsSession = new CWsSession(m_pDataTransfer, nc, m_pFrontLogger);
	CWsFrontRsp * pWsFrontRsp = NULL;

	int nWsRspFront = nc->session_id % m_WsFrontRsps.size();

	if (!m_WsFrontRsps[nWsRspFront]->insertSession(pWsSession))
	{
		//nc->flags |= MG_F_CLOSE_IMMEDIATELY;
		delete pWsSession;
		return false;
	}

	//获取UserID和Token
	map<string, string> formMap = mapSplit((char*)hm->query_string.p, (int)(hm->query_string.len), '&', '=');

	strncpy(pWsSession->m_sUserID, formMap["UserID"].c_str(), sizeof(pWsSession->m_sUserID) - 1);
	if (pWsSession->m_sUserID[0] == '\0')
		strncpy(pWsSession->m_sUserID, formMap["MemberID"].c_str(), sizeof(pWsSession->m_sUserID) - 1);
	strncpy(pWsSession->m_sToken, formMap["Token"].c_str(), sizeof(pWsSession->m_sToken) - 1);

	if (pWsSession->m_sToken[0] != '\0')
	{
		//这里其实要判断Token是否符合要求，不符合要求就干掉，省的到上面去判断
	}
	else
	{
		if (m_bManger)
			pWsSession->m_sToken[0] = OUTFOG_REMARK_TOKEN_MANAGER;
	}

	m_reqPackage.PrepareRequest(OUTFOG_TNO_NULL, OUTFOG_CHAIN_LAST);
	m_reqPackage.GetXTPHeader()->SessionNo = nc->session_id;
	m_reqPackage.GetXTPHeader()->RequestNo = 0;
	m_reqPackage.GetXTPHeader()->FrontNo = m_nFrontNo;
	m_reqPackage.GetXTPHeader()->Type = OUTFOG_TYPE_ADDSESSION;
	m_reqPackage.GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
	mg_sock_addr_to_str(&(nc->sa), m_reqPackage.GetXTPHeader()->Remark, sizeof(m_reqPackage.GetXTPHeader()->Remark), MG_SOCK_STRINGIFY_IP);
	m_reqPackage.MakePackage();
	m_pSeriesFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());

	nc->last_usec = -1;
	if (m_pFrontLogger != NULL)
		m_pFrontLogger->WriteLogBase(nc, "Connected");

	return true;
}

bool CWSFront::onDisConnect(mg_connection* nc)
{
	//	delete m_pWsSession;
	//	m_pWsSession=NULL;

	int nWsRspFront = nc->session_id % m_WsFrontRsps.size();

	m_WsFrontRsps[nWsRspFront]->deleteSession(nc);

	m_reqPackage.PreparePublish(OUTFOG_TNO_NULL, OUTFOG_CHAIN_LAST);
	m_reqPackage.GetXTPHeader()->Type = OUTFOG_TYPE_DELSESSION;
	m_reqPackage.GetXTPHeader()->SessionNo = nc->session_id;
	m_reqPackage.GetXTPHeader()->RequestNo = 0;
	m_reqPackage.GetXTPHeader()->FrontNo = m_nFrontNo;
	m_reqPackage.GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
	mg_sock_addr_to_str(&(nc->sa), m_reqPackage.GetXTPHeader()->Remark, sizeof(m_reqPackage.GetXTPHeader()->Remark), MG_SOCK_STRINGIFY_IP);
	m_reqPackage.MakePackage();
	m_pSeriesFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());

	//REPORT_EVENT(LOG_CRITICAL, "WsFront", "Session [%s] [0x%08x] Disconnected", m_reqPackage.GetXTPHeader()->Remark, pWsSession->GetSessionID());

	if (m_pFrontLogger != NULL)
		m_pFrontLogger->WriteLogBase(nc, "Closed");

	return true;
}

bool CWSFront::HandleMessage(mg_connection* nc, websocket_message* wm)
{
	//WriteLogByLenth(nc, (const char*)wm->data, wm->size);
	//m_pWSLogger->WriteLogBase(nc, (const char*)(wm->data), wm->size);

	int nWsRspFront = nc->session_id % m_WsFrontRsps.size();

	CWsSession* pWsSession = m_WsFrontRsps[nWsRspFront]->getSession(nc);
	if (pWsSession == NULL)
		return false;

	if (wm->size < 8)
	{
		if (!strncmp((const char*)wm->data, "ping", wm->size))
		{
			//mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, "pong", 4);
			//mg_if_can_send_cb(nc);
			pWsSession->SendSimpleMessage("pong",4);
			return true;
		}
		else
		{
			if (!strncmp((const char*)wm->data, "zip=", 4))
			{
				pWsSession->m_nZipType = ((char*)wm->data)[4] - '0';
				string sZip = "zip_ready_for_";
				sZip += ((char*)wm->data)[4];
				pWsSession->SendSimpleMessage((char*)(sZip.c_str()), sZip.length());
				//mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, sZip.c_str(), sZip.length());
				//mg_if_can_send_cb(nc);
				return true;
			}
		}
		return SendSimpleError(pWsSession, wm, HTTP_ERROR_Message_ID, "input_is_too_short");
	}
	nc->last_usec = -1;
	if (m_pFrontLogger != NULL)
		m_pFrontLogger->WriteLogBase(nc, (const char*)(wm->data), wm->size);

	//传入的是一个json格式: {"requestNo":1,"A":{"a":1,"b":2,...}}，标记序号，用于区别
	//或者 {"A":{"a":1,"b":2,...}}，不标记序号，用于区别
	//先提取A 和 {"a":1,"b":2,...}
	//A作为操作类型，{"a":1,"b":2,...}作为操作数据
	int i = 1;
	int j = 0;
	int nRequestNo = 0;
	while (wm->data[i] != ':')
	{
		if (wm->data[i] == '\0')
			return false;
		if (wm->data[i] != '\"' || wm->data[i] != '\"')
		{
			m_FieldMetaInput.SubIndexName[j] = wm->data[i];
			j++;
		}
		i++;
	}
	m_FieldMetaInput.SubIndexName[j] = '\0';

	if (!strcmp(m_FieldMetaInput.SubIndexName, "requestNo") || !strcmp(m_FieldMetaInput.SubIndexName, "RequestNo"))
	{
		i++;
		j = 0;
		char sRequestNo[256] = "";
		while (wm->data[i] != ',')
		{
			if (wm->data[i] == '\0')
				return false;
			sRequestNo[j] = wm->data[i];
			j++;
			i++;
		}
		nRequestNo = atoi(sRequestNo);
		i++;
		j = 0;
		while (wm->data[i] != ':')
		{
			if (wm->data[i] == '\0')
				return false;
			if (wm->data[i] != '\"' || wm->data[i] != '\"')
			{
				m_FieldMetaInput.SubIndexName[j] = wm->data[i];
				j++;
			}
			i++;
		}
		m_FieldMetaInput.SubIndexName[j] = '\0';
	}

	void* pData = &(wm->data[i + 1]);
	//去掉最后一个}
	wm->data[wm->size - 1] = '\0';

	m_FieldMetaInput.Length = wm->size - i - 2;
	m_FieldMetaInput.Chain = OUTFOG_DATAFORMAT_JSON;
	m_FieldMetaInput.Type = OUTFOG_TYPE_REQUEST;
	m_FieldMetaOutput.Chain = OUTFOG_DATAFORMAT_STREAM;

	if (m_pFrontLogger != NULL)
		m_pFrontLogger->WriteLogBase(nc, "1");
	bool ret = m_pDataTransfer->SyncReqField(&m_FieldMetaInput, pData, &m_FieldMetaOutput, m_fieldStruct);
	if (m_pFrontLogger != NULL)
		m_pFrontLogger->WriteLogBase(nc, "2");

	if (ret)
	{
		m_reqPackage.PreparePublish(m_FieldMetaOutput.MessageNo, OUTFOG_CHAIN_LAST);
		m_reqPackage.GetXTPHeader()->SessionNo = nc->session_id;
		m_reqPackage.GetXTPHeader()->RequestNo = nRequestNo;
		strcpy(m_reqPackage.GetXTPHeader()->UserID, pWsSession->m_sUserID);
		strcpy(m_reqPackage.GetXTPHeader()->Remark, pWsSession->m_sToken);
		//if (nRequestNo >= 0)
		//	nc->last_usec = m_nSendUsec;
		//else
		//	nc->last_usec = -1;
		m_reqPackage.AddStream(m_FieldMetaOutput.FieldNo, m_fieldStruct, m_FieldMetaOutput.Length);
		m_reqPackage.GetXTPHeader()->FrontNo = m_nFrontNo;
		m_reqPackage.GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
		m_reqPackage.MakePackage();
		m_pSeriesFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());
		if (m_pFrontLogger != NULL)
			m_pFrontLogger->WriteLogBase(nc, "3");
	}
	else
	{
		//mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, m_fieldStruct, strlen(m_fieldStruct));
		SendSimpleError(pWsSession, wm, HTTP_ERROR_Format_ID, m_fieldStruct);
	}
	return true;
}

bool CWSFront::SendSimpleError(CWsSession* pWsSession, websocket_message* wm, int nErrorID, const char* format, ...)
{
	char m_sError[5120];
	sprintf(m_sError, "{\"errorCode\":%d,\"errorMsg\":\"", nErrorID);

	va_list v;
	va_start(v, format);
	vsnprintf(m_sError + strlen(m_sError), sizeof(m_sError) - strlen(m_sError), format, v);
	va_end(v);

	char* pNext = fastStrcatLimit(m_sError, "\",\"result\":[");
	if (wm != NULL)
		pNext = fastStrcatLimit(pNext, (const char*)wm->data, MIN(wm->size, 4500));
	pNext = fastStrcatLimit(pNext, "]}");
	pWsSession->SendSimpleMessage(m_sError, pNext - m_sError);

	//string sError = m_sError;
	//sError += "\",\"result\":[";
	////为了兼顾效率，如果报文比较大，就不回复了，这样节省带宽
	//if (wm != NULL && nErrorID != HTTP_ERROR_Success_ID && wm->size < 4096)
	//	sError.append((const char*)wm->data, wm->size);
	//sError += "]}";
	//mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, sError.c_str(), sError.length());
	//mg_if_can_send_cb(nc);
	//pWsSession->SendSimpleMessage((char*)(sError.c_str()), sError.length());
	return true;
}

bool CWsFrontRsp::SendError(mg_connection* nc, websocket_message* wm, int nErrorID, const char* format, ...)
{
	char m_sError[5120];
	sprintf(m_sError, "{\"errorCode\":%d,\"errorMsg\":\"", nErrorID);

	va_list v;
	va_start(v, format);
	vsnprintf(m_sError + strlen(m_sError), sizeof(m_sError) - strlen(m_sError), format, v);
	va_end(v);

	//string sError = m_sError;
	//sError += "\",\"result\":[";
	////为了兼顾效率，如果报文比较大，就不回复了，这样节省带宽
	//if (wm != NULL && nErrorID != HTTP_ERROR_Success_ID && wm->size < 2048)
	//	sError.append((const char*)wm->data, wm->size);
	//sError += "]}";

	char* pNext = fastStrcatLimit(m_sError, "\",\"result\":[");;
	if (wm != NULL)
		pNext = fastStrcatLimit(pNext, (const char*)wm->data, MIN(wm->size, 4500));
	pNext = fastStrcatLimit(pNext, "]}");

	mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, m_sError, pNext - m_sError);
	mg_if_can_send_cb(nc);

	return true;
}