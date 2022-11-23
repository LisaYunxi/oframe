#include "XHttpFront.h"
#include "printinfo.h"
#include "ApiKeyUtil.h"
#include "LinkManager.h"

CHttpFrontMap g_XHttpFronts;

//CRWLock g_HttpFrontNCMapRWLock;

static void ev_handler_x(struct mg_connection* nc, int ev, void* ev_data)
{
	struct http_message* hm = (struct http_message*)ev_data;

	switch (ev)
	{
	case MG_EV_HTTP_REQUEST:
	{
		//nc->m_bServerClose = false;
		//REPORT_EVENT(LOG_INFO, "http request", "uri:%.*s, body:%.*s", hm->uri.len, hm->uri.p, hm->body.len, hm->body.p);
		CHttpFrontMap::iterator iter = g_XHttpFronts.find(nc->mgr);
		if (iter != g_XHttpFronts.end())
			iter->second->HandleMessage(nc, hm);
		break;
	}
	case MG_EV_CLOSE:
	{
		CHttpFrontMap::iterator iter = g_XHttpFronts.find(nc->mgr);
		if (iter != g_XHttpFronts.end())
			iter->second->HandleClose(nc, hm);
		break;
	}
	default:
		break;
	}
}

CXHttpFront::CXHttpFront(CReadFlow* pFlow, CFlow* pSeriesFlow, CFlowManager* pFlowManager, const char* pFrontAddress, int nFrontid, int nPortNumber) :COutFogServiceApi()
{
	memset(&m_FieldMetaInput, 0, sizeof(TOutFogMetaRef));
	memset(&m_FieldMetaOutput, 0, sizeof(TOutFogMetaRef));
	m_pDataTransfer = NULL;
	//	m_ResultReader.AttachFlow(pFlow, 0);
	m_pSeriesFlow = pSeriesFlow;
	//m_rspPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
	m_reqPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
	m_nFrontNo = nFrontid;
	m_nSessionNo = 1;
	m_nSendSecond = 0;
	m_bManger = false;
	memset(m_sToken, 0, sizeof(m_sToken));
	memset(m_sUserID, 0, sizeof(m_sUserID));

	vector<string> sSplitFirst = Txtsplit(pFrontAddress, "|");
	if (!strcmp(sSplitFirst[0].c_str(), "manager"))
		m_bManger = true;
	vector<string> sSplitSecond = Txtsplit(sSplitFirst[1], "/");
	sprintf(m_sHttpMode, "/%s/v1.0/", sSplitSecond[3].c_str());

	vector<string> sSplitThird = Txtsplit(sSplitSecond[2], ":");
	int nPort = atoi(sSplitThird[1].c_str());
	sprintf(m_FrontAddress, "%d", nPort + nPortNumber - 1);

	struct mg_connection* nc;
	struct mg_bind_opts bind_opts;

	mg_mgr_init(&mgr, NULL);

	/* Set HTTP server options */
	memset(&bind_opts, 0, sizeof(bind_opts));
	//	bind_opts.error_string = &(&(m_sError));

	nc = mg_bind_opt(&mgr, m_FrontAddress, ev_handler_x, bind_opts);
	if (nc == NULL)
	{
		if (bind_opts.error_string != NULL)
			PRINT_TO_STD("Error starting server %s:%s", m_FrontAddress, *bind_opts.error_string);
		else
			PRINT_TO_STD("Error starting server %s", m_FrontAddress);
		exit(1);
	}

	mg_set_protocol_http_websocket(nc);

	//printf("Starting RESTful server on %s\n", m_FrontAddress);

	PRINT_TO_STD("CHttpFront:Open Port:%s", m_FrontAddress);

	g_XHttpFronts.insert(CHttpFrontMap::value_type(&mgr, this));

	//m_nCount = 0;
	////m_nGap = atoi(pConfig->getConfig("PersistGap")) * 2;
	//m_nGap = GetConfigInt(INI_FILE_NAME, "PersistGap") * 2;
	//if (m_nGap == 0)
	//	m_nGap = 1000000;
	//m_pLogFile = NULL;
	////if (!strcmp(pConfig->getConfig("FrontLogs"), "yes"))
	//if (GetConfigString(INI_FILE_NAME, "FrontLogs") == "yes")
	//{
	//	m_strFlowPathName = GetConfigString(INI_FILE_NAME, "FlowPath");
	//	m_strFlowPathName += "httpfront";
	//	getLogMinTime(m_fDate);
	//	char szFullFilename[512];
	//	sprintf(szFullFilename, "%s_%d.%s.tlog", m_strFlowPathName.c_str(), nFrontid, m_fDate);
	//	m_pLogFile = fopen(szFullFilename, "at");
	//}

	//m_nCPUMode = atoi(pConfig->getConfig("CPUMode"));
	//if (m_nCPUMode < 0)
	//	m_nCPUMode = 0;

	//m_nNormalBatchs = atoi(pConfig->getConfig("NormalBatchs"));
	m_nNormalBatchs = GetConfigInt(INI_FILE_NAME, "NormalBatchs");
	if (m_nNormalBatchs <= 0)
		m_nNormalBatchs = 10;
	PRINT_TO_STD("CHttpFront:NormalBatchs:%d", m_nNormalBatchs);

	if (GetConfigString(INI_FILE_NAME, "FrontLogs") == "no")
	{
		m_pFrontLogger = NULL;
	}
	else
	{
		int nGap = GetConfigInt(INI_FILE_NAME, "PersistGap") * 2;
		if (nGap == 0)
			nGap = 1000000;
		m_pFrontLogger = new CFrontLogger((GetConfigString(INI_FILE_NAME, "FlowPath") + "httpfront").c_str(), nFrontid, nGap);
		m_pFrontLogger->Create();
	}

	m_pHttpFrontRsp = new CHttpFrontRsp(pFlow, m_pFrontLogger);
	m_pHttpFrontRsp->Create();
}

