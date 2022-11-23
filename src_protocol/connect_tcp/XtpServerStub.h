#ifndef __XtpServerStub_H__
#define __XtpServerStub_H__

#include "SessionFactory.h"
#include "TcpXtpSession.h"
#include "Reactor.h"
#include "CachedFlow.h"
#include "XtpServer.h"

class CXtpServerStub : public CSessionFactory,public CTcpXtpSessionCallback,public CXtpServer
{
public:
	CXtpServerStub(CReactor *pReactor, CFlow *pSubFlow, int nThreads = 1);
	~CXtpServerStub();
	virtual int HandlePackage(CXTPPackage *pPackage, CTcpXtpSession *pSession);
	virtual int OnReqClientLogin(CXTPPackage *pPackage, CTcpXtpSession *pSession);
	virtual void Init(const char *tmpAddress);
	virtual void EnableListener(bool bEnable);
	virtual void SetSubjectNo(unsigned int nSubjectNo);
	virtual void setXtpServerCallback(CXtpServerCallback *XtpServerCallback);

	virtual void RegisterFlow(CReadFlow *pFlow, unsigned int nSubjectNo);
	CReadFlow *GetFlow(unsigned int nSubjectNo);

protected:
	virtual CSession *CreateSession(CChannel *pChannel, unsigned int bIsListener);
	virtual void OnSessionConnected(CSession *pSession);
	virtual void OnSessionDisconnected(CSession *pSession, int nReason);
	
private:
	unsigned int m_nSubjectNo;
	CFlow *m_pSubFlow;
	//CXTPPackage m_XtpPackage;
	CXtpServerCallback *m_pServerCallBack;
	//下行流的集合
	typedef CHashMap<unsigned int, CReadFlow *, HashInt> CFlowMap;
	CFlowMap m_mapFlow;
	vector<CReactor*> m_Reactors; //发送线程
	int m_nSessionCount;
};

#endif