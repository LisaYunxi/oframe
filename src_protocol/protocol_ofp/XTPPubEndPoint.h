// XTPPubEndPoint.h: interface for the CXTPPubEndPoint class.
//
// 2006.10.20 xuzh 增加发送状态日志
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPPUBENDPOINT_H__93E840F9_7AEE_406D_A53E_283C938D4713__INCLUDED_)
#define AFX_XTPPUBENDPOINT_H__93E840F9_7AEE_406D_A53E_283C938D4713__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Protocol.h"
#include "PublishPort.h"
#include "HashMap.h"

class CXTPPubHelper
{
public:
	virtual void SendHeartBeat(unsigned int nSubjectNo, UF_INT8 nSeqNo){};
	virtual ~CXTPPubHelper(){};
};

class CXTPPubEndPoint 
{
public:
	CXTPPubEndPoint(CReadFlow *pFlow, unsigned int nSubjectNo,	CProtocol *pSendProtocol, CXTPPubHelper *pControlHelper);
	virtual ~CXTPPubEndPoint();
	void Publish(unsigned int nCurrClock,int DriveFlag);
	void OnNak(unsigned int dwSubscriberID, UF_INT8 nStartID, unsigned int dwNakTime);
	void OnAck(unsigned int dwSubscriberID, UF_INT8 nStartID, unsigned int dwAckTime);
	void OnTimer(unsigned int nCurrClock,int DriveFlag=0);
	inline unsigned int GetSubjectNo();
private:
	void CreatePublishPort(unsigned int dwSubscriberID, UF_INT8 nStartID, unsigned int dwNakTime);
	void RemovePublishPort(CPublishPort *pPublishPort);
private:
	unsigned int m_nSubjectNo;
	vector<CPublishPort *> m_PublishPorts;
	CReadFlow *m_pFlow;
	unsigned int m_nSendHeartbeatClock;
	unsigned int m_nLogClock;
	unsigned int m_nLastSendClock;
	CProtocol *m_pSendProtocol; 
	CXTPPubHelper *m_pControlHelper;
	CXTPPackage m_SendPackage;
	unsigned int	m_nLastRandClock;
};

inline unsigned int CXTPPubEndPoint::GetSubjectNo()
{
	return m_nSubjectNo;
}

#endif // !defined(AFX_XTPPUBENDPOINT_H__93E840F9_7AEE_406D_A53E_283C938D4713__INCLUDED_)
