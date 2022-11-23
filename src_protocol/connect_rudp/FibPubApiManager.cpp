// FibPubApiManager.cpp: implementation of the CFibPubApiManager class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "FibPubApiManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFibPubApiManager::CFibPubApiManager(CReactor *pReactor, DWORD nNodeID)
{
	m_pReactor = pReactor;
	m_nNodeID = nNodeID;
}

CFibPubApiManager::~CFibPubApiManager()
{
	CFibPublishApiMap::iterator i = m_mapFibPublishApi.begin();
	for(; i!=m_mapFibPublishApi.end(); i++)
	{
		((*i).second)->Release();
	}
}

CFibPublishApi *CFibPubApiManager::GetFibPublishApi(char *pszAddress)
{
	CFibPublishApi *pFibPublishApi = NULL;
	CFibPublishApiMap::iterator i = m_mapFibPublishApi.find(pszAddress);
	if (i != m_mapFibPublishApi.end())
	{
		pFibPublishApi = (*i).second; 
	}
	else
	{
		pFibPublishApi = CFibPublishApi::CreatePublishApi(m_pReactor,
			m_nNodeID, pszAddress);
		m_mapFibPublishApi[pFibPublishApi->GetAddress()] = pFibPublishApi;
	}

	return pFibPublishApi;
}
