/////////////////////////////////////////////////////////////////////////
///@system 
///@file KernelApp.cpp
///@brief omq 的核心框架代码
///@history 
///20200703	徐忠华 创建该文件
/////////////////////////////////////////////////////////////////////////

#include "XHttpFront.h"
#include "WSFront.h"
#include "platform.h"
#include "public.h"
#include "monitorIndex.h"
#include "CFilelogService.h"
#include "Config.h"
#include "CachedFlow.h"
#include "SelectReactor.h"
#include "PubFlow.h"
#include "utility.h"

#include "SimpleReactor.h"
#include "LinkManager.h"
#include "FtdFront.h"
#include "FileFlow.h"
#include "version.h"
#include "OutFogService.h"

#include "BaseFunction.h"
#include "versionValue.h"
//#include "strapi.h"
#include "FlowDispatcher.h"

//ppoll函数对应头文件
#ifdef LINUX
#include <poll.h>
#endif

#include "OneTimeFlow.h"
#include "GatewayFront.h"
#include "CompServer.h"
#include "ServiceResponser.h"

#define ReliableMode_Single 1
#define ReliableMode_HotStandby 2
#define ReliableMode_Cluster 3

const int FRONT_TIMER_TIMER_ID = 10;

#define INI_FILE_NAME "omq.ini"

class CKernelTimer :public CEventHandler, public COutFogServiceApi
{
public:
	CKernelTimer(CReactor *pReactor, int nTimerInterval, CFlow *pFlow) :CEventHandler(pReactor)
	{
		m_pReactor = pReactor;
		SetTimer(FRONT_TIMER_TIMER_ID, nTimerInterval);
		m_pFlow = pFlow;
		m_reqPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
	}
	virtual ~CKernelTimer()
	{
	}
	virtual const char* getVersion()
	{
		return "KernelTimer";
	}
	virtual bool ReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pStream)
	{
		if (pOutFogReqRef->Type == OUTFOG_TYPE_START)
		{
			m_pReactor->Create();
		}
		return true;
	}
	virtual bool SyncReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pReqStream, TOutFogMetaRef* pOutFogRspRef, void *pRspStream)
	{
		return true;
	}
	virtual bool Start()
	{
		return true;
	}

	virtual void OnTimer(int nIDEvent)
	{
		switch (nIDEvent)
		{
		case FRONT_TIMER_TIMER_ID:
		{
			m_reqPackage.PreparePublish(OUTFOG_TNO_NULL, OUTFOG_CHAIN_LAST);
			time_t dwCurrTime = (time_t)m_pReactor->Time();
			struct tm timeinfo2 = GetTM(&dwCurrTime);
//#ifdef WINDOWS
//			//struct tm* timeinfo = localtime(&dwCurrTime);
//			localtime_s(&timeinfo2,&dwCurrTime);
//#else
//			//struct tm* timeinfo = localtime(&dwCurrTime);
//			localtime_r(&dwCurrTime, &timeinfo2);
//#endif
			//strftime(m_reqPackage.GetXTPHeader()->Remark, 11, "%H:%M:%S", timeinfo);
			//strftime(m_reqPackage.GetXTPHeader()->SubIndexName, 11, "%Y%m%d", &timeinfo);

			strftime(m_reqPackage.GetXTPHeader()->SubIndexName, 11, "%Y%m%d", &timeinfo2);
			strftime(m_reqPackage.GetXTPHeader()->UserID, 11, "%H:%M:%S", &timeinfo2);
			sprintf(m_reqPackage.GetXTPHeader()->Remark, "%d:%d", timeinfo2.tm_wday,timeinfo2.tm_yday);
			
			if (m_nTimeZone < 0)
			{
				//按照00:00:00计算出一个一个余数，这个就是和UTC时间的差值
				int UTCSeconds = getSecondMinus(m_reqPackage.GetXTPHeader()->UserID, "00:00:00");
				m_nTimeZone = (dwCurrTime - UTCSeconds) % (24 * 60 * 60);
			}
			m_reqPackage.GetXTPHeader()->SubjectNo = m_nTimeZone;

			//if (strcmp(m_reqPackage.GetXTPHeader()->Remark, m_reqPackage.GetXTPHeader()->UserID))
			//{
			//	REPORT_EVENT(LOG_CRITICAL, "CKernelTimer", "Error:localtime_r[%s] != localtime[%s] ", m_reqPackage.GetXTPHeader()->UserID, m_reqPackage.GetXTPHeader()->Remark);
			//}

	//		UF_INT8 nCurrTime = dwCurrTime;
	//		REPORT_EVENT(LOG_CRITICAL, "CKernelTimer", "NewTime[%0.f][%s][%s]", nCurrTime, m_reqPackage.GetXTPHeader()->UserID, m_reqPackage.GetXTPHeader()->Remark);
			
			m_reqPackage.GetXTPHeader()->Type = OUTFOG_TYPE_TIMER;
			m_reqPackage.GetXTPHeader()->RequestNo = m_pReactor->GetMilTime();
			m_reqPackage.GetXTPHeader()->SessionNo = m_pReactor->Time();
			//m_reqPackage.GetXTPHeader()->BusinessNo = m_pReactor->GetClock();
			m_reqPackage.GetXTPHeader()->BusinessNo = m_pReactor->Time();
			m_reqPackage.MakePackage();
			m_pFlow->Append(m_reqPackage.Address(), m_reqPackage.Length());
			break;
		}
		default:
		{
		}
		}
	}
