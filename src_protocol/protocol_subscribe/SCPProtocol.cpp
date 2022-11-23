 // SCPProtocol.cpp: implementation of the CSCPProtocol class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "SCPProtocol.h"
#include "SCPPackage.h"



CSCPProtocol::CSCPProtocol(CReactor *pReactor)
:CProtocol(pReactor, SCPHLEN, new CSCPPackage(), NULL)
{
}

CSCPProtocol::~CSCPProtocol()
{
}

