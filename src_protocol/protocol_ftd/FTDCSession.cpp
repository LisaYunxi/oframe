#include "public.h"
#include "FTDCPackage.h"
#include "FTDCSession.h"

const unsigned int XMPTypeFTDC = 0x01;

CFTDCSession::CFTDCSession(CReactor *pReactor, CChannel *pChannel) :CXMPSession(pReactor, pChannel)
{
	m_pCRPProtocol = new CCompressProtocol(m_pReactor);
	m_pCRPProtocol->AttachLower(m_pXMPProtocol, XMPTypeCompressed);

	m_pFTDCProtocol = new CFTDCProtocol(m_pReactor);
	m_pFTDCProtocol->AttachLower(m_pCRPProtocol, XMPTypeFTDC);

	m_pFTDCProtocol->RegisterErrorHandler(this);
	m_pFTDCProtocol->RegisterUpperHandler(this);

	m_pPackageHandler = NULL;
}

CFTDCSession::~CFTDCSession()
{
	delete m_pFTDCProtocol;
	delete m_pCRPProtocol;
}

int CFTDCSession::HandleEvent(int nEventID, unsigned int dwParam, void *pParam)
{
	switch (nEventID)
	{
	case MSG_XMPERR_RECVHEARTBEAT:
	case MSG_XMPERR_SENDHEARTBEAT:
	case MSG_XMPERR_BADPACKAGE:
	case MSG_FTDCERR_BADPACKAGE:
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

void CFTDCSession::GetIds(int *pReadId, int *pWriteId)
{
	CXMPSession::GetIds(pReadId, pWriteId);
	if (*pWriteId == 0)
	{
		m_pFTDCProtocol->PublishSend();
		CXMPSession::GetIds(pReadId, pWriteId);
	}
}

void CFTDCSession::RegisterPackageHandler(CFTDCSessionCallback *pPackageHandler)
{
	m_pPackageHandler = pPackageHandler;
}

int CFTDCSession::HandlePackage(CPackage *pPackage, CProtocol *pProtocol)
{
	if (pProtocol == m_pFTDCProtocol)
	{
		CFTDCPackage *pFTDCPackage = (CFTDCPackage *)pPackage;
		if (m_pPackageHandler == NULL)
			return 0;
		return m_pPackageHandler->HandlePackage(pFTDCPackage, this);
	}
	return 0;
}

void CFTDCSession::Publish(CReadFlow *pFlow, unsigned short wSequenceSeries, UF_INT8 nStartId, UF_INT8 nBusinessNo, int nReaderTimes)
{
	m_pFTDCProtocol->Publish(pFlow, wSequenceSeries, nStartId, nBusinessNo, nReaderTimes);
}

void CFTDCSession::UnPublish(unsigned short wSequenceSeries)
{
	m_pFTDCProtocol->UnPublish(wSequenceSeries);
}

void CFTDCSession::UnPublishAll()
{
	m_pFTDCProtocol->UnPublishAll();
}

CFTDCPackage *CFTDCSession::CreatePackage()
{
	CFTDCPackage *pPackage = new CFTDCPackage;
	pPackage->ConstructAllocate(PACKAGE_MAX_SIZE, m_pFTDCProtocol->GetHdrReserveLen());
	return pPackage;
}

void CFTDCSession::SendRequestPackage(CFTDCPackage *pPackage)
{
	m_pFTDCProtocol->Send(pPackage);
}

void CFTDCSession::RegisterSubscriber(CFTDCSubscriber *pSubscriber)
{
	m_pFTDCProtocol->RegisterSubscriber(pSubscriber);
}

void CFTDCSession::UnRegisterSubscriber(CFTDCSubscriber *pSubscriber)
{
	m_pFTDCProtocol->UnRegisterSubscriber(pSubscriber);
}

void CFTDCSession::Disconnect(int nErrorCode)
{
	//如果是主动断开，则尝试发送流中未发完的包
	if (nErrorCode == 0)
		m_pFTDCProtocol->PublishSend();
	m_pFTDCProtocol->Clear();
	CXMPSession::Disconnect(nErrorCode);
}

void CFTDCSession::SetCompressMethod(unsigned char chCompressMethod)
{
	m_pCRPProtocol->SetCompressMethod(XMPTypeFTDC, chCompressMethod);
}

void CFTDCSession::SetVersion(unsigned char version)
{
	m_version = version;
}

unsigned char CFTDCSession::GetVersion(void)
{
	return m_version;
}
