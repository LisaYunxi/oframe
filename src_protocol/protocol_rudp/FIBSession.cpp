// FIBSession.cpp: implementation of the CFIBSession class.
//
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "XTPPackage.h"
#include "FIBSession.h"

const DWORD XMPTypeFIB = 0x01;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFIBSession::CFIBSession(CReactor *pReactor,CChannel *pChannel)
:CXMPSession(pReactor, pChannel)
{
	m_pCRPProtocol = new CCompressProtocol(m_pReactor);
	m_pCRPProtocol->AttachLower(m_pXMPProtocol, XMPTypeCompressed);
	m_pFIBProtocol = new CFIBProtocol(m_pReactor);
	m_pFIBProtocol->AttachLower(m_pCRPProtocol, XMPTypeFIB);

	m_pFIBProtocol->RegisterErrorHandler(this);
	m_pFIBProtocol->RegisterUpperHandler(this);
	
	m_pPackageHandler = NULL;
	
}

CFIBSession::~CFIBSession()
{
	delete m_pFIBProtocol;
	
	// longqy 2013-02-19
	delete m_pCRPProtocol;
}

int CFIBSession::HandleEvent(int nEventID, DWORD dwParam, void *pParam)
{
	switch(nEventID)
	{
	case MSG_XMPERR_RECVHEARTBEAT:
	case MSG_XMPERR_SENDHEARTBEAT:
	case MSG_XMPERR_BADPACKAGE:
	case MSG_FIBERR_BADPACKAGE:
		Disconnect(nEventID);
		return 0;
	case MSG_XMPWARNING_RECVHEARTBEAT:
		if (m_pSessionCallback != NULL)
		{
			m_pSessionCallback->OnSessionWarning(this, nEventID, (int)dwParam);
		}
		return 0;
	}

	return CSession::HandleEvent(nEventID, dwParam, pParam);
}	

void CFIBSession::GetIds(int *pReadId, int *pWriteId)
{
	m_pFIBProtocol->PublishSend();

	CXMPSession::GetIds(pReadId, pWriteId);
}

void CFIBSession::RegisterPackageHandler(CFIBSessionCallback *pPackageHandler)
{
	m_pPackageHandler = pPackageHandler;
}

int CFIBSession::HandlePackage(CPackage *pPackage,CProtocol *pProtocol)
{
	if (pProtocol == m_pFIBProtocol)
	{
		CXTPPackage *pFIBPacakge = (CXTPPackage *)pPackage;
		if (m_pPackageHandler == NULL)
		{
			return 0;
		}
		return m_pPackageHandler->HandlePackage(pFIBPacakge, this);
	}
	return 0;
}


void CFIBSession::Publish(CReadOnlyFlow *pFlow, WORD wSequenceSeries, int nStartId)
{
	m_pFIBProtocol->Publish(pFlow, wSequenceSeries, nStartId);
}

void CFIBSession::UnPublish(WORD wSequenceSeries)
{
	m_pFIBProtocol->UnPublish(wSequenceSeries);
}

CXTPPackage *CFIBSession::CreatePackage()
{
	CXTPPackage *pPackage = new CXTPPackage;
	pPackage->ConstructAllocate(XTP_PACKAGE_MAX_SIZE+XTPHLEN, m_pFIBProtocol->GetHdrReserveLen());
	return pPackage;
}

void CFIBSession::SendRequestPackage(CXTPPackage *pPackage)
{
	m_pFIBProtocol->Send(pPackage);
}

void CFIBSession::RegisterSubscriber(CFIBSubscriber *pSubscriber)
{
	m_pFIBProtocol->RegisterSubscriber(pSubscriber);
}

void CFIBSession::UnRegisterSubscriber(CFIBSubscriber *pSubscriber)
{
	m_pFIBProtocol->UnRegisterSubscriber(pSubscriber);
}

void CFIBSession::SetCommPhaseNo(DWORD wCommPhaseNo)
{
	m_pFIBProtocol->SetCommPhaseNo(wCommPhaseNo);
}

void CFIBSession::Disconnect(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		//如果是主动断开，则尝试发送流中未发完的包
		m_pFIBProtocol->PublishSend();
	}

	m_pFIBProtocol->Clear();
	
	CXMPSession::Disconnect(nErrorCode);
}

void CFIBSession::SetCompressMethod(BYTE chCompressMethod)
{
	m_pCRPProtocol->SetCompressMethod(XMPTypeFIB, chCompressMethod);
}

void CFIBSession::SetVersion(BYTE version)
{
	m_version=version;
}
	
BYTE CFIBSession::GetVersion(void)
{
	return m_version;
}
