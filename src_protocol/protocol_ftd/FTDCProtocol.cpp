#include "public.h"
#include "FTDCProtocol.h"
#include "FTDCPackage.h"

CFTDCProtocol::CFTDCProtocol(CReactor *pReactor) :CProtocol(pReactor, FTDCHLEN, new CFTDCPackage(), new CFTDCPackage())
{
}

CFTDCProtocol::~CFTDCProtocol()
{
	Clear();
}

void CFTDCProtocol::Clear()
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

int CFTDCProtocol::Pop(CPackage *pPackage)
{
	CFTDCPackage *pFTDCPackage = (CFTDCPackage *)pPackage;
	CFTDCSubEndPoint *pSubEndPoint = GetSubEndPoint(pFTDCPackage->GetFTDCHeader()->SubjectNo);
	if (pSubEndPoint != NULL)
		return pSubEndPoint->HandlePackage(pFTDCPackage);

	return CProtocol::Pop(pPackage);
}

void CFTDCProtocol::PublishSend()
{
	CPubEndPointMap::iterator pitor = m_mapPubEndPoint.Begin();
	while (!pitor.IsEnd())
	{
		(*pitor)->PublishSend();
		pitor++;
	}
}

int CFTDCProtocol::OnRecvErrorPackage(CPackage *pPackage)
{
	NotifyError(MSG_FTDCERR_BADPACKAGE, 0, this);
	return 0;
}

void CFTDCProtocol::Publish(CReadFlow *pFlow, unsigned short nSequenceSeries, UF_INT8 nStartId, UF_INT8 nBusinessNo, int nReaderTimes)
{
	//	DEBUG_LOG2("nSequenceSeries=[%d], nStartId=[%0.f]\n",nSequenceSeries, nStartId);
	CFTDCPubEndPoint *pPubEndPoint = GetPubEndPoint(nSequenceSeries);
	if (pPubEndPoint == NULL)
	{
		pPubEndPoint = new CFTDCPubEndPoint(pFlow, nSequenceSeries, nStartId, this);
		m_mapPubEndPoint.Insert(nSequenceSeries, pPubEndPoint);
	}
	pPubEndPoint->MoveTo(nStartId);
}

void CFTDCProtocol::UnPublish(unsigned short nSequenceSeries)
{
	CFTDCPubEndPoint *pPubEndPoint = GetPubEndPoint(nSequenceSeries);
	if (pPubEndPoint != NULL)
	{
		delete pPubEndPoint;
		m_mapPubEndPoint.Erase(nSequenceSeries);
	}
}

void CFTDCProtocol::UnPublishAll()
{
	CPubEndPointMap::iterator pitor = m_mapPubEndPoint.Begin();
	while (!pitor.IsEnd())
	{
		delete (*pitor);
		pitor++;
	}
	m_mapPubEndPoint.Clear();
}

void CFTDCProtocol::RegisterSubscriber(CFTDCSubscriber *pSubscriber)
{
	unsigned short nSequenceSeries = pSubscriber->GetSequenceSeries();
	CFTDCSubEndPoint *pSubEndPoint = GetSubEndPoint(nSequenceSeries);
	if (pSubEndPoint == NULL)
	{
		pSubEndPoint = new CFTDCSubEndPoint(pSubscriber);
		m_mapSubEndPoint.Insert(nSequenceSeries, pSubEndPoint);
	}
}

void CFTDCProtocol::UnRegisterSubscriber(CFTDCSubscriber *pSubscriber)
{
	unsigned short nSequenceSeries = pSubscriber->GetSequenceSeries();
	CFTDCSubEndPoint *pSubEndPoint = GetSubEndPoint(nSequenceSeries);
	if (pSubEndPoint != NULL)
	{
		delete pSubEndPoint;
		m_mapSubEndPoint.Erase(nSequenceSeries);
	}
}

CFTDCSubEndPoint *CFTDCProtocol::GetSubEndPoint(unsigned short nSequenceSeries)
{
	CFTDCSubEndPoint **pFind = m_mapSubEndPoint.Find(nSequenceSeries);
	if (pFind != NULL)
		return (*pFind);
	return NULL;
}

CFTDCPubEndPoint *CFTDCProtocol::GetPubEndPoint(unsigned short nSequenceSeries)
{
	CFTDCPubEndPoint **pFind = m_mapPubEndPoint.Find(nSequenceSeries);
	if (pFind != NULL)
		return (*pFind);
	return NULL;
}