CXHttpFront::~CXHttpFront()
{
	mg_mgr_free(&mgr);
}

const char* CXHttpFront::getVersion()
{
	return "httpfront";
}

bool CXHttpFront::ReqField(const TOutFogMetaRef* pOutFogReqRef, const void* pStream)
{
	if (pOutFogReqRef->Type == OUTFOG_TYPE_START)
	{
		Create();
		PRINT_TO_STD("HttpFrontStartWork: Open Port!");
	}
	return true;
}

bool CHttpFrontRsp::HandleResponse(CXTPPackage* pPackage, int TNo, int nSessionID)
{
	//CHttpNCMap::iterator nciter = m_pNCMap->find(nSessionID);
	//int nFindTimes = 0;
	//while (nciter == m_pNCMap->end())
	//{
	//	nFindTimes++;
	//	if (m_pFrontLogger != NULL)
	//		m_pFrontLogger->WriteLog(NULL, "Cannot Find [%d]'s nc in NCMap", nSessionID);
	//	if (nFindTimes > 10)
	//		return false;
	//	nciter = m_pNCMap->find(nSessionID);
	//}
	//mg_connection* pnc = nciter->second;

	if (nSessionID < 0)
		return false;

	mg_connection* pnc = (mg_connection*)(m_NCVector[nSessionID]);
	if (pnc == NULL)
		return false;
	
	if (!pnc->m_bMultiple)
	{
		if (pnc->m_bServerClose)
		{
			if (m_pFrontLogger != NULL)
				m_pFrontLogger->WriteLog(pnc, "NotMultipleAndServerCloseIsOver");
			return false;
		}
	}

	//m_MuiltMutex.Lock();
	m_FieldMetaInput.Type = OUTFOG_TYPE_NULL;
	m_FieldMetaInput.MessageNo = TNo;
	m_FieldMetaOutput.MessageNo = TNo;
	m_FieldMetaInput.FrontNo = pnc->m_nZipType;
	if (m_pFrontLogger != NULL)
		m_pFrontLogger->WriteLogBase(pnc, "4");
	m_pDataTransfer->SyncReqField(&m_FieldMetaInput, NULL, &m_FieldMetaOutput, m_fieldStruct);

	bool bFirst = false;
	if (pPackage->GetXTPHeader()->Chain == OUTFOG_CHAIN_FIRST || pPackage->GetXTPHeader()->Chain == OUTFOG_CHAIN_SINGLE)
	{
		SendResultHeader(pnc, 200);
		//	sprintf(m_SendBuff, "{\"action\":\"%s\",\"error_id\":%d,\"error_msg\":\"%s\",\"result\":[",m_fieldStruct, pPackage->GetXTPHeader()->SubjectNo, pPackage->GetXTPHeader()->SubIndexName);
		if (pnc->m_nZipType == 0)
			sprintf(m_SendBuff, "{\"action\":\"%s\",\"requestNo\":%d,\"errorCode\":%d,\"errorMsg\":\"%s\",\"result\":[",
				m_fieldStruct, pPackage->GetXTPHeader()->RequestNo, pPackage->GetXTPHeader()->SubjectNo, pPackage->GetXTPHeader()->Remark);
		else
			sprintf(m_SendBuff, "{\"a\":\"%s\",\"r\":%d,\"c\":%d,\"m\":\"%s\",\"r\":[",
				m_fieldStruct, pPackage->GetXTPHeader()->RequestNo, pPackage->GetXTPHeader()->SubjectNo, pPackage->GetXTPHeader()->Remark);
		if (m_SendString.capacity() > OUTFOG_MAX_PACKAGE_SIZE * 128)
		{
			if (m_pFrontLogger != NULL)
				m_pFrontLogger->WriteLog(pnc, "RspString[%d]IsTooLarge,SwapTo0", m_SendString.capacity());
			string().swap(m_SendString);
		}
		m_SendString = m_SendBuff;
		bFirst = true;
	}

	m_FieldMetaInput.Type = OUTFOG_TYPE_REQUEST;
	m_FieldMetaInput.Chain = OUTFOG_DATAFORMAT_STREAM;
	m_FieldMetaOutput.Chain = OUTFOG_DATAFORMAT_JSON;
	CFieldTypeIterator iter = pPackage->GetFieldTypeIterator();
	while (!iter.IsEnd())
	{
		TFieldHeader iterHeader = iter.Retrieve(m_fieldStream);
		m_FieldMetaInput.FieldNo = iterHeader.FieldID;
		m_FieldMetaOutput.FieldNo = iterHeader.FieldID;
		m_FieldMetaInput.Length = iterHeader.Size;
		bool ret = m_pDataTransfer->SyncReqField(&m_FieldMetaInput, m_fieldStream, &m_FieldMetaOutput, m_fieldStruct);
		if (m_pFrontLogger != NULL)
			m_pFrontLogger->WriteLogBase(pnc, "5");
		if (ret)
		{
			//if (m_pOtherTransfer != NULL)
			//	m_pOtherTransfer->SyncReqField(&m_FieldMetaInput, m_fieldStream, &m_FieldMetaOutput, m_fieldStruct);
			if (bFirst)
			{
				if (pnc->m_nZipType == 0)
					sprintf(m_SendBuff, "{\"table\":\"%s\",\"data\":{%s}}", m_FieldMetaOutput.SubIndexName, m_fieldStruct);
				else
					sprintf(m_SendBuff, "{\"t\":\"%s\",\"d\":{%s}}", m_FieldMetaOutput.SubIndexName, m_fieldStruct);
				bFirst = false;
			}
			else
			{
				if (pnc->m_nZipType == 0)
					sprintf(m_SendBuff, ",{\"table\":\"%s\",\"data\":{%s}}", m_FieldMetaOutput.SubIndexName, m_fieldStruct);
				else
					sprintf(m_SendBuff, ",{\"t\":\"%s\",\"d\":{%s}}", m_FieldMetaOutput.SubIndexName, m_fieldStruct);
			}
			m_SendString += m_SendBuff;
		}
		iter.Next();
	}

	if (pPackage->GetXTPHeader()->Chain == OUTFOG_CHAIN_LAST || pPackage->GetXTPHeader()->Chain == OUTFOG_CHAIN_SINGLE)
	{
		m_SendString += "]}";

		if (m_pFrontLogger != NULL)
			m_pFrontLogger->WriteLogBase(pnc, m_SendString.c_str(), m_SendString.size());

		//pnc->m_bServerClose = true;
		mg_send_http_chunk(pnc, m_SendString.c_str(), m_SendString.size());
		mg_send_http_chunk(pnc, "", 0); /* Send empty chunk, the end of response */

		while (pnc->send_mbuf.len > 0)
			mg_if_can_send_cb(pnc);

		//m_pNCMap->erase(nciter);
	//	if (pnc->m_bServerClose)
	//		m_MuiltMutex.UnLock();

		pnc->m_bServerClose = true;
	}
	else
	{
	//	if (pnc->m_bServerClose)
			//m_MuiltMutex.UnLock();
	}	

	return true;
}

