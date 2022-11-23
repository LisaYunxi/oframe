#ifndef AFX_LOCKFLOW_H__
#define AFX_LOCKFLOW_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Flow.h"

//CThreadSafeFlow是一个线程安全的流，可以建立在任意流的上面
class CThreadSafeFlow : public CFlow
{
public:
	CThreadSafeFlow(CFlow *pUnderFlow)
	{
		m_pUnderFlow = pUnderFlow;
		INIT_CRITICAL_VAR(m_criticalVar);
	}
	virtual ~CThreadSafeFlow()
	{
		delete m_pUnderFlow;
		DELETE_CRITICAL_VAR(m_criticalVar)
	}

	//获取流中已经有的对象的个数
	// @return 对象的个数
	virtual UF_INT8 GetCount()
	{
		ENTER_CRITICAL(m_criticalVar);
		UF_INT8 nCount = m_pUnderFlow->GetCount();
		LEAVE_CRITICAL(m_criticalVar);
		return nCount;
	}

	//获取指定编号的对象
	// @param	id	对象序号
	// @param pObject 获取对象的缓冲区
	// @param length 缓冲区大小
	// @return 取出对象的长度
	virtual int Get(UF_INT8 id, void *pObject, int length)
	{
		ENTER_CRITICAL(m_criticalVar);
		int nLength = m_pUnderFlow->Get(id, pObject, length);
		LEAVE_CRITICAL(m_criticalVar);
		return nLength;
	}

	//将流尾部的对象截掉
	// @param nCount 保留的对象的个数
	// @return true 操作成功； false 操作失败
	virtual bool Truncate(UF_INT8 nCount)
	{
		ENTER_CRITICAL(m_criticalVar);
		bool bResult = m_pUnderFlow->Truncate(nCount);
		LEAVE_CRITICAL(m_criticalVar);
		return bResult;
	}

	//向流中增加一个对象
	// @param	pObject 要增加的对象
	// @param length 增加数据的长度
	// @return 对象在流中的序号，小于0表示增加失败
	virtual UF_INT8 Append(void *pObject, int length)
	{
		ENTER_CRITICAL(m_criticalVar);
		int nId = m_pUnderFlow->Append(pObject, length);
		LEAVE_CRITICAL(m_criticalVar);
		return nId;
	}

	virtual int GetFlowType() { return FLOW_TYPE_LOCKFLOW; }

private:
	CRITICAL_VAR m_criticalVar;
};

#endif
