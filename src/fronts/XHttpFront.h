#ifndef _XHTTP_FRONT_H_
#define _XHTTP_FRONT_H_

#include "mongoose.h"
#include "CachedFlow.h"
#include "SelectReactor.h"
#include "Config.h"
#include "LinkManager.h"
#include "printinfo.h"
#include "OutFogServiceApi.h"
#include "FlowManager.h"
#include "ServiceResponser.h"
#include "FrontLogger.h"

#define MAX_HTTP_SESSIONS 102400

//typedef map<int, mg_connection*> CHttpNCMap;

//extern CRWLock g_HttpFrontNCMapRWLock;

class CHttpFrontRsp : public CThread
{
public:
	CHttpFrontRsp(CReadFlow *pFlow, CFrontLogger* pFrontLogger)
	{
		m_ResultReader.AttachFlow(pFlow, 0);
		//		m_pNCMap = pHttpNCMap;
		m_rspPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
		m_pFrontLogger = pFrontLogger;
		m_pDataTransfer = NULL;
		for (int i = 0; i < MAX_HTTP_SESSIONS; i++)
			m_NCVector[i] = NULL;
	}

//	bool SendResultHeader(struct mg_connection* nc, int status_code);
	bool HandleResponse(CXTPPackage *pPackage, int TNo, int nSessionID);

	bool SendResult(struct mg_connection* nc, int status_code, const char* result, size_t nLenth = 0);
	bool SendResultHeader(struct mg_connection* nc, int status_code);
	bool SendError(struct mg_connection* nc, http_message* hm, int nErrorID, const char* sAction, const char *format, ...);

	virtual void Run()
	{
		//读取上层信息
		while (true)
		{
			for (int i = 0; i < 10; i++)
			{
				if (!m_ResultReader.GetNext(&m_rspPackage))
				{
#ifdef WIN32
					::Sleep(1);
#else
					::usleep(100);
#endif
					break;
				}
				m_Mutex.Lock();
				HandleResponse(&m_rspPackage, m_rspPackage.GetXTPHeader()->MessageNo, m_rspPackage.GetXTPHeader()->SessionNo);
				m_Mutex.UnLock();
			}
		}
	}

	bool insertNC(mg_connection* nc)
	{
		if (nc->session_id < 1 || nc->session_id >= MAX_HTTP_SESSIONS)
		{
			if (m_pFrontLogger != NULL)
				m_pFrontLogger->WriteLog(NULL, "InsertNC:InsertSession[%d]Error", nc->session_id);
			return false;
		}
		mg_connection* pnc = (mg_connection*)(m_NCVector[nc->session_id]);
		if (pnc == NULL)
		{
			m_NCVector[nc->session_id] = nc;
			if (m_pFrontLogger != NULL)
				m_pFrontLogger->WriteLog(nc, "InsertNC:InsertSession[%d][0x%012x]", nc->session_id, nc);
		}
		else
		{
			//如果里面的链接没有处理完成
			if (pnc != nc)
			{
				m_NCVector[nc->session_id] = nc;
				if (m_pFrontLogger != NULL)
					m_pFrontLogger->WriteLog(nc, "InsertNC:InsertSessionReplace[%d][0x%012x]->[0x%012x]", nc->session_id, nc, pnc);
			}
		}
		return true;
	}

	void deleteNC(mg_connection* nc)
	{
		//这里需要注意多线程调用的情况下，和发送线程加锁
		if (nc->session_id < 1 || nc->session_id >= MAX_HTTP_SESSIONS)
		{
			if (m_pFrontLogger != NULL)
				m_pFrontLogger->WriteLog((mg_connection*)m_NCVector[nc->session_id], "InsertNC:DeleteSession[%d]Error", nc->session_id);
			return;
		}

		if (m_NCVector[nc->session_id] == NULL)
			return;

		if (m_NCVector[nc->session_id] != nc)
		{
			//漏网之鱼不能随便上次，也需要确定发送线程完全发送完成之后才能删除
			m_Mutex.Lock();
			if (m_pFrontLogger != NULL)
				m_pFrontLogger->WriteLog((mg_connection*)m_NCVector[nc->session_id], "InsertNC:DeleteSessionError[%d][0x%012x]!=[0x%012x]", nc->session_id, m_NCVector[nc->session_id], nc);
			m_Mutex.UnLock();
			return;
		}
		else
		{
			if (m_pFrontLogger != NULL)
				m_pFrontLogger->WriteLog((mg_connection*)m_NCVector[nc->session_id], "InsertNC:DeleteSession[%d][0x%012x]", nc->session_id, m_NCVector[nc->session_id]);
			
			if (nc->m_bMultiple)
			{
				m_Mutex.Lock();
				m_NCVector[nc->session_id] = NULL;
				m_Mutex.UnLock();
			}
			else
			{
				if (m_NCVector[nc->session_id]->m_bServerClose)
				{
					m_NCVector[nc->session_id] = NULL;
				}
				else
				{
					m_Mutex.Lock();
					m_NCVector[nc->session_id] = NULL;
					m_Mutex.UnLock();
				}
			}
		}
	}

