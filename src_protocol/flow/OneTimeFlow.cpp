#include "public.h"
#include "OneTimeFlow.h"

COneTimeFlow::COneTimeFlow(const char * pName) :CCachedFlow(true, pName, CFNV_MAX_OBJECTS, 20000)
{
	m_iHaveGetID = -1;
	m_sName = pName;
}

COneTimeFlow::~COneTimeFlow()
{
}

UF_INT8 COneTimeFlow::Append(void *pObject, int length)
{
	UF_INT8 nCount = CCachedFlow::Append(pObject, length);

	//ENTER_CRITICAL(m_criticalVar);
	m_RWLock.WriteLock();
	while (m_nFirstID < m_iHaveGetID)
	{
		CCachedFlow::PopFront();
	}
	//LEAVE_CRITICAL(m_criticalVar);
	m_RWLock.UnLock();
	return nCount;
}

void* COneTimeFlow::GetPointer(UF_INT8 id, int& length)
{
	//ENTER_CRITICAL(m_criticalVar);
	m_RWLock.ReadLock();
	if (id >= m_nFirstID)
	{
		int n = id - m_nFirstID;
		TCachedFlowNode node = m_NodeQueue[n];
		length = node.size;
		m_iHaveGetID = id;
		//LEAVE_CRITICAL(m_criticalVar);
		m_RWLock.UnLock();
		return node.address;
	}
	//LEAVE_CRITICAL(m_criticalVar);
	m_RWLock.UnLock();
	return NULL;
};

int COneTimeFlow::GetFlowType()
{
	return FLOW_TYPE_ONETIMEFLOW;
}