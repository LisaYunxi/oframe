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

	/**����β���Ķ���ص�
	*@param nCount �����Ķ���ĸ���
	*@return true �����ɹ��� false ����ʧ��
	*/
	virtual bool Truncate(UF_INT8 nCount) { return true; }

	/**����������һ������
	*@param	pObject Ҫ���ӵĶ���
	*@param length �������ݵĳ���
	*@return ���������е���ţ�С��0��ʾ����ʧ��
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
	volatile UF_INT8 m_nFirstID;	// ��һ�����ĵ�id��
	volatile UF_INT8 m_nCount;		// �Ѿ����ڵİ��ܸ���
	bool m_bAutoSync;				// �ڼ���һ�����ǣ��Ƿ�����ͬ�����ҽӵ���
	string m_sName;
};

#endif
