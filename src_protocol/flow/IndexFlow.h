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

	//��ȡָ����ŵĶ���
	// @param	id	�������
	// @param pObject ��ȡ����Ļ�����
	// @param length ��������С
	// @return ȡ������ĳ���
	virtual int Get(UF_INT8 id, void *pObject, int length);

	//����������һ������
	// @param nIndex �������²����е����
	// @return ���������е���ţ�С��0��ʾ����ʧ��
	virtual UF_INT8 Append(UF_INT8 nIndex);

	//����β���İ��ص�
	// @param nCount �����İ��ĸ���
	// @return true �����ɹ��� false ����ʧ��
	virtual bool Truncate(UF_INT8 nCount);

	virtual int GetFlowType() { return FLOW_TYPE_INDEXFLOW; }

protected:
	typedef deque<UF_INT8> CPackageIndex;
	CPackageIndex m_PackageIndex;
	int m_nMaxSize;
	//CRITICAL_VAR m_criticalVar;		//�ٽ������� 
	CRWLock m_RWLock;
};

#endif
