#include "public.h"
#include "TcpXtpSession.h"
#include "SelectReactor.h"
const unsigned int XMPTypeMDB = 0x01;

CTcpXtpSession::CTcpXtpSession(CReactor *pReactor, CChannel *pChannel, CTcpXtpSessionCallback *pSeesionCallback, unsigned int nSubjectNo)
	:CXMPSession(pReactor, pChannel)
{
	m_pCRPProtocol = new CCompressProtocol(m_pReactor);
	m_pCRPProtocol->AttachLower(m_pXMPProtocol, XMPTypeCompressed);
	m_pProtocol = new CTcpXtpProtocol(m_pReactor, nSubjectNo);
	m_pProtocol->AttachLower(m_pCRPProtocol, XMPTypeMDB);
	m_pProtocol->RegisterErrorHandler(this);
	m_pProtocol->RegisterUpperHandler(this);
	m_prePackage.ConstructAllocate(PACKAGE_MAX_SIZE, m_pProtocol->GetHdrReserveLen());
	//SetCompressMethod(CRPCM_ZERO);
	m_pCallback = pSeesionCallback;
	m_nSubjectNo = nSubjectNo;
}

CTcpXtpSession::~CTcpXtpSession()
{
	//先将upper的协议delete
	delete m_pProtocol;
	delete m_pCRPProtocol;
}

int CTcpXtpSession::SendReportPackage(CXTPPackage *pPackage)
{
	return m_pProtocol->Send(pPackage);
}

int CTcpXtpSession::HandlePackage(CPackage *pPackage, CProtocol *pProtocol)
{
	m_pCallback->HandlePackage((CXTPPackage*)pPackage, this);
	return 0;
}

void CTcpXtpSession::SetCompressMethod(unsigned char chCompressMethod)
{
	m_pCRPProtocol->SetCompressMethod(XMPTypeMDB, chCompressMethod);
}

void CTcpXtpSession::GetIds(int *pReadId, int *pWriteId)
{
	m_pProtocol->PublishSend();
	CXMPSession::GetIds(pReadId, pWriteId);
}

void  CTcpXtpSession::PublishFlow(CReadFlow *pFlow, UF_INT8 nStartId)
{
	m_pProtocol->PublishFlow(pFlow, nStartId);
}

