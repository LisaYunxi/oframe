#ifndef __WSSERVER_H__
#define __WSSERVER_H__

#include "FrontLogger.h"
//#include "mongoose.h"
#include "Config.h"
#include "Thread.h"
#include "WSFront.h"
#include "OutFogService.h"
#include "CachedFlow.h"
#include "Config.h"
#include "LinkManager.h"
#include "printinfo.h"
#include "OutFogServiceApi.h"
#include "FlowManager.h"
#include "FTDCPackage.h"
#include "ServiceResponser.h"
#include "LinkManager.h"

typedef map<int, CFlowReader*> CWsFlowReaderMap;


class CWsSession
{
public:
	CWsSession(COutFogServiceApi* pDataTransfer, mg_connection* nc, CFrontLogger *pCallback);
	virtual ~CWsSession();
	virtual int HandleOtherTask(bool* pSleep);
	//bool HandleNotify(CFieldSet *pPackage, int TNo, int nLocalNo, int nErrorCode, char* sErrorMsg, UF_INT8 SequenceNo = -1.0);
	int HandleNotify(CFTDCPackage *pPackage);
	bool HandleResponse(CXTPPackage *pPackage);
	//多线程发送报文
	bool SendSimpleMessage(char *pMessage, int nLength);

	int GetSessionID();

	bool Publish(CReadFlow *pFlow, int nSequenceSeries, UF_INT8 nStartId, UF_INT8 nBusinessNo = UF_INT8_MAX, int nReaderTimes = 1);
	bool UnPublish(int nSequenceSeries);
	void UnPublishAll();
	//bool WriteLog(const char* pChar, int nLength);
	void DisConnect();

	mg_connection* m_pnc;
	int m_nZipType;			//0:不压缩;1:短字符压缩;2:二进制
	char m_sToken[128];
	char m_sUserID[48];
private:
	CReadFlow* m_pTradeResult;

	CFTDCPackage m_rspPackage;

	char m_fieldStruct[4 * OUTFOG_MAX_PACKAGE_SIZE];
	char m_fieldStream[4 * OUTFOG_MAX_PACKAGE_SIZE];
//	char m_SendBuff[4 * OUTFOG_MAX_PACKAGE_SIZE];
	TOutFogMetaRef m_FieldMetaInput;
	TOutFogMetaRef m_FieldMetaOutput;

	CWsFlowReaderMap m_WsFlowReaderMap;
	COutFogServiceApi *m_pDataTransfer;

	string m_RspString;

	CFrontLogger *m_pCallback;
	CFlow *m_pFlow;
	CFlowReader m_reader;
};

typedef map<int, CWsSession*> CWsSessionMap;
typedef set<int> CWsSessionSet;

class CWsFrontRsp : public CThread
{
public:
	CWsFrontRsp(CReadFlow *pFlow, CFrontLogger* pFrontLogger, CFlowManager *pFlowManager)
	{
		m_ResultReader.AttachFlow(pFlow, 0);
		m_rspPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
		m_nLastPackages = 0;
		m_nLastCountTime = 0;
		m_nLastCounts = 0;
		m_pFrontLogger = pFrontLogger;
		m_pFlowManager = pFlowManager;
	}
	virtual ~CWsFrontRsp()
	{
		CWsSessionMap::iterator iter = m_WsSessionMap.begin();
		while (iter != m_WsSessionMap.end())
		{
			CWsSession* pWsSession = iter->second;
			m_WsSessionMap.erase(iter++);
			delete pWsSession;
		}
	}

	bool SendError(mg_connection* nc, websocket_message* wm, int nErrorID, const char *format, ...);
	bool HandleResponse(CXTPPackage *pMessage);

