#ifndef AFX_XTPSUBENDPOINT_H__
#define AFX_XTPSUBENDPOINT_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Protocol.h"
#include "Subscriber.h"
#include "OrderingQ.h"

class CXTPSubHelper
{
public:
	virtual void SendNak(unsigned int nSubjectNo, unsigned int dwSubscriberID, UF_INT8 nSeqNo) = 0;
	virtual void SendAck(unsigned int nSubjectNo, unsigned int dwSubscriberID, UF_INT8 nSeqNo) = 0;
	virtual ~CXTPSubHelper(){};
};

class CXTPSubEndPoint
{
public:
	CXTPSubEndPoint(CSubscriber *pSubscriber, unsigned int dwSubscriberID, CXTPSubHelper *pControlHelper);
	virtual ~CXTPSubEndPoint();

	int HandlePackage(CXTPPackage *pXTPPackage, unsigned int nCurrClock);
	void OnTimer(unsigned int nCurrClock);
	void OnHeartBeat(UF_INT8 nSeqNo, unsigned int nCurrClock);
	inline unsigned int GetSubjectNo();
	inline void SetAvailable();
private:
	int Delivery(CXTPPackage *pXTPPackage);
	
private:
	CSubscriber *m_pSubscriber;
	CXTPSubHelper *m_pControlHelper;
	COrderingQ m_orderingQ;
	unsigned int m_nDisorderClock;
	unsigned int m_nDeliveryClock;
	unsigned int m_nNakInterval;
	unsigned int m_dwSubscriberID;
	unsigned int m_dwLastAckClock;
//	unsigned int m_dwGroupID;
	unsigned short m_available;		//标志CXTPSubEndPoint是否可用
};

inline unsigned int CXTPSubEndPoint::GetSubjectNo()
{
	return m_pSubscriber->GetSubjectNo();
}

inline void CXTPSubEndPoint::SetAvailable()
{
	m_available = 0;
	return ;
}

#endif
