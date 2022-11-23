#ifndef AFX_INDEXFLOW_H__
#define AFX_INDEXFLOW_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "Flow.h"
#include "Mutex.h"

class CIndexFlow : public CFlow
{
public:
	CIndexFlow(const char* pName = "", int nMaxSize = 16384);
	virtual ~CIndexFlow();

	//获取指定编号的对象
	// @param	id	对象序号
	// @param pObject 获取对象的缓冲区
	// @param length 缓冲区大小
	// @return 取出对象的长度
	virtual int Get(UF_INT8 id, void *pObject, int length);

	//向流中增加一个对象
	// @param nIndex 对象在下层流中的序号
	// @return 对象在流中的序号，小于0表示增加失败
	virtual UF_INT8 Append(UF_INT8 nIndex);

	//将流尾部的包截掉
	// @param nCount 保留的包的个数
	// @return true 操作成功； false 操作失败
	virtual bool Truncate(UF_INT8 nCount);

	virtual int GetFlowType() { return FLOW_TYPE_INDEXFLOW; }

protected:
	typedef deque<UF_INT8> CPackageIndex;
	CPackageIndex m_PackageIndex;
	int m_nMaxSize;
	//CRITICAL_VAR m_criticalVar;		//临界区变量 
	CRWLock m_RWLock;
};

#endif
