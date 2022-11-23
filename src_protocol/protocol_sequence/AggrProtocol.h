#ifndef AFX_AGGRPROTOCOL_H__
#define AFX_AGGRPROTOCOL_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "Protocol.h"
#include "AggrPackage.h"

class CSequenceNoCounter
{
public:
	CSequenceNoCounter();
	~CSequenceNoCounter();
	UF_INT8 GetNextSendSeqNo();
	bool UpdRecvSeqNo(UF_INT8 dwRecvNo);
private:
	UF_INT8 m_dwSendSeqNo;
	UF_INT8 m_dwRecvSeqno;
};

class CAggrProtocol : public CProtocol  
{
public:
	CAggrProtocol(CReactor *pReactor);
	virtual ~CAggrProtocol();
	void AggrLogin(unsigned int dwAggregationID);
protected:
	virtual int MakePackage(CPackage *pPackage, CProtocol *pUpper);
	virtual int Pop(CPackage *pPackage);
	CSequenceNoCounter *GetSequenceNoCounter(unsigned int dwAggregationID);
protected:
	typedef map<unsigned int, CSequenceNoCounter> CAggrSequenceNoMap;
	CAggrSequenceNoMap m_mapSeqNoCounter;
};

#endif // !defined(AFX_AGGRPROTOCOL_H__21D58A29_0774_4DA1_ACAC_07BEC3CC5AC9__INCLUDED_)