bool CHttpFrontRsp::SendResultHeader(struct mg_connection* nc, int status_code)
{
	const char* szContextType = "application/json; charset=gbk";
	const char* reason = "OK";
	if (status_code != 200)
		reason = "Handle Error";
	mg_printf(nc, "HTTP/1.1 %d %s\r\nContent-Type: %.*s\r\nTransfer-Encoding: chunked\r\n\r\n", status_code, reason, strlen(szContextType), szContextType);
	return true;
}

bool CHttpFrontRsp::SendResult(struct mg_connection* nc, int status_code, const char* result, size_t nLenth)
{
	if (nc == NULL)
		return false;
	m_Mutex.Lock();
	SendResultHeader(nc, status_code);
	if (nLenth > 0)
	{
		mg_send_http_chunk(nc, result, nLenth);
	}
	else
	{
		size_t nResultLenth = strlen(result);
		if (nResultLenth > 0)
		{
			mg_send_http_chunk(nc, result, nResultLenth);
		}
		else
		{
			const char* ret_null = "null";
			mg_send_http_chunk(nc, ret_null, strlen(ret_null));
		}
	}
	//mg_if_can_send_cb(nc);
	// 注意 只能有一个empty chunk  否则前端会出错：Could not get any response
	mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
	//	REPORT_EVENT(LOG_INFO, "http response", "status:%d, msg:%s", status_code, result);
	mg_if_can_send_cb(nc);
	m_Mutex.UnLock();
	return true;
}

//bool CXHttpFront::SendError(struct mg_connection* nc, http_message* hm, int nErrorID, const char* sErrorMsg, const char* sAction)
bool CHttpFrontRsp::SendError(struct mg_connection* nc, http_message* hm, int nErrorID, const char* sAction, const char* format, ...)
{
	char m_sError[256];
	if (nc->m_nZipType == 0)
		sprintf(m_sError, "{\"action\":\"%s\",\"errorCode\":%d,\"errorMsg\":\"", sAction, nErrorID);
	else
		sprintf(m_sError, "{\"a\":\"%s\",\"c\":%d,\"m\":\"", sAction, nErrorID);
	va_list v;
	va_start(v, format);
	vsnprintf(m_sError + strlen(m_sError), sizeof(m_sError) - strlen(m_sError), format, v);
	va_end(v);

	string sError = m_sError;
	//if (nc->m_nZipType == 0)
	//	sError += "\",\"result\":[";
	//else
	//	sError += "\",\"r\":[";
	////为了兼顾效率，如果报文比较大，就不回复了，这样节省带宽
	//if (nErrorID != HTTP_ERROR_Success_ID && hm->body.len < 2048)
	//	sError.append(hm->body.p, hm->body.len);
	//sError += "]}";
	sError += "\"}";

	if (m_pFrontLogger != NULL)
		m_pFrontLogger->WriteLogBase(nc, sError.c_str());

	//string sErrorMessage(hm->message.p, 0, hm->message.len);
	//REPORT_EVENT(LOG_ERROR, "Error: HttpFront", sErrorMessage.c_str());
	//REPORT_EVENT(LOG_ERROR, "Error: HttpFront", m_sError.c_str());
	SendResult(nc, 200, sError.c_str(), sError.length());

	if (nErrorID == HTTP_ERROR_Success_ID)
		return true;
	else
		return false;
}

