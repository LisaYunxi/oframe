#include "public.h"
#include "AggrProtocol.h"

CSequenceNoCounter::CSequenceNoCounter()
{
	m_dwRecvSeqno = 0;
	m_dwSendSeqNo = 0;
}

CSequenceNoCounter::~CSequenceNoCounter()
{
}

UF_INT8 CSequenceNoCounter::GetNextSendSeqNo()
{
	return ++m_dwSendSeqNo;
}

bool CSequenceNoCounter::UpdRecvSeqNo(UF_INT8 dwRecvNo)
{
	if (dwRecvNo != m_dwRecvSeqno+1)
	{
		return false;
	}
	m_dwRecvSeqno = dwRecvNo;
	return true;
}



CAggrProtocol::CAggrProtocol(CReactor *pReactor)
:CProtocol(pReactor, AGGRHLEN, new CAggrPackage(), new CAggrPackage())
{
	CAggrPackage *pAggrPackage = (CAggrPackage *)m_pSendPackage;
	pAggrPackage->SetType(AGGR_TYPE_CARRY);
	pAggrPackage->SetAggregationID(0);	
	pAggrPackage->SetSequenceNo(0);	
}

CAggrProtocol::~CAggrProtocol()
{

}

void CAggrProtocol::AggrLogin(unsigned int dwAggregationID)
{
	CAggrPackage pkgAggr;
	pkgAggr.ConstructAllocate(AGGR_PACKAGE_MAX_SIZE, 1000);
	pkgAggr.SetAggregationID(dwAggregationID);
	pkgAggr.SetType(AGGR_TYPE_LOGIN);
	pkgAggr.SetSequenceNo(0);
	
	CProtocol::Send(&pkgAggr);
}

CSequenceNoCounter *CAggrProtocol::GetSequenceNoCounter(unsigned int dwAggregationID)
{
	CSequenceNoCounter counter;

	pair<CAggrSequenceNoMap::iterator, bool> p = m_mapSeqNoCounter.insert(
		CAggrSequenceNoMap::value_type(dwAggregationID, counter));
	
	return &((*(p.first)).second);
}


int CAggrProtocol::MakePackage(CPackage *pPackage, CProtocol *pUpper)
{
	if (pUpper != NULL)
	{
		CAggrPackage *pAggrPackage = (CAggrPackage *)pPackage;
		unsigned int dwAggregationID = pUpper->GetActiveID();
		pAggrPackage->SetAggregationID(dwAggregationID);
		CSequenceNoCounter * pCounter = GetSequenceNoCounter(dwAggregationID);
		pAggrPackage->SetSequenceNo(pCounter->GetNextSendSeqNo());	
	}
	
	return CProtocol::MakePackage(pPackage, pUpper);
}

int CAggrProtocol::Pop(CPackage *pPackage)
{
	CAggrPackage *pAggrPackage = (CAggrPackage *)pPackage;
	if(pAggrPackage->GetType() == AGGR_TYPE_CARRY)
	{
		unsigned int dwAggregationID = pPackage->GetActiveID();
		CSequenceNoCounter * pCounter = GetSequenceNoCounter(dwAggregationID);
		UF_INT8 dwSeqNo = pAggrPackage->GetSequenceNo();
		if (!pCounter->UpdRecvSeqNo(dwSeqNo))
		{
//			NET_STATUS_LOG1("ignore pAggrPackage, seq=%d\n", dwSeqNo);
			return 0;
		}
	}
	return CProtocol::Pop(pPackage);
}

