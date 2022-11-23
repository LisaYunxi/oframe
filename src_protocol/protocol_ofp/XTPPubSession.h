#ifndef AFX_XTPPUBSESSION_H__
#define AFX_XTPPUBSESSION_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Flow.h"
#include "XTPSession.h"
#include "SCPProtocol.h"
#include "Subscriber.h"

class CXTPFlowManager  
{
public:
	virtual CReadFlow *GetFlow(unsigned int nSubjectNo) = 0;
	virtual ~CXTPFlowManager(){};
};

class CXTPPubSession : public CXTPSession
{
public:
	CXTPPubSession(CReactor *pReactor, CChannel *pChannel, CXTPFlowManager *pFlowManager);
	virtual ~CXTPPubSession();
	CSCPPackage *PrepareControlPackage(unsigned short stid);
	void SendControlPackage(CSCPPackage *pPackage);
	
	void RegisterSubscriber(CSubscriber *pSubscriber, unsigned int dwSubscriberID);
	void UnRegisterSubscriber(CSubscriber *pSubscriber);
	virtual void SendNak(unsigned int nSubjectNo, unsigned int dwSubscriberID, UF_INT8 nSeqNo);
	virtual void SendAck(unsigned int nSubjectNo, unsigned int dwSubscriberID, UF_INT8 nSeqNo);
	virtual void SendHeartBeat(unsigned int nSubjectNo, UF_INT8 nSeqNo);
protected:
	virtual int HandlePackage(CPackage *pPackage, CProtocol *pProtocol);
	void OnSubscribeReq(CSCPPackage *pSCPPackage);
	void OnUnSubscribeReq(CSCPPackage *pSCPPackage);
	void OnHeartBeat(CSCPPackage *pSCPPackage);
	void OnAck(CSCPPackage *pSCPPackage);
	void SendSubscribeReq(unsigned int nSubjectNo, unsigned int dwSubscriberID, UF_INT8 nStartID);
	void SendUnSubscribeReq(unsigned int nSubjectNo);
private:
	CXTPFlowManager *m_pFlowManager;
	CSCPProtocol *m_pControlProtocol;
	CSCPPackage m_controlPackage;
};
#endif
