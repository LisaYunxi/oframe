// FibPubApiManager.h: interface for the CFibPubApiManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBPUBAPIMANAGER_H__31E0B14D_F679_4359_B470_1AC132227416__INCLUDED_)
#define AFX_FIBPUBAPIMANAGER_H__31E0B14D_F679_4359_B470_1AC132227416__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FibPublishApi.h"

class CFibPubApiManager  
{
public:
	CFibPubApiManager(CReactor *pReactor, DWORD nNodeID);
	virtual ~CFibPubApiManager();
	CFibPublishApi *GetFibPublishApi(char *pszAddress);
private:
	CReactor *m_pReactor;
	DWORD m_nNodeID;
	typedef map<const char *, CFibPublishApi *, ltstr> CFibPublishApiMap;
	CFibPublishApiMap m_mapFibPublishApi;
};

#endif // !defined(AFX_FIBPUBAPIMANAGER_H__31E0B14D_F679_4359_B470_1AC132227416__INCLUDED_)
