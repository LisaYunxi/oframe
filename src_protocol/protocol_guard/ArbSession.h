
#if !defined(AFX_ARBSESSION_H__74E7843A_FC9F_4A5C_BDFB_B1260D71F468__INCLUDED_)
#define AFX_ARBSESSION_H__74E7843A_FC9F_4A5C_BDFB_B1260D71F468__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "SessionAggregation.h"
#include "ArbProtocol.h"

class CGuardSession;

class CArbPackageHandler
{
public:
	virtual void HandleGuardPackage(CGuardPackage *pPackage, CGuardSession *pSession) = 0;
};

class CGuardSession : public CSessionAggregation 
{
public:
	CGuardSession(CReactor *pReactor, CArbPackageHandler *pArbPackageHandler, 
		unsigned int dwAggregationID);
	virtual ~CGuardSession();
	virtual int HandlePackage(CPackage *pPackage, CProtocol *pProtocol);
	int SendStateCommand(unsigned int nState);
	int SendStateReport(unsigned int nState);
	int SendEventNotify(unsigned int nEvent);
private:
	CGuardPackage m_prePackage;
	CArbProtocol *m_pArbProtocol;
	CArbPackageHandler *m_pArbPackageHandler;
};

#endif // !defined(AFX_ARBSESSION_H__74E7843A_FC9F_4A5C_BDFB_B1260D71F468__INCLUDED_)
