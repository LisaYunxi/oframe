// ArbProtocol.h: interface for the CArbProtocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARBPROTOCOL_H__21D58A29_0774_4DA1_ACAC_07BEC3CC5AC9__INCLUDED_)
#define AFX_ARBPROTOCOL_H__21D58A29_0774_4DA1_ACAC_07BEC3CC5AC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Protocol.h"
#include "ArbPackage.h"

class CArbProtocol : public CProtocol  
{
public:
	CArbProtocol(CReactor *pReactor);
	virtual ~CArbProtocol();
};

#endif // !defined(AFX_ARBPROTOCOL_H__21D58A29_0774_4DA1_ACAC_07BEC3CC5AC9__INCLUDED_)
