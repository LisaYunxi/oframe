#ifndef __XtpClientStub_H__
#define __XtpClientStub_H__

#include "public.h"
#include "SessionFactory.h"
#include "SelectReactor.h"
#include "TcpXtpSession.h"
#include "Flow.h"
#include "PipeEventHandler.h"
#include "XtpClient.h"

#define USER_CHANNEL_DISCONNECTED_ALL 0x1199

class CXtpClientStub:public CSessionFactory,public CTcpXtpSessionCallback,public CXtpClient
{
public:
	CXtpClientStub(CReactor *pReactor, CFlow *pSerailFlow, int nSubjectNo, UF_INT8 nSequenceNo);
	~CXtpClientStub();
	virtual void RegisterReportServer(const char *pszServerAddress);
	virtual void Init();
	virtual void StartConnect();
	virtual void StopConnect();
	virtual int HandlePackage(CXTPPackage *pXTPPackage, CTcpXtpSession *pSession);
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);
	virtual void RegClientCallback(CXtpClientCallback *pClientCallback);

	void InterLogin(CTcpXtpSession *pSession);

	virtual int SendReportPackage(CXTPPackage *pXTPPackage);
	virtual int RequestToDialogFlow(CXTPPackage *pXTPPackage);

protected:
	virtual CSession *CreateSession(CChannel *pChannel, unsigned int dwMark);
	virtual void OnSessionConnected(CSession *pSession);
	virtual void OnSessionDisconnected(CSession *pSession, int nReason);
	void CreateDialogFlow();
	void RemoveDialogFlow();

	CXTPPackage m_XtpPackage;
	char *m_pzLogPath;
	bool m_bConnected;
	CXtpClientCallback *m_pClientCallBack;

public:
	
	CFlow *m_pSerialFlow;
	//记录收到了多少个包,由于可能m_pSerialFlow会有多个同时写，此时需要自己记录接受的报文
	//在只接受一个流文件的时候，m_nSubCount不管用
	//如果上层初始化m_nSubCount==-1的时候,使用流大小初始化，认为只有一个流文件
	UF_INT8 m_nSubCount;
	//订阅的主题号
	int m_nSubjectNo;
	UF_INT8 m_nSequenceNo;
	CFlow *m_pDialogReqFlow;
	CPipeEventHandler *m_pPipeEventHandler;
};
#endif