	virtual void Run()
	{
		//读取上层信息
		while (true)
		{
			int nCount = 0;
			//time_t now = time(NULL);
			time_t now = (time_t)mg_time();
			bool bSleep = true;
			//这一步为了发送有一些没有来得及发送的东西
			m_WsSessionMapRWLock.ReadLock();
			CWsSessionMap::iterator iter = m_WsSessionMap.begin();
			while (iter != m_WsSessionMap.end())
			{
				CWsSession* pWsSession = iter->second;
				bool bSleepIn;
				int nDealCount = pWsSession->HandleOtherTask(&bSleepIn);
				if (!bSleepIn)
					bSleep = false;
				mg_if_can_send_cb(pWsSession->m_pnc);

				if (nDealCount > 10 && m_pFrontLogger != NULL)
					m_pFrontLogger->WriteLog(pWsSession->m_pnc, "Deal %d Messages", nDealCount);

				m_nLastCounts += nDealCount;
				nCount += nDealCount;
				if (nDealCount > 0)
					m_pLastActiveSessionSet.insert(pWsSession->GetSessionID());

				//如果长时间没有消息，可以断开
				//if (pWsSession->m_pnc->last_io_time < now - 60 * 60)
				if (pWsSession->m_pnc->last_io_time < now - g_nWSHeartBeat)
				{
					if (m_pFrontLogger != NULL)
						m_pFrontLogger->WriteLog(pWsSession->m_pnc, "Long Time No Message,DisConnect");
					pWsSession->DisConnect();
					m_WsSessionMap.erase(iter++);
				}
				else
				{
					iter++;
				}
			}

			bool bSleepx = false;
			for (int i = 0; i < 10; i++)
			{
				if (!m_ResultReader.GetNext(&m_rspPackage))
				{
					bSleepx = true;
					break;
				}					
				HandleResponse(&m_rspPackage);
				m_nLastCounts++;
				nCount++;
			}

			if (m_nLastCounts >= g_nMaxPackages * 10)
			{
				time_t nTimeGap = now - m_nLastCountTime;
				if (nTimeGap > 0)
					m_nLastPackages = ((double)m_nLastCounts) / nTimeGap;
				else
					m_nLastPackages = m_nLastCounts;

				if (m_pFrontLogger != NULL)
					m_pFrontLogger->WriteLog(NULL, "ThreadUD[%d]:%zd/%zd Sessions %d Messages by %zd Seconds: %0.f/s",
						m_IDThread, m_pLastActiveSessionSet.size(), m_WsSessionMap.size(), m_nLastCounts, nTimeGap, m_nLastPackages);

				m_nLastCountTime = now;
				m_nLastCounts = 0;
				m_pLastActiveSessionSet.clear();
			}

			m_WsSessionMapRWLock.UnLock();

			if (bSleep && bSleepx)
			{
#ifdef WIN32
				::Sleep(1);
#else
				::usleep(10);
#endif
			}
		}
	}

	bool deleteSession(mg_connection* nc)
	{
		m_WsSessionMapRWLock.WriteLock();
		CWsSessionMap::iterator iter = m_WsSessionMap.find((int)nc->session_id);
		if (iter == m_WsSessionMap.end())
		{
			if (m_pFrontLogger != NULL)
				m_pFrontLogger->WriteLogBase(nc, "Disconnected Error:not in map!");
			m_WsSessionMapRWLock.UnLock();
			return false;
		}
		delete iter->second;
		m_WsSessionMap.erase(iter);
		m_WsSessionMapRWLock.UnLock();
		return true;
	}

	CWsSession* getSession(mg_connection* nc)
	{
		m_WsSessionMapRWLock.ReadLock();
		CWsSessionMap::iterator iter = m_WsSessionMap.find((int)nc->session_id);
		if (iter == m_WsSessionMap.end())
		{
			m_WsSessionMapRWLock.UnLock();
			return NULL;
		}
		m_WsSessionMapRWLock.UnLock();
		return iter->second;
	}

