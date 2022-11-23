// CompState.cpp: implementation of the CCompState class.
//

//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "CompState.h"


CCompState::CCompState(CCompServer *pCommServer)
{
	m_pCommServer = pCommServer;
}

CCompState::~CCompState()
{
}

void CCompState::OnHotlinePackageReceived(CHotlinePackage *pHotlinePackage)
{
	EMERGENCY_EXIT("Can't exec the opration in this state");
}

void CCompState::OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode)
{
	EMERGENCY_EXIT("Can't exec the opration in this state");
}

void CCompState::OnGuardPackageReceived(CGuardPackage *pArbPackage)
{
	EMERGENCY_EXIT("Can't exec the opration in this state");
}

void CCompState::OnGuardChannelLost(CGuardSession *pSession, int ErrorCode)
{
	EMERGENCY_EXIT("Can't exec the opration in this state");
}

