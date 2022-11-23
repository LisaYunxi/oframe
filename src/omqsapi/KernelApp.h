/////////////////////////////////////////////////////////////////////////
///@system 
///@file KernelApp.cpp
///@brief omq �ĺ��Ŀ�ܴ���
///@history 
///20200703	���һ� �������ļ�
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

//ppoll������Ӧͷ�ļ�
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
				//����00:00:00�����һ��һ��������������Ǻ�UTCʱ��Ĳ�ֵ
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
	CCachedFlow* m_pPreSeries;      //׼�����͵���ˮ�����û���Ŷӻ�������m_pTradeSeries	
	CLinkManager* m_pLinkManager;

	CGateWayFront* m_pGatewayFront;
	CFlowDispatcher* m_pFlowDispatcher;

	CFtdFront* m_pKernelFront;
	//CHttpFront *m_pHttpFront;
	CXHttpFront *m_pHttpFront;
	CWSFront *m_pWsFront;
	//������ˮ�ļ���
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
	bool m_bFastRunMode;//�Ƿ��ǿ���ģʽ������ģʽû��������
	int m_nKernelid;//����ʱ��kernel���ĺ��룬�����0��˵��û�д�����,1����2����������ģʽ


	CCachedFlow* m_pTradeSeries;    //���Է��͵�������
	CFlow* m_pPreResult;			//�����
	CFlow* m_pTradeResult;			//���Է��͵Ľ����
	CPubFlow* m_pResultPubFlow;		//�������������ӿ�

	CXTPPackage m_pubPackage;
	CXTPPackage *m_pPackage;

	CFlowReader m_reader;
	volatile TCompStateID m_CompStateID;
	bool m_bStartAddin;
	int m_nCPUMode;//CPU������ģʽ��sleep��΢����Ŀ
	int m_nReliableMode;//�ɿ��Է�ʽ
	char pStreamBuf[OUTFOG_MAX_PACKAGE_SIZE];
	TOutFogMetaRef m_OutFogMetaRef;
	UF_INT8 m_nReDoID;//��ʼ���д�m_pTradeSeries���ĸ���ſ�ʼ
	UF_INT8 m_nCount;//Ŀǰ�����˵ڼ������
	int m_CurrPackageTime;//Ŀǰ�����ĵ�ʱ���
	int m_nFlowCacheSize;//��ˮ�ڴ�Ĵ�С
	CKernelAppCallback* m_pPackageCallback;

	double m_nLatencySums; //��¼��ʱ���ݵĺ�
	int m_nLatencyTimes;//��¼��ʱ���ݵĴ���

	bool m_bRuning;//�Ƿ��ڹ���
};