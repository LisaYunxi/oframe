// ArbProtocol.cpp: implementation of the CArbProtocol class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "ArbProtocol.h"



CArbProtocol::CArbProtocol(CReactor *pReactor)
:CProtocol(pReactor, ARBHLEN, new CGuardPackage(), NULL)
{
}

CArbProtocol::~CArbProtocol()
{

}

