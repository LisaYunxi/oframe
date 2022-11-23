// SessionAggregation.cpp: implementation of the CSessionAggregation class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "SessionAggregation.h"

const int DEFAULT_HEARTBEAT_TIMEOUT = 60;



CSessionAggregation::CSessionAggregation(CReactor *pReactor, unsigned int dwAggregationID)
:CEventHandler(pReactor)
{
	m_dwAggregationID = dwAggregationID;
	m_pCallback = NULL;
	m_pAggrProtocol = new CAggrProtocol(pReactor);
	m_pAggrProtocol->RegisterUpperHandler(this);
	m_pAggrProtocol->RegisterErrorHandler(this);
	m_dwReadTimeout = DEFAULT_HEARTBEAT_TIMEOUT;
}

CSessionAggregation::~CSessionAggregation()
{
	delete m_pAggrProtocol;
}

unsigned int CSessionAggregation::GetAggregationID()
{
	return m_dwAggregationID;
}

void CSessionAggregation::Disconnect(int nErrorCode)
{
	CAggrSessionMap::iterator i = m_mapSession.begin();
	for (; i != m_mapSession.end(); i++)
	{
		(*i).second->Disconnect(nErrorCode);
	}
	if (m_pCallback != NULL) 
	{
		m_pCallback->OnSessionAggrDisconnected(this, nErrorCode);
	}
	delete this;
}

bool CSessionAggregation::AddSession(CAggrSession *pAggrSession, bool bLogin)
{
	bool bIsEmpty = m_mapSession.empty();
	pAggrSession->AttachCarryAggrProtocol(m_pAggrProtocol);
	m_mapSession[pAggrSession->GetSessionID()] = pAggrSession;
	pAggrSession->SetHeartbeatTimeout(m_dwReadTimeout);
	pAggrSession->SetAggregationID(GetAggregationID(), bLogin);
	if (bIsEmpty && m_pCallback!=NULL)
	{
		m_pCallback->OnSessionAggrLogin(this);
	}
	return bIsEmpty;
}

bool CSessionAggregation::RemoveSession(CAggrSession *pAggrSession)
{
	pAggrSession->DetachCarryAggrProtocol(m_pAggrProtocol);
	int n = m_mapSession.erase(pAggrSession->GetSessionID());
	if (m_mapSession.empty())
	{
		Disconnect(MSG_AGGRERR_DISCONNECT);
		return true;
	}
	
	if (n > 0 && m_pCallback!=NULL)
	{
		m_pCallback->OnSessionAggrWarning(this, MSG_AGGRERR_DISCONNECT,pAggrSession->GetSessionID());
	}
	return false;
}

void CSessionAggregation::SetHeartbeatTimeout(unsigned int dwReadTimeout)
{
	m_dwReadTimeout = dwReadTimeout;
	CAggrSessionMap::iterator i = m_mapSession.begin();
	for (; i != m_mapSession.end(); i++)
	{
		(*i).second->SetHeartbeatTimeout(m_dwReadTimeout);
	}
}

int CSessionAggregation::HandlePackage(CPackage *pPackage, CProtocol *pProtocol)
{
	return 0;
}

