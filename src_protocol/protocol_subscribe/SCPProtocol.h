  // SCPProtocol.h: interface for the CSCPProtocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCPPROTOCOL_H__64B9EF5C_30E8_46D1_89BB_27493E8A848B__INCLUDED_)
#define AFX_SCPPROTOCOL_H__64B9EF5C_30E8_46D1_89BB_27493E8A848B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Protocol.h"
#include "SCPPackage.h"

class CSCPProtocol : public CProtocol  
{
public:
	CSCPProtocol(CReactor *pReactor);
	virtual ~CSCPProtocol();
};

#endif // !defined(AFX_SCPPROTOCOL_H__64B9EF5C_30E8_46D1_89BB_27493E8A848B__INCLUDED_)
