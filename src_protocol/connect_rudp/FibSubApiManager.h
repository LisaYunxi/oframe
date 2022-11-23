// FibSubApiManager.h: interface for the CFibSubApiManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBSUBAPIMANAGER_H__31E0B14D_F679_4359_B470_1AC132227416__INCLUDED_)
#define AFX_FIBSUBAPIMANAGER_H__31E0B14D_F679_4359_B470_1AC132227416__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FibSubscribeApi.h"

class CFibSubApiManager  
{
public:
	CFibSubApiManager(CReactor *pReactor, DWORD nNodeID);
	virtual ~CFibSubApiManager();
	CFibSubscribeApi *GetFibSubscribeApi(char *pszAddress);

	//增加一个与制定Reactor绑定的订阅接口
	CFibSubscribeApi *GetFibSubscribeApi(char *pszAddress,CReactor *pReactor);
private:
	CReactor *m_pReactor;
	DWORD m_nNodeID;
	typedef map<const char *, CFibSubscribeApi *, ltstr> CFibSubscribeApiMap;
	CFibSubscribeApiMap m_mapFibSubscribeApi;
};

#endif // !defined(AFX_FIBSUBAPIMANAGER_H__31E0B14D_F679_4359_B470_1AC132227416__INCLUDED_)
