// EventHandler.cpp: implementation of the CEventHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "BlockEventHandler.h"
#include "BlockReactor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlockEventHandler::CBlockEventHandler(CBlockReactor *pReactor)
{
	m_pReactor=pReactor;
}

CBlockEventHandler::~CBlockEventHandler()
{
	KillTimer(0);
	m_pReactor->RemoveIO(this);
	//m_pReactor->ClearEvent(this);
}

void CBlockEventHandler::SetTimer(int nIDEvent, int nElapse)
{
	m_pReactor->RegisterTimer(this,nIDEvent,nElapse);
}

void CBlockEventHandler::KillTimer(int nIDEvent)
{
	m_pReactor->RemoveTimer(this,nIDEvent);
}

bool CBlockEventHandler::PostEvent(int message, DWORD dwParam, void *pParam, release_func pReleaseFunc)
{
	return m_pReactor->PostEvent(this,message,dwParam,pParam, pReleaseFunc);
}

int CBlockEventHandler::SendEvent(int message, DWORD dwParam, void *pParam)
{
	return m_pReactor->SendEvent(this,message,dwParam,pParam);
}

