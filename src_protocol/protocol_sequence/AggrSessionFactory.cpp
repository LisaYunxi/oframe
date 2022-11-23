#include "public.h"
#include "AggrSessionFactory.h"

CAggrSessionFactory::CAggrSessionFactory(CReactor *pReactor, int nMaxSession,unsigned int dwDefaultAggrID)
:CSessionFactory(pReactor, nMaxSession)
{
	m_dwDefaultAggrID = dwDefaultAggrID;
}

CAggrSessionFactory::~CAggrSessionFactory()
{

}

CSession *CAggrSessionFactory::CreateSession(CChannel *pChannel, unsigned int dwMark)
{
	bool bIsListener = (bool)dwMark;
	CAggrSession *pAggrSession = new CAggrSession(m_pReactor, pChannel);
	if (!bIsListener)
	{
		CSessionAggregation *pSessionAggr = GetSessionAggregation(m_dwDefaultAggrID);
		pSessionAggr->RegisterAggrCallback(this);
		pSessionAggr->AddSession(pAggrSession, true);
	}

	pAggrSession->RegisterPackageHandler(this);
	
	return pAggrSession;
}

void CAggrSessionFactory::OnSessionDisconnected(CSession *pSession, int nReason)
{
	CSessionAggrMap::iterator i = m_mapSessionAggr.begin();
	for (;i != m_mapSessionAggr.end(); i++)
	{
		if((*i).second->RemoveSession((CAggrSession *)pSession))
		{
			//破坏了迭代器，一定要退出循环
			break;
		}
	}
	
	CSessionFactory::OnSessionDisconnected(pSession, nReason);
}

CSessionAggregation *CAggrSessionFactory::GetSessionAggregation(unsigned int dwAggregationID)
{
	CSessionAggrMap::iterator i = m_mapSessionAggr.find(dwAggregationID);
	if (i != m_mapSessionAggr.end())
	{
		return (*i).second;
	}
	CSessionAggregation *pSessionAggr = CreateSessionAggregation(dwAggregationID);
	m_mapSessionAggr[dwAggregationID] = pSessionAggr;

	return pSessionAggr;
}

void CAggrSessionFactory::OnSessionAggrLogin(CSessionAggregation *pSessionAggr)
{
}

void CAggrSessionFactory::OnSessionAggrDisconnected(CSessionAggregation *pSessionAggr, int nReason)
{
	m_mapSessionAggr.erase(pSessionAggr->GetAggregationID());
}

void CAggrSessionFactory::OnSessionAggrWarning(CSessionAggregation *pSessionAggr,int nReason, int nParam)
{
}

int CAggrSessionFactory::HandleAggrPackage(CAggrPackage *pPackage, CAggrSession *pSession)
{
	if (pPackage->GetType() == AGGR_TYPE_LOGIN)
	{
		unsigned int dwAggregationID = pPackage->GetAggregationID();
		
		CSessionAggregation *pSessionAggr = GetSessionAggregation(dwAggregationID);
		pSessionAggr->RegisterAggrCallback(this);
		pSessionAggr->AddSession(pSession, false);
		pSession->SetAggregationID(dwAggregationID, false);
	}

	return 0;
}
