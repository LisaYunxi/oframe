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

/**CFlowSubscriber是一种主题的订阅者。它关联着一个流，将收到的
*  消息记入流中。
*/
class CSynFlowSubscriber : public CFlowSubscriber  
{
public:
	/**构造函数
	*@param pFlow 保存收到的消息的Flow
	*@param nSubscribeSubjectNo 要订阅的主题代码
	*/
	CSynFlowSubscriber(CFlow *pFlow, unsigned int nSubscribeSubjectNo):CFlowSubscriber(pFlow,nSubscribeSubjectNo)
	{
	}

	/**进入临界区
	*/
	virtual void EnterCritical()
	{
		m_Mutex.Lock();
	}
	
	/**离开临界区
	*/
	virtual void LeaveCritical()
	{
		m_Mutex.UnLock();
	}

protected:
	CMutex m_Mutex;						/**用于临界操作 */
};

#endif // !defined(AFX_FLOWSUBSCRIBER_H__3456D5BD_8E16_40BC_8BD4_E1A82BE75BF3__INCLUDED_)
