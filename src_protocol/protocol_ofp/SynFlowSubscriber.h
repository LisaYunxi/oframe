// FlowSubscriber.h: interface for the CFlowSubscriber class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNFLOWSUBSCRIBER_H__3456D5BD_8E16_40BC_8BD4_E1A82BE75BF3__INCLUDED_)
#define AFX_SYNFLOWSUBSCRIBER_H__3456D5BD_8E16_40BC_8BD4_E1A82BE75BF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "FlowSubscriber.h"
#include "Flow.h"

/**CFlowSubscriber��һ������Ķ����ߡ���������һ���������յ���
*  ��Ϣ�������С�
*/
class CSynFlowSubscriber : public CFlowSubscriber  
{
public:
	/**���캯��
	*@param pFlow �����յ�����Ϣ��Flow
	*@param nSubscribeSubjectNo Ҫ���ĵ��������
	*/
	CSynFlowSubscriber(CFlow *pFlow, unsigned int nSubscribeSubjectNo):CFlowSubscriber(pFlow,nSubscribeSubjectNo)
	{
	}

	/**�����ٽ���
	*/
	virtual void EnterCritical()
	{
		m_Mutex.Lock();
	}
	
	/**�뿪�ٽ���
	*/
	virtual void LeaveCritical()
	{
		m_Mutex.UnLock();
	}

protected:
	CMutex m_Mutex;						/**�����ٽ���� */
};

#endif // !defined(AFX_FLOWSUBSCRIBER_H__3456D5BD_8E16_40BC_8BD4_E1A82BE75BF3__INCLUDED_)
