// FIBProtocl.cpp: implementation of the CFIBProtocl class.
//
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "FIBProtocol.h"
#include "XTPPackage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFIBProtocol::CFIBProtocol(CReactor *pReactor)
:CProtocol(pReactor, XTPHLEN, new CXTPPackage(), new CXTPPackage())
{
}

CFIBProtocol::~CFIBProtocol()
{
	Clear();
}

void CFIBProtocol::Clear()
{
	CSubEndPointMap::iterator sitor = m_mapSubEndPoint.Begin();
	while (!sitor.IsEnd())
	{
		delete (*sitor);
		sitor++;
	}
	m_mapSubEndPoint.Clear();

	CPubEndPointMap::iterator pitor = m_mapPubEndPoint.Begin();
	while (!pitor.IsEnd())
	{
		delete (*pitor);
		pitor++;
	}
	m_mapPubEndPoint.Clear();
}

int CFIBProtocol::Pop(CPackage *pPackage)
{
	CXTPPackage *pXTPPackage = (CXTPPackage *)pPackage;
	CFIBSubEndPoint *pSubEndPoint = GetSubEndPoint(pXTPPackage->GetXTPHeader()->SubjectNo);
	if (pSubEndPoint != NULL)
	{
		return pSubEndPoint->HandlePackage(pXTPPackage);
	}

	return CProtocol::Pop(pPackage);
}

void CFIBProtocol::PublishSend()
{
	CPubEndPointMap::iterator pitor = m_mapPubEndPoint.Begin();
	while (!pitor.IsEnd())
	{
		(*pitor)->PublishSend();
		pitor++;
	}
}

int CFIBProtocol::OnRecvErrorPackage(CPackage *pPackage)
{
	NotifyError(MSG_FIBERR_BADPACKAGE, 0, this);
	return 0;
}

void CFIBProtocol::Publish(CReadOnlyFlow *pFlow, WORD nSequenceSeries, int nStartId)
{
//	DEBUG_LOG2("nSequenceSeries=[%d], nStartId=[%d]\n",
//		nSequenceSeries, nStartId);
	CFIBPubEndPoint *pPubEndPoint = GetPubEndPoint(nSequenceSeries);
	if (pPubEndPoint == NULL)
	{
		pPubEndPoint = new CFIBPubEndPoint(pFlow, nSequenceSeries, nStartId, this);
		m_mapPubEndPoint.Insert(nSequenceSeries, pPubEndPoint);
	}
	pPubEndPoint->MoveTo(nStartId);
}

void CFIBProtocol::UnPublish(WORD nSequenceSeries)
{
	CFIBPubEndPoint *pPubEndPoint = GetPubEndPoint(nSequenceSeries);
	if (pPubEndPoint != NULL)
	{
		delete pPubEndPoint;
		m_mapPubEndPoint.Erase(nSequenceSeries);
	}
}

void CFIBProtocol::RegisterSubscriber(CFIBSubscriber *pSubscriber)
{
	WORD nSequenceSeries = pSubscriber->GetSequenceSeries();
	CFIBSubEndPoint *pSubEndPoint = GetSubEndPoint(nSequenceSeries);
	if (pSubEndPoint == NULL)
	{
		pSubEndPoint = new CFIBSubEndPoint(pSubscriber);
		m_mapSubEndPoint.Insert(nSequenceSeries, pSubEndPoint);
	}
}

void CFIBProtocol::UnRegisterSubscriber(CFIBSubscriber *pSubscriber)
{
	WORD nSequenceSeries = pSubscriber->GetSequenceSeries();
	CFIBSubEndPoint *pSubEndPoint = GetSubEndPoint(nSequenceSeries);
	if (pSubEndPoint != NULL)
	{
		delete pSubEndPoint;
		m_mapSubEndPoint.Erase(nSequenceSeries);
	}
}

CFIBSubEndPoint *CFIBProtocol::GetSubEndPoint(WORD nSequenceSeries)
{
	CFIBSubEndPoint **pFind = m_mapSubEndPoint.Find(nSequenceSeries);
	if (pFind != NULL)
	{
		return (*pFind);
	}
	return NULL;
}

CFIBPubEndPoint *CFIBProtocol::GetPubEndPoint(WORD nSequenceSeries)
{
	CFIBPubEndPoint **pFind = m_mapPubEndPoint.Find(nSequenceSeries);
	if (pFind != NULL)
	{
		return (*pFind);
	}
	return NULL;
}

void CFIBProtocol::SetCommPhaseNo(DWORD wCommPhaseNo)
{
	CSubEndPointMap::iterator sitor = m_mapSubEndPoint.Begin();
	while (!sitor.IsEnd())
	{
		(*sitor)->SetCommPhaseNo(wCommPhaseNo);
		sitor++;
	}
}