	bool insertSession(CWsSession* pWsSession)
	{
		m_WsSessionMapRWLock.WriteLock();

		if (m_WsSessionMap.size() > g_nMaxSessions)
		{
			if (m_pFrontLogger != NULL)
				m_pFrontLogger->WriteLog(pWsSession->m_pnc, "MaxSessionsLimit:%zd>%d", m_WsSessionMap.size(), g_nMaxSessions);
			m_WsSessionMapRWLock.UnLock();
			return false;
		}

		m_WsSessionMap.insert(CWsSessionMap::value_type(pWsSession->GetSessionID(), pWsSession));
		m_WsSessionMapRWLock.UnLock();
		return true;
	}
	CRWLock m_WsSessionMapRWLock;

private:
	CWsSessionMap m_WsSessionMap;
	CFlowReader m_ResultReader;
	CXTPPackage m_rspPackage;
	time_t m_nLastCountTime;
	int m_nLastCounts;
	CWsSessionSet m_pLastActiveSessionSet;
	double m_nLastPackages;
	CFrontLogger* m_pFrontLogger;
	CFlowManager *m_pFlowManager;
};

class CWSFront :public COutFogServiceApi, public CThread, public COutFogServiceCallback
{
public:
	CWSFront(CReadFlow* pFlow, CFlow* pSeriesFlow, CFlowManager *pFlowManager, const char* pFrontAddress, int nFrontid, int nPortNumber, int nThreads);
	virtual ~CWSFront();
	void Run();

//	virtual bool onAccept(mg_connection* nc);
	virtual bool onConnect(mg_connection* nc, http_message *hm);
	virtual bool onDisConnect(mg_connection* nc);
	virtual bool onHttpRequest(mg_connection* nc, http_message *hm);

	virtual bool HandleMessage(mg_connection* nc, websocket_message *wm);

	//bool HandleResponse(CXTPPackage *pMessage);

	virtual const char* getVersion();
	virtual bool ReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pStream);
	virtual bool SyncReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pReqStream, TOutFogMetaRef* pOutFogRspRef, void *pRspStream)
	{
		return true;
	}
	virtual bool RspField(const TOutFogMetaRef* pRspRef, const void *pRspBuffer)
	{
		CopyHeadMeta(m_reqPackage.GetXTPHeader(), pRspRef);
		m_reqPackage.GetXTPHeader()->FrontNo = m_nFrontNo;
		m_reqPackage.AddStream(pRspRef->FieldNo, (const char *)pRspBuffer, pRspRef->Length);
		m_reqPackage.MakePackage();
		m_pSeriesFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());
		return true;
	}
	void setDataSwapApi(COutFogServiceApi *pServiceApi)
	{
		m_pDataTransfer = pServiceApi;
	}
	vector<CWsFrontRsp*> m_WsFrontRsps;

private:
	bool SendSimpleError(CWsSession* pWsSession, websocket_message* wm, int nErrorID, const char *format, ...);
	struct mg_mgr mgr;
	COutFogServiceApi *m_pDataTransfer;
	CFlow* m_pSeriesFlow;
	//CFlowReader m_ResultReader;
	CXTPPackage m_reqPackage;
	//CXTPPackage m_rspPackage;
	char m_fieldStruct[OUTFOG_MAX_PACKAGE_SIZE];
	char m_fieldStream[OUTFOG_MAX_PACKAGE_SIZE];
	TOutFogMetaRef m_FieldMetaInput;
	TOutFogMetaRef m_FieldMetaOutput;
	int m_nFrontNo;
	char m_FrontAddress[512];
	
	//CWsSessionMap m_pWsSessionMap;

	int m_nSessionID;

	//CFlowManager *m_pFlowManager;
	//FILE *m_pLogFile;
	//char m_fDate[13];
	//string m_strFlowPathName;
	//int m_nCount;
	//int m_nGap;

	//int m_nCPUMode;
	//time_t m_nLastCountTime;
	//int m_nLastCounts;
	//CWsSessionSet m_pLastActiveSessionSet;
	//double m_nLastPackages;
	//int m_nSendUsec;	//记录操作的微秒数，用于后续延时统计
	bool m_bManger;		//是否是管理员使用模式

	CFrontLogger* m_pFrontLogger;
};

typedef map<mg_mgr*, CWSFront *> CWSFrontMap;

#endif
