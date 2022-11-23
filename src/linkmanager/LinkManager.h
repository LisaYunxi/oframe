//////////////////////////////////////////////////////////////////////////
// 文件: LinkManager.h
// 功能: 定义了连接管理器
// 历史: 20180525 徐忠华
//////////////////////////////////////////////////////////////////////////

#ifndef _EXCHANGE_LINK_API_MANAGER_H_
#define _EXCHANGE_LINK_API_MANAGER_H_

#include "CachedFlow.h"
#include "OneTimeFlow.h"
#include "platform.h"
#include "Config.h"
#include "XTPPackage.h"
#include "FlowReader.h"
#include "monitorIndex.h"
#include "OutFogService.h"
#include "XtpClient.h"

#define INI_FILE_NAME "omq.ini"

#define LINK_KERNEL_GATEWAY		-7			//Kernel的级联线程GateWay使用
#define	LINK_KERNEL_TIMER		-8          //Kernel定时器使用
#define LINK_MANAGER_RSP		-9			//Link管理的回报使用

#define	LINK_KERNEL_FTDFRONT    -10          //Kernel的前置FtdFront使用
#define	LINK_KERNEL_HTTPFRONT   -20          //Kernel的前置HttpFront使用
#define	LINK_KERNEL_WSFRONT     -40          //Kernel的前置WsFront使用

#define	LINK_KERNEL_SHMFRONT    -50          //Kernel的前置ShmFront使用
#define	LINK_KERNEL_MULTIFRONT  -60          //Kernel的前置MultiFront使用
#define	LINK_KERNEL_FIXFRONT    -70          //Kernel的前置FixFront使用

#define	LINK_INNER_BEGIN		-100         //Link使用的Link序号开始




#define HTTP_ERROR_Success_ID			0
#define HTTP_ERROR_Url_ID				-1
#define HTTP_ERROR_Speed_ID				-2
#define HTTP_ERROR_Message_ID			-3
#define HTTP_ERROR_Format_ID			-6
#define HTTP_ERROR_BatchLimit_ID		-7
#define HTTP_ERROR_NoRsp				-8
#define HTTP_ERROR_Token_ID				-10

#define WS_ERROR_SubTopic_ID			-8

struct SHttpSession
{
	string AccessKey;
	string SecretKey;
	string UserID;
	UF_INT8 ExpireTime;
	set<string> IPAddress;
	int MaxSpeed;//设置的最大速度
	int HitTimes;//使用了多少次，用于速度限制
	int HitBeginTime;//统计次数的开始时间
};

typedef map<string, SHttpSession> CHttpSessionMap;

extern CHttpSessionMap g_SessionMap;
extern CRWLock g_SessionMapRWLock;
extern int g_nMaxSpeed;
extern int g_nMaxSubTopics;
extern int g_nMaxSendPackages;
extern int g_nMaxSessions;
extern int g_nMaxPackages;
extern bool g_bLowLatency;
extern int g_nWSHeartBeat;

//////////////////////////////////////////////////
// 报盘管理线程
//////////////////////////////////////////////////

class CLinkItem
{
public:
	COutFogServiceApi* m_pLinkApi;			//对应的Link
	int m_nLinkID;					//对应的Link的席位信息
	CFlow *m_pSeriesFlow;			//对应的Link的下行订单成交等实时一致性流水
	CFlow *m_pRspFlow;				//对应的Link的Rsp的临时数据
	CFlowReader* m_pSeriesReader;	//SeriesFlow的读取
	CFlowReader* m_pRspReader;		//RspFlow的读取
	CXtpClient *m_pSendClient;		//OfferLink使用package时专用
	bool m_bBeginSeries;			//启动pSeries读取
};

typedef map<int,CLinkItem*> CLinkItemMap;

//报盘管理的回报使用
class CLinkManagerRsp: public COutFogServiceApi
{
public:
	CLinkManagerRsp():COutFogServiceApi()
	{
		return;
	}
	virtual ~CLinkManagerRsp(){};

	virtual bool ReqField(const TOutFogMetaRef* pOutFogReqRef,const void *pField)
	{
		return true;
	}
	virtual bool SyncReqField(const TOutFogMetaRef* pOutFogReqRef,const void *pReqStream,TOutFogMetaRef* pOutFogRspRef,void *pRspStream)
	{
		return true;
	}

// 	void OnRspMessage(unsigned int nLinkID,unsigned int nTid,CFieldDescribe* pDescribe, void* pField)
// 	{
// 		//m_pServiceCallback->HandleLinkField(nLinkID,nTid,pDescribe,pField);
// 	}

	virtual const char* getVersion()
	{
		return "CManagerRsp";
	};

	virtual bool Start()
	{
		return true;
	};
};

class CLinkManager: public CLinkItemMap,public COutFogServiceCallback
{
public:
	CLinkManager();
	virtual ~CLinkManager();
	
	virtual bool RspField(const TOutFogMetaRef* pOutFogRspRef,const void *pStream);
	virtual bool ReqField(const TOutFogMetaRef* pOutFogReqRef,const void *pStream);
	virtual bool ReqField(CXTPPackage *pPackage);
	bool GetNextMessage(CXTPPackage *pPackage);

	bool GetLinkMessage(CLinkItem *pLinkItem,CXTPPackage *pPackage);

	//从一个席位信息初始化一个报盘链接
	COutFogServiceApi* initLink(const char* sLinkDll,int nLinkID);

	//初始化OfferLink
	COutFogServiceApi* initOfferLink(int nLinkID, const char * ipAddress, int nSubjectNo, UF_INT8 nSequenceNo);

	//插入一个Link
	CLinkItem* setLinkApi(int LinkID,COutFogServiceApi* pLinkApi,CFlow *pFlow,CFlow *pRspFlow);

	//使用api的编号找到通道
	COutFogServiceApi* getLinkApi(int nLinkID);
	CLinkItem* getLinkItem(int nLinkID);
	bool setLinkInitOver(int nLinkID);	

	void startWork();
	void OnTimer();
	bool RspError(int nLinkID);

private:
//	CXTPPackage	*m_pPubPackage;				//写流用的包

	CLinkItemMap::iterator m_iCurrLinkItem;	//目前遍历到的ListItem的位置
	CLinkItem *m_pCurrLinkItem;				//目前遍历到的LinkItem
	int	m_nCurrCount;						//目前LinkItem访问的次数

	CLinkManagerRsp *m_pLinkManagerRsp;		//错误的Link的回报处理

	COutFogService *m_pServiceLoad;
	int m_nFlowCacheSize;					//流水内存的大小
};

#endif