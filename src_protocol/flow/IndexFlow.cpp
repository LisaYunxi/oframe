#include "public.h"
#include "IndexFlow.h"

CIndexFlow::CIndexFlow(const char* pName, int nMaxSize)
{
	m_nMaxSize = nMaxSize-1;
	//INIT_CRITICAL_VAR(m_criticalVar);
	m_sName = pName;
}

CIndexFlow::~CIndexFlow()
{
	//DELETE_CRITICAL_VAR(m_criticalVar);
}

UF_INT8 CIndexFlow::Append(UF_INT8 nIndex)
{
	//ENTER_CRITICAL(m_criticalVar);
	m_RWLock.WriteLock();
	m_PackageIndex.push_back(nIndex);
	if (m_PackageIndex.size() > m_nMaxSize)
	{
		m_PackageIndex.pop_front();
		m_nFirstID++;
	}
	m_nCount++;
	//LEAVE_CRITICAL(m_criticalVar);
	m_RWLock.UnLock();
	//printf("\n");
	//printf("-------------------------------\n");
	//printf("index_flow:insert index[%0.f]\n", nIndex);
	//printf("index_flow:first_index_ID[%0.f],last_index_ID[%0.f]\n", m_nFirstID, m_nFirstID + m_PackageIndex.size()-1);
	//printf("index_flow:first[%0.f],last[%0.f]\n", m_PackageIndex[0],m_PackageIndex[m_PackageIndex.size()-1]);

	return m_nCount - 1;
}

int CIndexFlow::Get(UF_INT8 id, void *pObject, int length)
{
	//ENTER_CRITICAL(m_criticalVar);
	m_RWLock.ReadLock();
	if (id < m_nFirstID)
	{
		//LEAVE_CRITICAL(m_criticalVar);
		m_RWLock.UnLock();
		return -1;
	}
	int nIndex = id - m_nFirstID;
	int ret =  m_pUnderFlow->Get(m_PackageIndex[nIndex], pObject, length);
	//LEAVE_CRITICAL(m_criticalVar);
	m_RWLock.UnLock();
	return ret;
}

bool CIndexFlow::Truncate(UF_INT8 nCount)
{
	if (nCount <= GetCount())
	{
		return true;
	}
	int nResize = nCount - m_nFirstID;
	m_PackageIndex.resize(nResize);
	return true;
}