void CXHttpFront::HandleClose(mg_connection* nc, http_message* hm)
{
	//	if (m_pLogFile != NULL)
	//	{
	//#ifdef WIN32
	//		struct _timeb timeout;
	//		_ftime(&timeout);
	//		struct tm tt;
	//		LOCALTIME(&tt, &(timeout.time));
	//		fprintf(m_pLogFile, "\n[%02d:%02d:%02d %03d][S:%010d][A:%s] Closed", tt.tm_hour, tt.tm_min, tt.tm_sec, timeout.millitm, nc->session_id, inet_ntoa(nc->sa.sin.sin_addr));
	//#else
	//		struct timeval timeout;
	//		gettimeofday(&timeout, 0);
	//		struct tm tt;
	//		LOCALTIME(&tt, &(timeout.tv_sec));
	//		fprintf(m_pLogFile, "\n[%02d:%02d:%02d %06d][S:%010d][A:%s] Closed", tt.tm_hour, tt.tm_min, tt.tm_sec, timeout.tv_usec, nc->session_id, inet_ntoa(nc->sa.sin.sin_addr));
	//#endif
	//		fwrite("\n---------------------------------------------------------------", 60, 1, m_pLogFile);
	//		fflush(m_pLogFile);
	//		//m_nCount++;
	//	}

	if (m_pFrontLogger != NULL)
		m_pFrontLogger->WriteLogBase(nc, "Closed");

	//CHttpNCMap::iterator nciter = m_NCMap.find(nc->session_id);
	//if (nciter == m_NCMap.end())
	//	return;
	//if (nc->m_bServerClose)
	//{
	//	m_NCMap.erase(nciter);
	//}		
	//else
	//{
	//	g_HttpFrontNCMapRWLock.WriteLock();
	//	m_NCMap.erase(nciter);
	//	g_HttpFrontNCMapRWLock.UnLock();
	//}

	m_pHttpFrontRsp->deleteNC(nc);
}

//bool CXHttpFront::WriteLog(struct mg_connection* nc, const char* pChar, size_t nLength)
//{
//#ifdef WIN32
//	struct _timeb timeout;
//	_ftime(&timeout);
//	m_nSendSecond = timeout.time;
//#else
//	struct timeval timeout;
//	gettimeofday(&timeout, 0);
//	m_nSendSecond = timeout.tv_sec;
//#endif
//
//	struct tm tt;
//	LOCALTIME(&tt, &m_nSendSecond);
//
//	if (m_pLogFile != NULL)
//	{
//		if (m_nCount > m_nGap)
//		{
//			fclose(m_pLogFile);
//			char szFullFilename[512];
//			getLogMinTime(m_fDate);
//			sprintf(szFullFilename, "%s_%d.%s.tlog", m_strFlowPathName.c_str(), m_nFrontNo, m_fDate);
//			m_pLogFile = fopen(szFullFilename, "at");
//			m_nCount = 0;
//		}
//
//#ifdef WIN32
//		if (nc->last_usec < 0)
//		{
//			nc->last_usec = tt.tm_sec * 1000 + timeout.millitm;
//			fprintf(m_pLogFile, "\n[%02d:%02d:%02d %03d][S:%010d][A:%s] ", tt.tm_hour, tt.tm_min, tt.tm_sec, timeout.millitm, nc->session_id, inet_ntoa(nc->sa.sin.sin_addr));
//		}
//		else
//		{
//			int nSendUsec = tt.tm_sec * 1000 + timeout.millitm;
//			fprintf(m_pLogFile, "\n[%02d:%02d:%02d %03d][S:%010d][A:%s][%05d ms] ", tt.tm_hour, tt.tm_min, tt.tm_sec, timeout.millitm, nc->session_id, inet_ntoa(nc->sa.sin.sin_addr), nSendUsec - nc->last_usec);
//		}
//#else
//		if (nc->last_usec < 0)
//		{
//			nc->last_usec = tt.tm_sec * 1000000 + timeout.tv_usec;
//			fprintf(m_pLogFile, "\n[%02d:%02d:%02d %06d][S:%010d][A:%s] ", tt.tm_hour, tt.tm_min, tt.tm_sec, timeout.tv_usec, nc->session_id, inet_ntoa(nc->sa.sin.sin_addr));
//		}
//		else
//		{
//			int nSendUsec = tt.tm_sec * 1000000 + timeout.tv_usec;
//			fprintf(m_pLogFile, "\n[%02d:%02d:%02d %06d][S:%010d][A:%s][%08d us] ", tt.tm_hour, tt.tm_min, tt.tm_sec, timeout.tv_usec, nc->session_id, inet_ntoa(nc->sa.sin.sin_addr), nSendUsec - nc->last_usec);
//		}
//#endif
//		fwrite(pChar, nLength, 1, m_pLogFile);
//		fwrite("\n---------------------------------------------------------------", 60, 1, m_pLogFile);
//		fflush(m_pLogFile);
//		m_nCount++;
//		return true;
//	}
//	return false;
//}

