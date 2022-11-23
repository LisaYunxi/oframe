#ifndef AFX_FLOW_H__
#define AFX_FLOW_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "ReadFlow.h"
#include "public.h"


class  CFlow : public CReadFlow
{
public:
	CFlow()
	{
		m_nFirstID = UF_INT8_ZERO;
		m_nCount = UF_INT8_ZERO;
		m_pEventHandler = NULL;
		m_pUnderFlow = NULL;
		m_bAutoSync = true;
	}

	virtual ~CFlow()
	{
	}

	/**将流尾部的对象截掉
	*@param nCount 保留的对象的个数
	*@return true 操作成功； false 操作失败
	*/
	virtual bool Truncate(UF_INT8 nCount) { return true; }

	/**向流中增加一个对象
	*@param	pObject 要增加的对象
	*@param length 增加数据的长度
	*@return 对象在流中的序号，小于0表示增加失败
	*/
	virtual UF_INT8 Append(void *pObject, int length) { return -1; }

	virtual UF_INT8 GetCount()
	{
		return m_nCount;
	}

	virtual UF_INT8 GetUnderCount()
	{
		if (m_pUnderFlow == NULL)
			return -1;
		return m_pUnderFlow->GetCount();
	}

	virtual UF_INT8 GetFirstID()
	{
		return m_nFirstID;
	}

	const char* GetName()
	{
		return m_sName.c_str();
	}

	virtual bool AttachObserver(CFlowCallback* pEventHandler)
	{
		m_pEventHandler = pEventHandler;
		return true;
	}

	virtual CFlowCallback* DeatchObserver()
	{
		CFlowCallback* pCallback = m_pEventHandler;
		m_pEventHandler = NULL;
		return pCallback;
	}

	virtual CFlowCallback* GetObserver()
	{
		return m_pEventHandler;
	}

	virtual bool AttachUnderFlow(CFlow *pFlow)
	{
		if (pFlow == NULL)
			return false;
		m_pUnderFlow = pFlow;
		return true;
	}

	virtual bool SyncUnderFlow(UF_INT8 id)
	{
		return true;
	}

	virtual CFlow* DeatchUnderFlow()
	{
		CFlow *pFlow = m_pUnderFlow;
		m_pUnderFlow = NULL;
		return pFlow;
	}

	virtual CFlow* GetUnderFlow()
	{
		return m_pUnderFlow;
	}

	virtual void* GetPointer(UF_INT8 id, int& length)
	{
		return NULL;
	}

protected:
	CFlowCallback *m_pEventHandler;
	CFlow* m_pUnderFlow;
	volatile UF_INT8 m_nFirstID;	// 第一个报文的id号
	volatile UF_INT8 m_nCount;		// 已经存在的包总个数
	bool m_bAutoSync;				// 在加入一个包是，是否立刻同步到挂接的流
	string m_sName;
};

#endif
