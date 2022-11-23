#include "public.h"
#include "AggrSession.h"

CAggrSession::CAggrSession(CReactor *pReactor,CChannel *pChannel)
:CXMPSession(pReactor, pChannel)
{
	m_pPackageHandler = NULL;

	m_pAggrProtocol = new CAggrProtocol(pReactor);
	m_pAggrProtocol->AttachLower(m_pXMPProtocol, XMPTypeNone);
	m_pAggrProtocol->RegisterErrorHandler(this);
	m_pAggrProtocol->RegisterUpperHandler(this);
}

CAggrSession::~CAggrSession()
{
	delete m_pAggrProtocol;
}

void CAggrSession::SetAggregationID(unsigned int dwAggregationID, bool bLogin)
{
	if (bLogin)
	{
		m_pAggrProtocol->AggrLogin(dwAggregationID);
	}
}

int CAggrSession::HandlePackage(CPackage *pPackage, CProtocol *pProtocol)
{
	if (pProtocol == m_pAggrProtocol)
	{
		CAggrPackage *pAggrPackage = (CAggrPackage *)pPackage;
		if (m_pPackageHandler != NULL)
		{
			return m_pPackageHandler->HandleAggrPackage(pAggrPackage, this);
		}
		return 0;
	}
	return 0;
}

void CAggrSession::AttachCarryAggrProtocol(CAggrProtocol *pAggrProtocol)
{
	pAggrProtocol->AttachLower(m_pXMPProtocol, XMPTypeBinary);
}

void CAggrSession::DetachCarryAggrProtocol(CAggrProtocol *pAggrProtocol)
{
	pAggrProtocol->DetachLower(m_pXMPProtocol);
}

void CAggrSession::RegisterPackageHandler(CAggrPackageHandler *pPackageHandler)
{
	m_pPackageHandler = pPackageHandler;
}

void CAggrSession::ClearChannel()
{
	m_pChannel = NULL;
}
