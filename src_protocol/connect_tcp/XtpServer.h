#ifndef __XtpServer_H__
#define __XtpServer_H__

#include "Reactor.h"
#include "XTPPackage.h"
#include "Flow.h"

#if defined(XTPSERVERAPI_IS_LIB) && defined(WIN32)
#ifdef LIB_XTPSERVER_API_EXPORT
#define XTPSERVER_API_EXPORT __declspec(dllexport)
#else
#define XTPSERVER_API_EXPORT __declspec(dllimport)
#endif
#else
#define XTPSERVER_API_EXPORT 
#endif

#define XtpLinkFlowInput	1	//接收到的服务器的输入流水
#define XtpLinkFlowOutput   2	//接收到的服务器的输出流水


class CXtpServerCallback
{
public:
	virtual int HandlePackage(CXTPPackage *pPackage) = 0;
	virtual void OnSessionConnected(unsigned int nSessionID) = 0;
	virtual void OnSessionDisconnected(unsigned int nSessionID, int nReason) = 0;
	virtual ~CXtpServerCallback() {};
};

class XTPSERVER_API_EXPORT CXtpServer
{
public:
	~CXtpServer() {};
	static CXtpServer* CreateServerApi(CReactor *pReactor, CFlow *pSubFlow, int nThreads = 1);
	virtual void Init(const char *tmpAddress) = 0;
	virtual void EnableListener(bool bEnable) = 0;
	virtual void setXtpServerCallback(CXtpServerCallback *XtpServerCallback) = 0;
	virtual void RegisterFlow(CReadFlow *pFlow, unsigned int nSubjectNo) = 0;
};

#endif
