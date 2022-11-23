#ifndef __XtpClient_H__
#define __XtpClient_H__

#include "Reactor.h"
#include "XTPPackage.h"
#include "Flow.h"

#if defined(XTPCLIENTAPI_IS_LIB) && defined(WIN32)
#ifdef LIB_XTPCLIENT_API_EXPORT
#define XTPCLIENT_API_EXPORT __declspec(dllexport)
#else
#define XTPCLIENT_API_EXPORT __declspec(dllimport)
#endif
#else
#define XTPCLIENT_API_EXPORT 
#endif

#define XtpLinkFlowGateWay 1	//ǰ���ڲ�ʹ�õļ���GateWay����ˮ
#define XtpLinkFlowInner   2	//�����ڲ���ˮ
#define XtpLinkFlowOthers  3	//���̻���������ˮ

class CXtpClientCallback
{
public:
	~CXtpClientCallback(){};
	virtual int HandlePackage(CXTPPackage *pXTPPackage)=0;
	virtual void OnSessionConnected(unsigned int nSessionID)=0;
	virtual void OnSessionDisconnected(unsigned int nSessionID, int nReason)=0;
};

class XTPCLIENT_API_EXPORT CXtpClient
{
public:
	~CXtpClient(){};
	static CXtpClient* CreateClientApi(CReactor *pReactor, CFlow *pSerailFlow, int nSubjectNo, UF_INT8 nSequenceNo);
	virtual void RegisterReportServer(const char *pszServerAddress)=0;
	virtual void Init()=0;
	virtual void RegClientCallback(CXtpClientCallback *pClientCallback)=0;
	virtual int RequestToDialogFlow(CXTPPackage *pXTPPackage)=0;
};
#endif