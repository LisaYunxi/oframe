// SessionAggregation.h: interface for the CSessionAggregation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SESSIONAGGREGATION_H__EDCB24A5_6574_400B_8EF5_2CF43F8FF0B2__INCLUDED_)
#define AFX_SESSIONAGGREGATION_H__EDCB24A5_6574_400B_8EF5_2CF43F8FF0B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "AggrSession.h"

#define MSG_AGGRERR_DISCONNECT		0xB001

class CSessionAggregation;

class CSessionAggrCallback
{
public:
	virtual void OnSessionAggrLogin(CSessionAggregation *pSessionAggr) = 0;
	virtual void OnSessionAggrDisconnected(CSessionAggregation *pSessionAggr, int nReason) = 0;
	virtual void OnSessionAggrWarning(CSessionAggregation *pSessionAggr, int nReason, int nParam) = 0;
};

class CSessionAggregation : public CEventHandler, public CProtocolCallback
{
public:
	CSessionAggregation(CReactor *pReactor, unsigned int dwAggregationID);
	virtual ~CSessionAggregation();
	unsigned int GetAggregationID();
	void Disconnect(int nErrorCode);
	bool AddSession(CAggrSession *pAggrSession, bool bLogin);
	bool RemoveSession(CAggrSession *pAggrSession);
	inline void RegisterAggrCallback(CSessionAggrCallback *pCallback);
	void SetHeartbeatTimeout(unsigned int dwReadTimeout);
	virtual int HandlePackage(CPackage *pPackage, CProtocol *pProtocol);
protected:
	unsigned int m_dwAggregationID;
	typedef  map<unsigned int, CAggrSession *> CAggrSessionMap;
	CAggrSessionMap m_mapSession;
	CSessionAggrCallback *m_pCallback;
	CAggrProtocol *m_pAggrProtocol;
	unsigned int m_dwReadTimeout;
};

inline void CSessionAggregation::RegisterAggrCallback(CSessionAggrCallback *pCallback)
{
	m_pCallback = pCallback;
}

#endif // !defined(AFX_SESSIONAGGREGATION_H__EDCB24A5_6574_400B_8EF5_2CF43F8FF0B2__INCLUDED_)
