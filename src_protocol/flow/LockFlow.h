#ifndef AFX_LOCKFLOW_H__
#define AFX_LOCKFLOW_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Flow.h"

//CThreadSafeFlow��һ���̰߳�ȫ���������Խ�����������������
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

	//��ȡ�����Ѿ��еĶ���ĸ���
	// @return ����ĸ���
	virtual UF_INT8 GetCount()
	{
		ENTER_CRITICAL(m_criticalVar);
		UF_INT8 nCount = m_pUnderFlow->GetCount();
		LEAVE_CRITICAL(m_criticalVar);
		return nCount;
	}

	//��ȡָ����ŵĶ���
	// @param	id	�������
	// @param pObject ��ȡ����Ļ�����
	// @param length ��������С
	// @return ȡ������ĳ���
	virtual int Get(UF_INT8 id, void *pObject, int length)
	{
		ENTER_CRITICAL(m_criticalVar);
		int nLength = m_pUnderFlow->Get(id, pObject, length);
		LEAVE_CRITICAL(m_criticalVar);
		return nLength;
	}

	//����β���Ķ���ص�
	// @param nCount �����Ķ���ĸ���
	// @return true �����ɹ��� false ����ʧ��
	virtual bool Truncate(UF_INT8 nCount)
	{
		ENTER_CRITICAL(m_criticalVar);
		bool bResult = m_pUnderFlow->Truncate(nCount);
		LEAVE_CRITICAL(m_criticalVar);
		return bResult;
	}

	//����������һ������
	// @param	pObject Ҫ���ӵĶ���
	// @param length �������ݵĳ���
	// @return ���������е���ţ�С��0��ʾ����ʧ��
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