void CXHttpFront::HandleMessage(mg_connection* nc, http_message* hm)
{
	//判断是否有链接重用的情况
	if (nc->session_id != 0)
	{
		//链接重用的基础是上次链接已经处理完成了请求
		//if (!nc->m_bServerClose)
		//{
		//	m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_NoRsp, "DuplicateSessionError", "LastSessionNoRsponse");
		//	return;
		//}
		nc->m_bMultiple = true;
	}
	nc->session_id = m_nSessionNo;
	m_nSessionNo++;
	if (m_nSessionNo > MAX_HTTP_SESSIONS - 1)
		m_nSessionNo = 1;

	nc->m_nZipType = 0;
	m_nRequestNo = 0;
	nc->last_usec = -1;
	//header和body之前有一个回车，导致日志无法同时显示，这里做一个小技巧，弄好之后就还原
	int nEnterIndex = hm->message.len - hm->body.len;
	char cEnterReplace1 = hm->message.p[nEnterIndex - 1];
	char cEnterReplace2 = hm->message.p[nEnterIndex - 2];
	((char*)hm->message.p)[nEnterIndex - 1] = '>';
	((char*)hm->message.p)[nEnterIndex - 2] = '-';
	nc->last_usec = -1;
	if (m_pFrontLogger != NULL)
		m_pFrontLogger->WriteLogBase(nc, hm->message.p, hm->message.len);
	m_nSendSecond = nc->last_usec;
	((char*)hm->message.p)[nEnterIndex - 1] = cEnterReplace1;
	((char*)hm->message.p)[nEnterIndex - 2] = cEnterReplace2;

	//先通过/action/v1.0
	if (strncmp(hm->uri.p, m_sHttpMode, strlen(m_sHttpMode)))
	{
		HandleSimpleMessage(nc, hm);
		return;
	}

	m_FieldMetaInput.Length = hm->body.len;
	m_FieldMetaInput.Type = OUTFOG_TYPE_REQUEST;

	size_t i = strlen(m_sHttpMode);
	int j = 0;
	while (hm->uri.p[i] != '/' && i < hm->uri.len)
	{
		m_FieldMetaInput.SubIndexName[j] = hm->uri.p[i];
		i++; j++;
	}

	//if (i < 1)
	//{
	//	SendError(nc, hm, HTTP_ERROR_Url_ID, "Invalid Url");
	//	return;
	//}
	//else
	m_FieldMetaInput.SubIndexName[j] = '\0';

	if (hm->uri.p[i] == '/')
	{
		int j = 0; i++;
		while (hm->uri.p[i] != '/' && i < hm->uri.len)
		{
			m_FieldMetaInput.UserID[j] = hm->uri.p[i];
			i++; j++;
		}
		if (j > 0)
		{
			m_FieldMetaInput.Type = OUTFOG_TYPE_INSERT;
			m_FieldMetaInput.UserID[j] = '\0';
		}
	}
	m_sToken[0] = OUTFOG_REMARK_TOKEN_MANAGER; m_sToken[1] = '\0'; m_sUserID[0] = '\0';
	bool bGetToken = false;
	int nApiKeyIndex = -1; int nTimestampIndex = -1; int nSingIndex = -1;

	//获取Token,这里最大128位
	for (int i = 0; i < MG_MAX_HTTP_HEADERS; i++)
	{
		if (hm->header_names[i].p == NULL)
			break;
		switch (hm->header_names[i].p[0])
		{
		case 'z':
		case 'Z':
		{
			if (strnicmp("ip", hm->header_names[i].p, 2) == 0)
				nc->m_nZipType = hm->header_values[i].p[0] - '0';
			break;
		}
		case 'r':
		case 'R':
		{
			if (strnicmp("equestNo", hm->header_names[i].p, 8) == 0)
				m_nRequestNo = atoi(hm->header_values[i].p);
			break;
		}
		case 'U':
		case 'u':
		case 'M':
		case 'm':
		{
			if (strnicmp("serID", hm->header_names[i].p, 5) == 0 || strnicmp("emberID", hm->header_names[i].p, 7) == 0)
			{
				if (hm->header_values[i].len < 48)
				{
					memcpy(m_sUserID, hm->header_values[i].p, hm->header_values[i].len);
					m_sUserID[hm->header_values[i].len] = '\0';
				}
				else
				{
					memcpy(m_sUserID, hm->header_values[i].p, 47);
					m_sUserID[47] = '\0';
				}
			}
			break;
		}
		case 'o':
		case 'O':
		{
			if (strnicmp("token", hm->header_names[i].p + 1, 5) == 0)
			{
				if (hm->header_values[i].len < 128)
				{
					memcpy(m_sToken, hm->header_values[i].p, hm->header_values[i].len);
					m_sToken[hm->header_values[i].len] = '\0';
				}
				else
				{
					memcpy(m_sToken, hm->header_values[i].p, 127);
					m_sToken[127] = '\0';
				}
				g_SessionMapRWLock.ReadLock();
				CHttpSessionMap::iterator iter = g_SessionMap.find(m_sToken);
				if (iter == g_SessionMap.end())
				{
					m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Token_ID, m_FieldMetaInput.SubIndexName, "no Token exist!");
					g_SessionMapRWLock.UnLock();
					return;
				}
				if (m_nSendSecond > iter->second.ExpireTime)
				{
					m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Token_ID, m_FieldMetaInput.SubIndexName, "Token Expire!");
					g_SessionMapRWLock.UnLock();
					return;
				}
				if (m_nSendSecond - iter->second.HitBeginTime > 1)
				{
					iter->second.HitTimes = 1;
					iter->second.HitBeginTime = m_nSendSecond;
				}
				else
				{
					iter->second.HitTimes++;
				}
				if (iter->second.HitTimes > iter->second.MaxSpeed)
				{
					m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Speed_ID, m_FieldMetaInput.SubIndexName, "Over Max Speed [%d]>[%d]!", iter->second.HitTimes, iter->second.MaxSpeed);
					g_SessionMapRWLock.UnLock();
					return;
				}
				memcpy(m_FieldMetaOutput.UserID, iter->second.UserID.c_str(), iter->second.UserID.size());
				g_SessionMapRWLock.UnLock();
				bGetToken = true;
			}
			break;
		}
		case 'A':
		{
			if (strnicmp("ACCESS-KEY", hm->header_names[i].p, 10) == 0)
				nApiKeyIndex = i;
			if (strnicmp("ACCESS-TIMESTAMP", hm->header_names[i].p, 16) == 0)
				nTimestampIndex = i;
			if (strnicmp("ACCESS-SIGN", hm->header_names[i].p, 11) == 0)
				nSingIndex = i;
			break;
		}
		default:
		{
			continue;
		}
		}
	}

	//没有获取Token，就获取ApiKey，判断参数是否有效，同时认定为token，为了后续能够鉴别身份
	if (!bGetToken)
	{
		if (nApiKeyIndex != -1)
		{
			if (nTimestampIndex == -1 || nSingIndex == -1)
			{
				m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Token_ID, m_FieldMetaInput.SubIndexName, "no ACCESS-TIMESTAMP or ACCESS-SIGN!");
				return;
			}

			if (abs(atoi(hm->header_values[nTimestampIndex].p) - m_nSendSecond) > 30)
			{
				m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Token_ID, m_FieldMetaInput.SubIndexName, "ACCESS-TIMESTAMP Not In Range!");
				return;
			}
			if (hm->header_values[nApiKeyIndex].len < 128)
			{
				memcpy(m_sToken, hm->header_values[nApiKeyIndex].p, hm->header_values[nApiKeyIndex].len);
				m_sToken[hm->header_values[nApiKeyIndex].len] = '\0';
			}
			else
			{
				memcpy(m_sToken, hm->header_values[nApiKeyIndex].p, 128);
				m_sToken[128] = '\0';
			}
			g_SessionMapRWLock.ReadLock();
			CHttpSessionMap::iterator iter = g_SessionMap.find(m_sToken);
			if (iter == g_SessionMap.end())
			{
				m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Token_ID, m_FieldMetaInput.SubIndexName, "no apiKey exist!");
				g_SessionMapRWLock.UnLock();
				return;
			}
			else
			{
				if (m_nSendSecond > iter->second.ExpireTime)
				{
					m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Token_ID, m_FieldMetaInput.SubIndexName, "apiKey Expire!");
					g_SessionMapRWLock.UnLock();
					return;
				}
				if (m_nSendSecond - iter->second.HitBeginTime > 1)
				{
					iter->second.HitTimes = 1;
					iter->second.HitBeginTime = m_nSendSecond;
				}
				else
				{
					iter->second.HitTimes++;
				}
				if (iter->second.HitTimes > iter->second.MaxSpeed)
				{
					m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Speed_ID, m_FieldMetaInput.SubIndexName, "Over Max Speed [%d]>[%d]!", iter->second.HitTimes, iter->second.MaxSpeed);
					g_SessionMapRWLock.UnLock();
					return;
				}
				g_SessionMapRWLock.UnLock();
			}

			if (iter->second.IPAddress.size() > 0)
			{
				//IP地址比较
				char sIPAddress[16];
				sprintf(sIPAddress, "%s", inet_ntoa(nc->sa.sin.sin_addr));

				set<string>::iterator iterip = iter->second.IPAddress.begin();
				while (iterip != iter->second.IPAddress.end())
				{
					if ((*iterip) == sIPAddress)
						break;
					iterip++;
				}
				if (iterip == iter->second.IPAddress.end())
				{
					m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Token_ID, m_FieldMetaInput.SubIndexName, "Ip Not In List!");
					return;
				}
			}

			string sCompareString(hm->header_values[nTimestampIndex].p, hm->header_values[nTimestampIndex].len);
			sCompareString += "POST";
			sCompareString.append(hm->uri.p, hm->uri.len);
			sCompareString.append(hm->body.p, hm->body.len);
			string sSign = HmacShaEncode(iter->second.SecretKey.c_str(), sCompareString.c_str());
			if (strncmp(sSign.c_str(), hm->header_values[nSingIndex].p, hm->header_values[nSingIndex].len) != 0)
			{
				m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Token_ID, m_FieldMetaInput.SubIndexName, "auth is not compare!");
				return;
			}
			//如果是有Token是不需要传递UserID的，因为token可以找到UserID
			//如果有token，需要使用token里面的代替userID
			//strncpy(m_sUserID, iter->second.UserID.c_str(), sizeof(m_sUserID));
		}
	}

	//如果是非manger模式，必须要有OToken，要不然权限控制就失效了
	if (m_sToken[0] == OUTFOG_REMARK_TOKEN_MANAGER && m_sToken[1] == '\0')
	{
		if (!m_bManger)
		{
			m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Token_ID, m_FieldMetaInput.SubIndexName, "no otoken or ACCESS-KEY");
			return;
		}			
	}

	m_FieldMetaOutput.Chain = OUTFOG_DATAFORMAT_STREAM;
	if (!strncmp(hm->method.p, "GET", hm->method.len))
	{
		m_FieldMetaInput.Chain = OUTFOG_DATAFORMAT_FORMDATA;
		HandleSingleMessage(nc, hm);
	}
	else
	{
		m_FieldMetaInput.Chain = OUTFOG_DATAFORMAT_JSON;
		if (*(hm->body.p) == '{')
		{
			HandleSingleMessage(nc, hm);
		}
		else if (*(hm->body.p) == '[')
		{
			HandleManyMessage(nc, hm);
		}
		else
		{
			m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Format_ID, m_FieldMetaInput.SubIndexName, "json not begin with [ or { !");
			return;
		}			
	}
}