	COutFogServiceApi *m_pDataTransfer;

private:
	CFlowReader m_ResultReader;
	//CHttpNCMap* m_pNCMap;
	CXTPPackage m_rspPackage;
	CFrontLogger* m_pFrontLogger;
	CFlowManager *m_pFlowManager;
	TOutFogMetaRef m_FieldMetaInput;
	TOutFogMetaRef m_FieldMetaOutput;
	char m_fieldStruct[8 * OUTFOG_MAX_PACKAGE_SIZE];
	char m_fieldStream[8 * OUTFOG_MAX_PACKAGE_SIZE];
	char m_SendBuff[16 * OUTFOG_MAX_PACKAGE_SIZE];
	string m_SendString;
	volatile mg_connection* m_NCVector[MAX_HTTP_SESSIONS];
	CMutex m_Mutex;
	//CMutex m_MuiltMutex;  //nc被重复使用的情况下，断线和发送需要加锁
};

class CXHttpFront :public COutFogServiceApi, public CThread, public COutFogServiceCallback
{
public:
	//pTSeries是前置的输出
	CXHttpFront(CReadFlow* pFlow, CFlow* pSeriesFlow, CFlowManager *pFlowManager, const char* pFrontAddress, int nFrontid, int nPortNumber);
	virtual ~CXHttpFront();

	virtual void HandleClose(mg_connection* nc, http_message* hm);
	virtual void HandleMessage(mg_connection* nc, http_message* hm);
	//bool HandleResponse(CXTPPackage *pPackage, int TNo, int nSessionID);

	virtual bool HandleSimpleMessage(mg_connection* nc, http_message* hm);
	virtual bool HandleSingleMessage(mg_connection* nc, http_message* hm);
	virtual bool HandleManyMessage(mg_connection* nc, http_message* hm);

	virtual const char* getVersion();
	virtual bool ReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pStream);
	virtual bool RspField(const TOutFogMetaRef* pRspRef, const void *pRspBuffer)
	{
		CopyHeadMeta(m_reqPackage.GetXTPHeader(), pRspRef);
		m_reqPackage.GetXTPHeader()->FrontNo = m_nFrontNo;
		m_reqPackage.AddStream(pRspRef->FieldNo, (const char *)pRspBuffer, pRspRef->Length);
		m_reqPackage.MakePackage();
		m_pSeriesFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());
		return true;
	}
	virtual bool SyncReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pReqStream, TOutFogMetaRef* pOutFogRspRef, void *pRspStream)
	{
		return true;
	}
	void setDataSwapApi(COutFogServiceApi *pServiceApi)
	{
		m_pDataTransfer = pServiceApi;
		m_pHttpFrontRsp->m_pDataTransfer = pServiceApi;
	}

	//bool SendResult(struct mg_connection* nc, int status_code, const char* result, size_t nLenth = 0);
	//bool SendResultHeader(struct mg_connection* nc, int status_code);
	//	bool SendError(struct mg_connection* nc, http_message* hm, int nErrorID, const char* sErrorMsg, const char* sAction="unknow");
	//bool SendError(struct mg_connection* nc, http_message* hm, int nErrorID, const char *format, ...);
	//	bool WriteLog(struct mg_connection* nc, const char* pChar, size_t nLength);

	virtual void Run();
private:
	CFlow* m_pSeriesFlow;

	CXTPPackage m_reqPackage;
	//	CXTPPackage m_rspPackage;
	//	CFlowReader m_ResultReader;
	char m_fieldStruct[4 * OUTFOG_MAX_PACKAGE_SIZE];
	//	char m_fieldStream[4 * OUTFOG_MAX_PACKAGE_SIZE];
	char m_SendBuff[4 * OUTFOG_MAX_PACKAGE_SIZE];
	char m_fieldTemp[4 * OUTFOG_MAX_PACKAGE_SIZE];
	//	string m_SendString;

	TOutFogMetaRef m_FieldMetaInput;
	TOutFogMetaRef m_FieldMetaOutput;
	int m_nFrontNo;
	char m_FrontAddress[512];

	COutFogServiceApi *m_pDataTransfer;

	struct mg_mgr mgr;

	//string m_sError;
	//char m_sError[512];

	//CHttpNCMap m_NCMap;
	int m_nSessionNo;
	int m_nRequestNo;

	//	FILE *m_pLogFile;
	//	char m_fDate[13];
	//	string m_strFlowPathName;
	//	int m_nCount;
	//	int m_nGap;
	time_t m_nSendSecond;	 //记录操作的秒数，用于判断时间

	//int m_nCPUMode;
	bool m_bManger; //是否是管理员使用模式
	char m_sHttpMode[32];
	char m_sTempAuthCode[1024];
	int m_nNormalBatchs;
	char m_sToken[128];
	char m_sUserID[48];
	CFrontLogger *m_pFrontLogger;
	CHttpFrontRsp* m_pHttpFrontRsp;
};

typedef map<mg_mgr*, CXHttpFront *> CHttpFrontMap;

#endif
