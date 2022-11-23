// FibSubApiManager.cpp: implementation of the CFibSubApiManager class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "FibSubApiManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFibSubApiManager::CFibSubApiManager(CReactor *pReactor, DWORD nNodeID)
{
	m_pReactor = pReactor;
	m_nNodeID = nNodeID;
}

CFibSubApiManager::~CFibSubApiManager()
{
	CFibSubscribeApiMap::iterator i = m_mapFibSubscribeApi.begin();
	for(; i!=m_mapFibSubscribeApi.end(); i++)
	{
		((*i).second)->Release();
	}
}

CFibSubscribeApi *CFibSubApiManager::GetFibSubscribeApi(char *pszAddress)
{
	CFibSubscribeApi *pFibSubscribeApi = NULL;
	CFibSubscribeApiMap::iterator i = m_mapFibSubscribeApi.find(pszAddress);
	if (i != m_mapFibSubscribeApi.end())
	{
		pFibSubscribeApi = (*i).second; 
	}
	else
	{
		pFibSubscribeApi = CFibSubscribeApi::CreateSubscribeApi(m_pReactor,
			m_nNodeID, pszAddress);
		m_mapFibSubscribeApi[pFibSubscribeApi->GetAddress()] = pFibSubscribeApi;
	}

	return pFibSubscribeApi;
}

CFibSubscribeApi *CFibSubApiManager::GetFibSubscribeApi(char *pszAddress,CReactor *pReactor)
{
	CFibSubscribeApi *pFibSubscribeApi = NULL;
	CFibSubscribeApiMap::iterator i = m_mapFibSubscribeApi.find(pszAddress);
	if (i != m_mapFibSubscribeApi.end())
	{
		pFibSubscribeApi = (*i).second; 
	}
	else
	{
		pFibSubscribeApi = CFibSubscribeApi::CreateSubscribeApi(pReactor,m_nNodeID, pszAddress);
		m_mapFibSubscribeApi[pFibSubscribeApi->GetAddress()] = pFibSubscribeApi;
	}
	return pFibSubscribeApi;
}