bool CXHttpFront::HandleSimpleMessage(mg_connection* nc, http_message* hm)
{
	m_reqPackage.PrepareRequest(0, OUTFOG_CHAIN_SINGLE);
	int i = 1;
	int j = 0;
	while (i < hm->uri.len && j < sizeof(m_reqPackage.GetXTPHeader()->SubIndexName))
	{
		m_reqPackage.GetXTPHeader()->SubIndexName[j] = hm->uri.p[i];
		i++; j++;
	}
	m_reqPackage.GetXTPHeader()->SubIndexName[j] = '\0';

	if (!strcmp(m_reqPackage.GetXTPHeader()->SubIndexName, "ping"))
		return m_pHttpFrontRsp->SendResult(nc, 200, "pong", 4);

	if (!m_bManger)
	{
		strcpy(m_FieldMetaInput.SubIndexName, m_reqPackage.GetXTPHeader()->SubIndexName);
		return m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Url_ID, m_FieldMetaInput.SubIndexName, "vaild url!");
	}

	if (m_nRequestNo >= 0)
	{
		//m_NCMap.insert(CHttpNCMap::value_type(nc->session_id, nc));
		if(!m_pHttpFrontRsp->insertNC(nc))
			return m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Speed_ID, m_FieldMetaInput.SubIndexName, "OverMaxSpeed!");
	}
	else
		return m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Success_ID, m_FieldMetaInput.SubIndexName, "RequestSuccessAndNoKernelResponse");

	m_reqPackage.GetXTPHeader()->FrontNo = m_nFrontNo;
	m_reqPackage.GetXTPHeader()->SessionNo = nc->session_id;
	m_reqPackage.GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
	m_reqPackage.GetXTPHeader()->Type = OUTFOG_TYPE_MANAGER;

	if (!strcmp(m_reqPackage.GetXTPHeader()->SubIndexName, "Start"))
	{
		m_reqPackage.GetXTPHeader()->Type = OUTFOG_TYPE_START;
		//SendError(nc,hm,0, "Start OMQ Message Success!", "RecvStart");
	}

	if (!strcmp(m_reqPackage.GetXTPHeader()->SubIndexName, "Stop"))
	{
		m_reqPackage.GetXTPHeader()->Type = OUTFOG_TYPE_STOP;
		//SendError(nc,hm,0, "Stop OMQ Message Success!", "RecvStop");
	}
	m_reqPackage.GetXTPHeader()->RequestNo = m_nRequestNo;
	m_reqPackage.MakePackage();
	m_pSeriesFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());
	return true;
}

