#include "public.h"
#include "ArbSession.h"

CGuardSession::CGuardSession(CReactor *pReactor, CArbPackageHandler *pArbPackageHandler,unsigned int dwAggregationID)
:CSessionAggregation(pReactor, dwAggregationID)
{
	m_pArbPackageHandler = pArbPackageHandler;

	m_pArbProtocol = new CArbProtocol(pReactor);
	m_pArbProtocol->AttachLower(m_pAggrProtocol, dwAggregationID);
	m_pArbProtocol->RegisterUpperHandler(this);
	m_pArbProtocol->RegisterErrorHandler(this);

	m_prePackage.ConstructAllocate(ARB_PACKAGE_MAX_SIZE, 256);
}

CGuardSession::~CGuardSession()
{
	delete m_pArbProtocol;
}

int CGuardSession::SendStateCommand(unsigned int nState)
{
	m_prePackage.Clear();
	m_prePackage.SetTid(HTID_STATE_COMMAND);
	m_prePackage.SetStateID(nState);
	return m_pArbProtocol->Send(&m_prePackage);
}

int CGuardSession::SendStateReport(unsigned int nState)
{
	m_prePackage.Clear();
	m_prePackage.SetTid(HTID_STATE_REPORT);
	m_prePackage.SetStateID(nState);
	return m_pArbProtocol->Send(&m_prePackage);
}

int CGuardSession::SendEventNotify(unsigned int nEvent)
{
	m_prePackage.Clear();
	m_prePackage.SetTid(HTID_EVENT_NOTIFY);
	m_prePackage.SetStateID(nEvent);
	return m_pArbProtocol->Send(&m_prePackage);
}

int CGuardSession::HandlePackage(CPackage *pPackage, CProtocol *pProtocol)
{
	m_pArbPackageHandler->HandleGuardPackage((CGuardPackage *)pPackage, this);
	return CSessionAggregation::HandlePackage(pPackage, pProtocol);
}
