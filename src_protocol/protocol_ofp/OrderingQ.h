#ifndef _ORDERINGQ_H__
#define _ORDERINGQ_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CacheList.h"

struct CDataItem;

class COrderingQ  
{
public:
	COrderingQ(int nMaxSlot, int nDataBlockSize = 40000);
	virtual ~COrderingQ();
	
	bool EnQueue(UF_INT8 nIndex, void *pData, int nDataLen);

	void DeQueue();

	void *QueueFront(int &nDataLen);

	void Clear();

	inline UF_INT8 GetStartIndex();
	
	inline void SetStartIndex(UF_INT8 nStartIndex);
	
	inline bool IsEmpty();
protected:
	CDataItem **m_pSlot;
	int m_nMaxSlot;
	int m_nSlotHead;

	CDataItem *m_pDataItem;
	int m_nMaxDataItem;
	int m_nDataItemHead;
	int m_nDataItemTail;
	
	CCacheList m_cacheList;

	UF_INT8 m_nStartIndex;
};

inline UF_INT8 COrderingQ::GetStartIndex()
{
	return m_nStartIndex;
}

inline void COrderingQ::SetStartIndex(UF_INT8 nStartIndex)
{
	Clear();
	m_nStartIndex = nStartIndex;
}

inline bool COrderingQ::IsEmpty()
{
	return m_nDataItemHead == m_nDataItemTail;
}

#endif