bool CXHttpFront::HandleSingleMessage(mg_connection* nc, http_message* hm)
{
	bool ret = false;
	if (m_pFrontLogger != NULL)
		m_pFrontLogger->WriteLogBase(nc, "1");
	if (m_FieldMetaInput.Chain == OUTFOG_DATAFORMAT_FORMDATA)
	{
		if (hm->query_string.len <= 0)
			return m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Message_ID, m_FieldMetaInput.SubIndexName, "No Get Message");
		m_FieldMetaInput.Length = hm->query_string.len;
		ret = m_pDataTransfer->SyncReqField(&m_FieldMetaInput, (void*)hm->query_string.p, &m_FieldMetaOutput, m_fieldStruct);
	}
	else
	{
		if (hm->body.len <= 0)
			return m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Message_ID, m_FieldMetaInput.SubIndexName, "No Post Body Message");
		m_FieldMetaInput.Length = hm->body.len;
		ret = m_pDataTransfer->SyncReqField(&m_FieldMetaInput, (void*)hm->body.p, &m_FieldMetaOutput, m_fieldStruct);
	}
	if (m_pFrontLogger != NULL)
		m_pFrontLogger->WriteLogBase(nc, "2");

	if (ret)
	{
		if (m_nRequestNo >= 0)
		{
			//m_NCMap.insert(CHttpNCMap::value_type(nc->session_id, nc));
			if (!m_pHttpFrontRsp->insertNC(nc))
				return m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Speed_ID, m_FieldMetaInput.SubIndexName, "OverMaxSpeed!");
		}
		else
			return m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Success_ID, m_FieldMetaInput.SubIndexName, "RequestSuccessAndNoKernelResponse");

		m_reqPackage.PrepareRequest(0, OUTFOG_CHAIN_SINGLE);
		m_reqPackage.GetXTPHeader()->FrontNo = m_nFrontNo;
		m_reqPackage.GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
		m_reqPackage.GetXTPHeader()->SessionNo = nc->session_id;
		m_reqPackage.GetXTPHeader()->RequestNo = m_nRequestNo;
		m_reqPackage.GetXTPHeader()->BusinessNo = LINK_INNER_BEGIN;
		m_reqPackage.GetXTPHeader()->MessageNo = m_FieldMetaOutput.MessageNo;
		//先拷贝UserID,这样token可以覆盖
		strcpy(m_reqPackage.GetXTPHeader()->UserID, m_sUserID);
		strcpy(m_reqPackage.GetXTPHeader()->Remark, m_sToken);
	
	   	m_reqPackage.AddStream(m_FieldMetaOutput.FieldNo, m_fieldStruct, m_FieldMetaOutput.Length);
		m_reqPackage.MakePackage();
		m_pSeriesFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());

		if (m_pFrontLogger != NULL)
			m_pFrontLogger->WriteLogBase(nc, "3");
		return true;
	}
	else
	{
		return m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Format_ID, m_FieldMetaInput.SubIndexName, m_fieldStruct);
	}
}

