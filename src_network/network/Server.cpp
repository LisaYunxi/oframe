// Server.cpp: implementation of the CServer class.
//
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "Server.h"



CServerBase::CServerBase(CServiceName *pName)
{
	SetErrorMsg(NULL);
	m_pServiceName=pName;
}

CServerBase::~CServerBase()
{
}

