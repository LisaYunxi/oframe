// AggrSession.h: interface for the CAggrSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGGRSESSION_H__74E7843A_FC9F_4A5C_BDFB_B1260D71F468__INCLUDED_)
#define AFX_AGGRSESSION_H__74E7843A_FC9F_4A5C_BDFB_B1260D71F468__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "XMPSession.h"
#include "AggrProtocol.h"

class CAggrSession;

class CAggrPackageHandler
{
public:
	virtual int HandleAggrPackage(CAggrPackage *pPackage, CAggrSession *pSession) = 0;
};

class CAggrSession : public CXMPSession, public CProtocolCallback
{
public:
	CAggrSession(CReactor *pReactor, CChannel *pChannel);
	virtual ~CAggrSession();

	void SetAggregationID(unsigned int dwAggregationID, bool bLogin);

	virtual int HandlePackage(CPackage *pPackage, CProtocol *pProtocol);
	
	void AttachCarryAggrProtocol(CAggrProtocol *pAggrProtocol);

	void DetachCarryAggrProtocol(CAggrProtocol *pAggrProtocol);

	void RegisterPackageHandler(CAggrPackageHandler *pPackageHandler);

	void ClearChannel();
protected:
	CAggrProtocol *m_pAggrProtocol;
	CAggrPackageHandler *m_pPackageHandler;
};

#endif // !defined(AFX_AGGRSESSION_H__74E7843A_FC9F_4A5C_BDFB_B1260D71F468__INCLUDED_)