bool CXHttpFront::HandleManyMessage(mg_connection* nc, http_message* hm)
{
	m_reqPackage.PrepareRequest(0, OUTFOG_CHAIN_SINGLE);
	//m_reqPackage.GetXTPHeader()->FrontNo = m_nFrontNo;
	m_reqPackage.GetXTPHeader()->FrontNo = INT_32_MAX;
	m_reqPackage.GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
	m_reqPackage.GetXTPHeader()->RequestNo = -1;
	strcpy(m_reqPackage.GetXTPHeader()->Remark, m_sToken);
	int nCount = 0;
	const char* pFieldEnd = hm->body.p;
	int nFieldTempIndex = -1;
	while (pFieldEnd < hm->body.p + hm->body.len)
	{
		if (*pFieldEnd == '{')
		{
			nFieldTempIndex = 0;
		}
		else if (*pFieldEnd == '}')
		{
			m_fieldTemp[nFieldTempIndex] = *pFieldEnd;
			m_fieldTemp[nFieldTempIndex + 1] = '\0';
			nFieldTempIndex = -1;
			m_FieldMetaInput.Length = nFieldTempIndex + 1;
			bool ret = m_pDataTransfer->SyncReqField(&m_FieldMetaInput, (void*)m_fieldTemp, &m_FieldMetaOutput, m_fieldStruct);
			if (ret)
			{
				m_reqPackage.GetXTPHeader()->MessageNo = m_FieldMetaOutput.MessageNo;
				if (m_reqPackage.AddStream(m_FieldMetaOutput.FieldNo, m_fieldStruct, m_FieldMetaOutput.Length) == NULL)
				{
					m_reqPackage.GetXTPHeader()->MessageNo = m_FieldMetaOutput.MessageNo;
					m_reqPackage.MakePackage();
					m_pSeriesFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());
					m_reqPackage.PrepareRequest(0, OUTFOG_CHAIN_SINGLE);
					//m_reqPackage.GetXTPHeader()->FrontNo = m_nFrontNo;
					m_reqPackage.GetXTPHeader()->FrontNo = INT_32_MAX;
					m_reqPackage.GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
					m_reqPackage.GetXTPHeader()->MessageNo = m_FieldMetaOutput.MessageNo;
					m_reqPackage.GetXTPHeader()->RequestNo = -1;
					strcpy(m_reqPackage.GetXTPHeader()->UserID, m_sUserID);
					strcpy(m_reqPackage.GetXTPHeader()->Remark, m_sToken);
					m_reqPackage.AddStream(m_FieldMetaOutput.FieldNo, m_fieldStruct, m_FieldMetaOutput.Length);
				}
				nCount++;
				if (!m_bManger && nCount > m_nNormalBatchs)
					return m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_BatchLimit_ID, m_FieldMetaInput.SubIndexName, "TooManyBatchMessagesForNormalUser[%d>%d]", nCount, m_nNormalBatchs);
			}
			else
			{
				return m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Format_ID, m_FieldMetaInput.SubIndexName, "FieldEndError");
			}
		}
		if (nFieldTempIndex >= 0)
		{
			m_fieldTemp[nFieldTempIndex] = *pFieldEnd;
			nFieldTempIndex++;
		}
		pFieldEnd++;
	}

	if (nCount > 0)
	{
		m_reqPackage.MakePackage();
		m_pSeriesFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());
		//如果是批量操作，无需上层回复
		m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Success_ID, m_FieldMetaInput.SubIndexName, "BatchMessageSuccess[%d]", nCount);
		return true;
	}
	else
	{
		return m_pHttpFrontRsp->SendError(nc, hm, HTTP_ERROR_Message_ID, m_FieldMetaInput.SubIndexName, "NoPostBodyMessage");
	}
}

void CXHttpFront::Run()
{
	while (true)
	{
		mg_mgr_poll(&mgr, 1);
	}
	//for (;;)
	//{
	//	//mg_mgr_poll(&mgr, m_nCPUMode);
	//	for (int j = 0; j < 100; j++)
	//	{
	//		if (mg_mgr_poll(&mgr, 1) == 0)
	//			break;
	//		//int a = 0;
	//		//GET_ONLY_MSEC_TIME(a);
	//		//REPORT_EVENT(LOG_CRITICAL, "CXHttpFront0", "%d", a);
	//	}

	//	////读取上层信息
	//	//for (int i = 0; i < 100; i++)
	//	//{
	//	//	if (!m_ResultReader.GetNext(&m_rspPackage))
	//	//		break;
	//	//	//GET_ONLY_MSEC_TIME(a);
	//	//	//REPORT_EVENT(LOG_CRITICAL, "CXHttpFront1", "%0.f:%d", m_rspPackage.GetXTPHeader()->GlobalNo, a);

	//	//	HandleResponse(&m_rspPackage, m_rspPackage.GetXTPHeader()->MessageNo, m_rspPackage.GetXTPHeader()->SessionNo);

	//	//	//GET_ONLY_MSEC_TIME(a);
	//	//	//REPORT_EVENT(LOG_CRITICAL, "CXHttpFront2", "%0.f:%d", m_rspPackage.GetXTPHeader()->GlobalNo, a);
	//	//}
	//}
}