protected:
	CReactor *m_pReactor;
	CFlow* m_pFlow;
	CXTPPackage m_reqPackage;
	int m_nTimeZone = -1;
};

class CKernelAppCallback
{
public:
	virtual bool RspPackage(CXTPPackage *pMessage) = 0;
};


class CKernelApp : public CThread, public CStatusCtrlInterfaces
{
public:
	CKernelApp(int nKernelid, COutFogServiceCallback* pCallback = NULL);
	TCompStateID GetCompStateID();

	void SetKernelAppCallback(CKernelAppCallback* pCallback);

	void FirstReDo();
	//void SetReDoID(UF_INT8 nID);
	bool RspField(const TOutFogMetaRef* pOutFogRspRef, const void *pStream);
	bool RspField(CXTPPackage *pMessage);

	bool AppendToFlow(CXTPPackage *pMessage);

	bool m_bFinishInitInstance;
	CCachedFlow* m_pPreSeries;      //准备发送的流水，如果没有排队机，就是m_pTradeSeries	
	CLinkManager* m_pLinkManager;

	CGateWayFront* m_pGatewayFront;
	CFlowDispatcher* m_pFlowDispatcher;

	CFtdFront* m_pKernelFront;
	//CHttpFront *m_pHttpFront;
	CXHttpFront *m_pHttpFront;
	CWSFront *m_pWsFront;
	//发送流水的集合
	CFlowManager* m_pFlowManager;

	virtual bool InitInstance();
	virtual void ExitInstance();
	int Redo();
	void RealDo();
	virtual UF_INT8 GetHandleCount();
	virtual void Run();
	void StartAddin();
	virtual bool EnterSlaver();
	virtual bool EnterSingle();
	virtual bool EnterMaster();
	virtual void ConfirmQueued(UF_INT8 nKey);
	int handleMessage(CXTPPackage *pPackage);

private:
	COutFogService *m_pServiceLoad;
	COutFogServiceCallback* m_pCallback;
	bool m_bFastRunMode;//是否是快速模式，快速模式没有输入流
	int m_nKernelid;//启动时候kernel带的号码，如果是0，说明没有带号码,1或者2代表是主备模式


	CCachedFlow* m_pTradeSeries;    //可以发送的输入流
	CFlow* m_pPreResult;			//结果流
	CFlow* m_pTradeResult;			//可以发送的结果流
	CPubFlow* m_pResultPubFlow;		//主备结果流处理接口

	CXTPPackage m_pubPackage;
	CXTPPackage *m_pPackage;

	CFlowReader m_reader;
	volatile TCompStateID m_CompStateID;
	bool m_bStartAddin;
	int m_nCPUMode;//CPU的运行模式，sleep的微秒数目
	int m_nReliableMode;//可靠性方式
	char pStreamBuf[OUTFOG_MAX_PACKAGE_SIZE];
	TOutFogMetaRef m_OutFogMetaRef;
	UF_INT8 m_nReDoID;//开始运行从m_pTradeSeries的哪个序号开始
	UF_INT8 m_nCount;//目前处理了第几个序号
	int m_CurrPackageTime;//目前处理报文的时间戳
	int m_nFlowCacheSize;//流水内存的大小
	CKernelAppCallback* m_pPackageCallback;

	double m_nLatencySums; //记录延时数据的和
	int m_nLatencyTimes;//记录延时数据的次数

	bool m_bRuning;//是否在工作
};