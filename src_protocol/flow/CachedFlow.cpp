#include "public.h"
#include "CachedFlow.h"

CCachedFlow::CCachedFlow(bool bAutoSync, const char* pName, int nMaxObjects, int nDataBlockSize) :m_CacheList(nDataBlockSize)
{
	m_sName = pName;
	m_nMaxObjects = nMaxObjects;
	if (nMaxObjects > CFNV_MAX_OBJECTS)
	{
		//�������objects�������ڴ�������������
		//EMERGENCY_EXIT("nMaxObjects is larger than CFNV_MAX_OBJECTS");
		m_nMaxObjects = CFNV_MAX_OBJECTS;
	}
	m_nDataBlockSize = nDataBlockSize;
	m_bAutoSync = bAutoSync;
	m_nFirstID = UF_INT8_ZERO;
	Clear();
	//INIT_CRITICAL_VAR(m_criticalVar);
}

CCachedFlow::~CCachedFlow()
{
	//DELETE_CRITICAL_VAR(m_criticalVar);
}

void CCachedFlow::Clear()
{
	m_nFirstID = 0;
	m_NodeQueue.clear();
	m_CacheList.Clear();
}

bool CCachedFlow::AttachUnderFlow(CFlow *pFlow)
{
	if (pFlow == NULL)
		return false;

	m_pUnderFlow = pFlow;

	//��ջ���
	Clear();

	//���²����Ķ������뻺��
	char *buf = new char[m_nDataBlockSize];

	m_nFirstID = m_pUnderFlow->GetCount() - m_nMaxObjects;
	if (m_nFirstID < UF_INT8_ZERO)
		m_nFirstID = UF_INT8_ZERO;

	if (m_nFirstID < m_pUnderFlow->GetFirstID())
		m_nFirstID = m_pUnderFlow->GetFirstID();

	for (UF_INT8 i = m_nFirstID; i < m_pUnderFlow->GetCount(); i++)
	{
		int len = m_pUnderFlow->Get(i, buf, m_nDataBlockSize);
		Append(buf, len);
	}
	//���һ����û�У���Ҫ����m_nCount,Ҫ��Ȼ�Կ��ļ�ʹ�û����
	m_nCount = m_nFirstID + m_NodeQueue.size();
	delete[] buf;

	return true;
}

UF_INT8 CCachedFlow::Append(void *pObject, int length)
{
	//ENTER_CRITICAL(m_criticalVar);
	m_RWLock.WriteLock();
	if (m_NodeQueue.size() >= m_nMaxObjects)
		PopFront();

	TCachedFlowNode node;
	node.address = m_CacheList.PushBack(pObject, length);
	node.size = length;
	m_NodeQueue.push_back(node);
	m_nCount = m_nFirstID + m_NodeQueue.size();
	if (m_bAutoSync)
		SyncUnderFlow(m_nCount - 1);
	//LEAVE_CRITICAL(m_criticalVar);
	m_RWLock.UnLock();

	//����֪ͨ m_pEventHandler �������Ψһ���ã�����֪ͨ�������µ����ݡ�
	if (m_pEventHandler != NULL)
		m_pEventHandler->Notify();

	return m_nCount - 1;
}

int CCachedFlow::Get(UF_INT8 id, void *pObject, int length)
{
	//ENTER_CRITICAL(m_criticalVar);
	m_RWLock.ReadLock();
	if (id >= m_nFirstID)
	{	//Ҫ�ҵĶ����ڻ�����
		int n = id - m_nFirstID;
		TCachedFlowNode node = m_NodeQueue[n];
		if (node.size > length)
			EMERGENCY_EXIT("Insufficient buffer length while reading CCachedFlow node.size[%d]>length[%d]", node.size, length);

		memcpy(pObject, node.address, node.size);
		//LEAVE_CRITICAL(m_criticalVar);
		m_RWLock.UnLock();
		return node.size;
	}
	else
	{
		//��ʱ����Ҫ���ײ�������ȥ��ȡ������7*24Сʱ��ϵͳ�ᵼ�¶����ܾ�֮ǰ������
		//����������ϵͳѹ��
		if (m_pUnderFlow != NULL)
		{
			//LEAVE_CRITICAL(m_criticalVar);
			m_RWLock.UnLock();
			return m_pUnderFlow->Get(id, pObject, length);
		}
	}
	//LEAVE_CRITICAL(m_criticalVar);
	m_RWLock.UnLock();
	return -1;
}

bool CCachedFlow::SyncUnderFlow(UF_INT8 id)
{
	if (m_pUnderFlow == NULL)
		return false;

	UF_INT8 undercount = GetUnderCount();
	if (id != undercount)
		return false;

	if (GetCount() == undercount)
		return false;

	int n = undercount - m_nFirstID;
	TCachedFlowNode node = m_NodeQueue[n];
	m_pUnderFlow->Append(node.address, node.size);
	return true;
}

bool CCachedFlow::Truncate(UF_INT8 nCount)
{
	if (m_pUnderFlow != NULL)
	{
		if (!m_pUnderFlow->Truncate(nCount))
			return false;
		CCachedFlow::AttachUnderFlow(m_pUnderFlow);
	}
	else
	{
		if (nCount == 0)
			CCachedFlow::Clear();
		else
			EMERGENCY_EXIT("CCachedFlow can't truncate great 0 ");
	}
	return true;
}

bool CCachedFlow::PopFront()
{
	if (m_NodeQueue.size() == 0)
		return false;

	m_CacheList.PopFront(m_NodeQueue[0].size);
	m_NodeQueue.pop_front();
	m_nFirstID++;
	return true;
}

int CCachedFlow::GetFlowType()
{
	return FLOW_TYPE_CACHEDFLOW;
}
