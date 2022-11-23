#include "public.h"
#include "XTPProtocol.h"
#include "XTPPackage.h"

const int XTP_TIMER_DRIVE = 0;

CXTPProtocol::CXTPProtocol(CReactor *pReactor, CXTPPubHelper *pPubHelper, CXTPSubHelper *pSubHelper)
	:CProtocol(pReactor, XTPHLEN, new CXTPPackage(), new CXTPPackage())
{
	m_pSubHelper = pSubHelper;
	m_pPubHelper = pPubHelper;
	SetTimer(XTP_TIMER_DRIVE, 5);
	//SetTimer(XTP_TIMER_DRIVE, 10);
	m_pReactor->RegisterIO(this);
}

CXTPProtocol::~CXTPProtocol()
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

int CXTPProtocol::Pop(CPackage *pPackage)
{

	unsigned int nCurrClock = m_pReactor->GetClock();
	CXTPPackage *pXTPPackage = (CXTPPackage *)pPackage;
	CXTPSubEndPoint *pSubEndPoint = GetSubEndPoint(pXTPPackage->GetXTPHeader()->SubjectNo);
	if (pSubEndPoint != NULL)
	{
		return pSubEndPoint->HandlePackage(pXTPPackage, nCurrClock);
	}

	return CProtocol::Pop(pPackage);
}

void CXTPProtocol::OnTimer(int nIDEvent)
{
	if (nIDEvent == XTP_TIMER_DRIVE)
	{
		unsigned int nCurrClock = m_pReactor->GetClock();
		CSubEndPointMap::iterator sitor = m_mapSubEndPoint.Begin();
		while (!sitor.IsEnd())
		{
			(*sitor)->OnTimer(nCurrClock);
			sitor++;
		}

		CPubEndPointMap::iterator pitor = m_mapPubEndPoint.Begin();
		while (!pitor.IsEnd())
		{
			(*pitor)->OnTimer(nCurrClock, 0);
			pitor++;
		}
		//对没有本周期发送出去的消息，放弃事件触发，目的是更好的流量控制
		m_pReactor->SetSpeciaFlag(false);
	}
}

int CXTPProtocol::OnRecvErrorPackage(CPackage *pPackage)
{
	NotifyError(MSG_XTPERR_BADPACKAGE, 0, this);
	return 0;
}

void CXTPProtocol::OnHeartBeat(unsigned int nSubjectNo, UF_INT8 nSeqNo)
{
	unsigned int nCurrClock = m_pReactor->GetClock();
	CXTPSubEndPoint *pSubEndPoint = GetSubEndPoint(nSubjectNo);
	if (pSubEndPoint != NULL)
	{
		pSubEndPoint->OnHeartBeat(nSeqNo, nCurrClock);
	}
}

void CXTPProtocol::Publish(CReadFlow *pFlow, unsigned int nSubjectNo, unsigned int dwSubscriberID, UF_INT8 nStartId)
{
	//	DEBUG_LOG3("startid=[%d]\n", nStartId);

	CXTPPubEndPoint *pPubEndPoint = GetPubEndPoint(nSubjectNo);
	if (pPubEndPoint == NULL)
	{
		pPubEndPoint = new CXTPPubEndPoint(pFlow, nSubjectNo, this, m_pPubHelper);
		m_mapPubEndPoint.Insert(nSubjectNo, pPubEndPoint);
	}
	pPubEndPoint->OnNak(dwSubscriberID, nStartId, m_pReactor->GetClock());
}

void CXTPProtocol::UnPublish(unsigned int nSubjectNo)
{
	CXTPPubEndPoint *pPubEndPoint = GetPubEndPoint(nSubjectNo);
	if (pPubEndPoint != NULL)
	{
		delete pPubEndPoint;
		m_mapPubEndPoint.Erase(nSubjectNo);
	}
}

void CXTPProtocol::RegisterSubscriber(CSubscriber *pSubscriber, unsigned int dwSubscriberID)
{
	unsigned int nSubjectNo = pSubscriber->GetSubjectNo();
	CXTPSubEndPoint *pSubEndPoint = GetSubEndPoint(nSubjectNo);
	if (pSubEndPoint == NULL)
	{
		pSubEndPoint = new CXTPSubEndPoint(pSubscriber, dwSubscriberID, m_pSubHelper);
		m_mapSubEndPoint.Insert(nSubjectNo, pSubEndPoint);
	}
}

void CXTPProtocol::UnRegisterSubscriber(CSubscriber *pSubscriber)
{
	unsigned int nSubjectNo = pSubscriber->GetSubjectNo();
	CXTPSubEndPoint *pSubEndPoint = GetSubEndPoint(nSubjectNo);
	if (pSubEndPoint != NULL)
	{
		delete pSubEndPoint;
		m_mapSubEndPoint.Erase(nSubjectNo);
	}
}

CXTPSubEndPoint *CXTPProtocol::GetSubEndPoint(unsigned int nSubjectNo)
{
	CXTPSubEndPoint **pFind = m_mapSubEndPoint.Find(nSubjectNo);
	if (pFind != NULL)
	{
		return (*pFind);
	}
	return NULL;
}

CXTPPubEndPoint *CXTPProtocol::GetPubEndPoint(unsigned int nSubjectNo)
{
	CXTPPubEndPoint **pFind = m_mapPubEndPoint.Find(nSubjectNo);
	if (pFind != NULL)
	{
		return (*pFind);
	}
	return NULL;
}

void CXTPProtocol::OnAck(unsigned int dwSubscriberID, unsigned int dwSubjectNo, UF_INT8 nStartId)
{
	CXTPPubEndPoint *pPubEndPoint = GetPubEndPoint(dwSubjectNo);
	if (pPubEndPoint != NULL)
	{
		pPubEndPoint->OnAck(dwSubscriberID, nStartId, m_pReactor->GetClock());
	}
}
void CXTPProtocol::GetIds(int *pReadId, int *pWriteId)
{
	*pReadId = 0;
	*pWriteId = 0;

	if (m_pReactor->GetSpeciaFlag())
	{

		CPubEndPointMap::iterator pitor = m_mapPubEndPoint.Begin();
		while (!pitor.IsEnd())
		{
			(*pitor)->OnTimer(m_pReactor->GetClock(), 1);
			pitor++;
		}
		//关闭特殊事件标志
		//m_pReactor->SetSpeciaFlag(false);
	}
}
