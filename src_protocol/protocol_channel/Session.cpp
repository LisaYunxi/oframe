#include "public.h"
#include "Session.h"

static unsigned int g_dwSessionIDBase = 0;

CSession::CSession(CReactor *pReactor, CChannel *pChannel, int nMaxPackageSize) :CEventHandler(pReactor)
{
	m_bSuspendRead = false;
	m_nSessionID = ++g_dwSessionIDBase + (time(NULL) << 16);
	if (pChannel == NULL) 
	{
		EMERGENCY_EXIT("CXTPControl can't use NULL channel");
	}
	m_pChannel = pChannel;
	m_pChannelProtocol = new CChannelProtocol(m_pReactor, pChannel, nMaxPackageSize);
	m_pChannelProtocol->RegisterErrorHandler(this);
	m_pSessionCallback = NULL;
}

CSession::~CSession()
{
	delete m_pChannelProtocol;
	m_pChannel->Disconnect();
	delete m_pChannel;
}


int CSession::HandleInput()
{
	return m_pChannelProtocol->HandleInput();
}

int CSession::HandleOutput()
{
	return m_pChannelProtocol->HandleOutput();
}

int CSession::HandleEvent(int nEventID, unsigned int dwParam, void *pParam)
{
	switch (nEventID)
	{
	case MSG_DRSTROY_ME:
		delete this;
		return 0;
	case MSG_CHNLERR_READ:
	case MSG_CHNLERR_WRITE:
		Disconnect(nEventID);
		return 0;
	}
	return 0;
}

void CSession::Disconnect(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		//主动断开，尝试将未发送的数据尽量发出
		if (m_pChannelProtocol->Flush() < 0)
		{
			//会话已经销毁了，直接返回
			return;
		}
	}
	m_pChannel->Disconnect();
	OnChannelLost(nErrorCode);
	delete this;
	//	PostEvent(MSG_DRSTROY_ME, 0, this);
}

void CSession::GetIds(int *pReadId, int *pWriteId)
{
	m_pChannelProtocol->GetIds(pReadId, pWriteId);
	if (m_bSuspendRead)
	{
		*pReadId = 0;
	}
}

void CSession::OnChannelLost(int nErrorCode)
{
	if (m_pSessionCallback != NULL)
	{
		m_pSessionCallback->OnSessionDisconnected(this, nErrorCode);
	}
}

void CSession::SuspendRead(bool bSuspend)
{
	m_bSuspendRead = bSuspend;
}

bool CSession::IsChannelWriteBusy()
{
	return m_pChannelProtocol->IsWriteBusy();
}

void CSession::ReSetSessionID(int nThreadID)
{
	m_nSessionID = m_nSessionID & 0x0fffffff | (nThreadID << 28);
}

