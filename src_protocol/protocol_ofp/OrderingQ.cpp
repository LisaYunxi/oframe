#include "public.h"
#include "OrderingQ.h"

struct CDataItem
{
	void *pData;
	int nDataLen;
	bool bDelivered;
};

COrderingQ::COrderingQ(int nMaxSlot, int nDataBlockSize):m_cacheList(nDataBlockSize)
{
	m_nMaxSlot = nMaxSlot;
	m_pSlot = new CDataItem *[m_nMaxSlot];
	m_nMaxDataItem = 2*m_nMaxSlot;
	m_pDataItem = new CDataItem[m_nMaxDataItem];
	Clear();
}

COrderingQ::~COrderingQ()
{
	//删除的时候，应当与 new 分配内存时一致，对数组加上 []
	delete [] m_pSlot;
	delete [] m_pDataItem;
}

void COrderingQ::Clear()
{
	memset(m_pSlot, 0, sizeof(CDataItem *)*m_nMaxSlot);
	memset(m_pDataItem, 0, sizeof(CDataItem)*m_nMaxDataItem);
	m_nSlotHead = 0;
	m_nDataItemHead = 0;
	m_nDataItemTail= 0;
	m_nStartIndex = 1;			//从1开始
}

bool COrderingQ::EnQueue(UF_INT8 nIndex, void *pData, int nDataLen)
{
	//如果新加入的序号不在slot范围内，则返回失败
	if (nIndex<m_nStartIndex || nIndex>=m_nStartIndex+m_nMaxSlot)
		return false;

	UF_INT8 n = m_nSlotHead+nIndex-m_nStartIndex;
	//int nSlotNum = n % m_nMaxSlot;
	int nSlotNum = fmod(n, m_nMaxSlot);
	CDataItem **pSlot = &m_pSlot[nSlotNum];
	//丢弃重复的数据
	if (*pSlot != NULL)
		return false;

	//将数据块加入cacheList,并加入dataitem
	CDataItem *pDataItem = &m_pDataItem[m_nDataItemTail];
	pDataItem->bDelivered = false;
	pDataItem->pData = m_cacheList.PushBack(pData, nDataLen);
	pDataItem->nDataLen = nDataLen;

	m_nDataItemTail ++;
    if (m_nDataItemTail >= m_nMaxDataItem)
		m_nDataItemTail = 0;
	
	//将dataitem加入slot
	*pSlot = pDataItem;
	return true;
}

void COrderingQ::DeQueue()
{
	CDataItem **pSlot = &m_pSlot[m_nSlotHead];
	if (*pSlot != NULL)
	{
		(*pSlot)->bDelivered = true;
		*pSlot = NULL;
	}
	
	m_nSlotHead ++;
    if (m_nSlotHead >= m_nMaxSlot)
		m_nSlotHead = 0;
	
	while (m_nDataItemHead != m_nDataItemTail)
	{
		CDataItem *pDataItem = &m_pDataItem[m_nDataItemHead];
		if (!pDataItem->bDelivered)
			break;
		m_cacheList.PopFront(pDataItem->nDataLen);
		
		m_nDataItemHead++;
		if (m_nDataItemHead >= m_nMaxDataItem)
			m_nDataItemHead = 0;
	}
	m_nStartIndex++;
}

void *COrderingQ::QueueFront(int &nDataLen)
{
	CDataItem **pSlot = &m_pSlot[m_nSlotHead];
	if ( *pSlot == NULL)
		return NULL;

	nDataLen = (*pSlot)->nDataLen;
	return (*pSlot)->pData;
}
