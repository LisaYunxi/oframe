#ifndef AFX_COMPFLOWFACTORY_H__
#define AFX_COMPFLOWFACTORY_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "Flow.h"
#include "CachedFlow.h"
#include "IndexFlow.h"
#include "HashMap.h"

/**CCompFlowFactory�����Ŷӷ������еķ����Ͷ��ĵ�����
* ������Ϊ2�ࣺ
*		1.ȷ����(Series)���Ѿ��źöӲ��־û�����������
*		2.δȷ����(PreSeries)��������ȷ����֮�ϣ���ȷ������İ�δȷ��
*/
class CCompFlowFactory 
{
public:
	CCompFlowFactory(CCachedFlow *pPreSeries);

	virtual ~CCompFlowFactory();

	//ȡδȷ����
	inline CCachedFlow *GetPreSeries();

	//��ȡ��־��
	//inline CFlow *GetSeries();

	//��δȷ�ϵı��Ĵ����е�����ɾ��
	//void Rollback();

private:
//	CFlow *m_pSeries;							/**< ȷ���� */
	CCachedFlow *m_pPreSeries;					/**< δȷ���� */
};

inline CCachedFlow *CCompFlowFactory::GetPreSeries()
{
	return m_pPreSeries;
}

//inline CFlow *CCompFlowFactory::GetSeries()
//{
//	return m_pSeries;
//}

#endif
