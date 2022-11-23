// ListenCtrl.cpp: implementation of the CListenCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "ListenCtrl.h"



CListenCtrl::CListenCtrl(CReactor *pReactor,CServerBase *pServer)
:CEventHandler(pReactor)
{
	m_pServer=pServer;
}

CListenCtrl::~CListenCtrl()
{
	delete m_pServer;
}

int CListenCtrl::HandleInput()
{
	CChannel *pChannel=m_pServer->Accept(0);
	if (pChannel==NULL)
		return -1;
	return OnConnected(pChannel);
}

void CListenCtrl::GetIds(int *pReadId, int *pWriteId)
{
	*pReadId=m_pServer->GetId();
	*pWriteId=0;
}

CUdpListenCtrl::CUdpListenCtrl(CReactor *pReactor, CServerBase *pServer)
:CEventHandler(pReactor)
{
	m_pServer = pServer;
	m_pChannel = m_pServer->Accept(0);
}

CUdpListenCtrl::~CUdpListenCtrl()
{
	delete m_pChannel;
	delete m_pServer;
}

int CUdpListenCtrl::HandleInput()
{
	return OnRecvDatagram(m_pChannel);
}

void CUdpListenCtrl::GetIds(int *pReadId, int *pWriteId)
{
	*pReadId=m_pChannel->GetId();
	*pWriteId=0;
}

