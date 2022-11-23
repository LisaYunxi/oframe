//////////////////////////////////////////////////////////////////////////
// �ļ�: LinkManager.h
// ����: ���������ӹ�����
// ��ʷ: 20180525 ���һ�
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

#define LINK_KERNEL_GATEWAY		-7			//Kernel�ļ����߳�GateWayʹ��
#define	LINK_KERNEL_TIMER		-8          //Kernel��ʱ��ʹ��
#define LINK_MANAGER_RSP		-9			//Link����Ļر�ʹ��

#define	LINK_KERNEL_FTDFRONT    -10          //Kernel��ǰ��FtdFrontʹ��
#define	LINK_KERNEL_HTTPFRONT   -20          //Kernel��ǰ��HttpFrontʹ��
#define	LINK_KERNEL_WSFRONT     -40          //Kernel��ǰ��WsFrontʹ��

#define	LINK_KERNEL_SHMFRONT    -50          //Kernel��ǰ��ShmFrontʹ��
#define	LINK_KERNEL_MULTIFRONT  -60          //Kernel��ǰ��MultiFrontʹ��
#define	LINK_KERNEL_FIXFRONT    -70          //Kernel��ǰ��FixFrontʹ��

#define	LINK_INNER_BEGIN		-100         //Linkʹ�õ�Link��ſ�ʼ




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
	int MaxSpeed;//���õ�����ٶ�
	int HitTimes;//ʹ���˶��ٴΣ������ٶ�����
	int HitBeginTime;//ͳ�ƴ����Ŀ�ʼʱ��
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
// ���̹����߳�
//////////////////////////////////////////////////

class CLinkItem
{
public:
	COutFogServiceApi* m_pLinkApi;			//��Ӧ��Link
	int m_nLinkID;					//��Ӧ��Link��ϯλ��Ϣ
	CFlow *m_pSeriesFlow;			//��Ӧ��Link�����ж����ɽ���ʵʱһ������ˮ
	CFlow *m_pRspFlow;				//��Ӧ��Link��Rsp����ʱ����
	CFlowReader* m_pSeriesReader;	//SeriesFlow�Ķ�ȡ
	CFlowReader* m_pRspReader;		//RspFlow�Ķ�ȡ
	CXtpClient *m_pSendClient;		//OfferLinkʹ��packageʱר��
	bool m_bBeginSeries;			//����pSeries��ȡ
};

typedef map<int,CLinkItem*> CLinkItemMap;

//���̹���Ļر�ʹ��
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

	//��һ��ϯλ��Ϣ��ʼ��һ����������
	COutFogServiceApi* initLink(const char* sLinkDll,int nLinkID);

	//��ʼ��OfferLink
	COutFogServiceApi* initOfferLink(int nLinkID, const char * ipAddress, int nSubjectNo, UF_INT8 nSequenceNo);

	//����һ��Link
	CLinkItem* setLinkApi(int LinkID,COutFogServiceApi* pLinkApi,CFlow *pFlow,CFlow *pRspFlow);

	//ʹ��api�ı���ҵ�ͨ��
	COutFogServiceApi* getLinkApi(int nLinkID);
	CLinkItem* getLinkItem(int nLinkID);
	bool setLinkInitOver(int nLinkID);	

	void startWork();
	void OnTimer();
	bool RspError(int nLinkID);

private:
//	CXTPPackage	*m_pPubPackage;				//д���õİ�

	CLinkItemMap::iterator m_iCurrLinkItem;	//Ŀǰ��������ListItem��λ��
	CLinkItem *m_pCurrLinkItem;				//Ŀǰ��������LinkItem
	int	m_nCurrCount;						//ĿǰLinkItem���ʵĴ���

	CLinkManagerRsp *m_pLinkManagerRsp;		//�����Link�Ļر�����

	COutFogService *m_pServiceLoad;
	int m_nFlowCacheSize;					//��ˮ�ڴ�Ĵ�С
};

#endif