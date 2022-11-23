// EventHandler.cpp: implementation of the CEventHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "EventHandler.h"
#include "Reactor.h"



CEventHandler::CEventHandler(CReactor *pReactor)
{
	m_pReactor=pReactor;
}

CEventHandler::~CEventHandler()
{
	KillTimer(0);
	m_pReactor->RemoveIO(this);
	m_pReactor->ClearEvent(this);
}

void CEventHandler::SetTimer(int nIDEvent, int nElapse)
{
	m_pReactor->RegisterTimer(this,nIDEvent,nElapse);
}

void CEventHandler::KillTimer(int nIDEvent)
{
	m_pReactor->RemoveTimer(this,nIDEvent);
}

bool CEventHandler::PostEvent(int message, unsigned int dwParam, void *pParam)
{
	return m_pReactor->PostEvent(this,message,dwParam,pParam);
}

int CEventHandler::SendEvent(int message, unsigned int dwParam, void *pParam)
{
	return m_pReactor->SendEvent(this,message,dwParam,pParam);
}

