#ifndef AFX_FLOWSUBSCRIBER_H__
#define AFX_FLOWSUBSCRIBER_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Subscriber.h"
#include "Flow.h"

/**CFlowSubscriber是一种主题的订阅者。它关联着一个流，将收到的
*  消息记入流中。
*/
class CFlowSubscriber : public CSubscriber  
{
public:
	/**构造函数
	*@param pFlow 保存收到的消息的Flow
	*@param nSubscribeSubjectNo 要订阅的主题代码
	*/
	CFlowSubscriber(CFlow *pFlow, unsigned int nSubscribeSubjectNo)
	{
		m_pFlow = pFlow;
		m_nSubjectNo = nSubscribeSubjectNo;
	}

	virtual ~CFlowSubscriber(){};
	
	/**获取订阅的主题代码
	*@return 订阅的主题代码
	*/
	virtual unsigned int GetSubjectNo()
	{
		return m_nSubjectNo;
	}

	/**获取已收到的消息个数
	*@return 已收到的消息个数
	*/
	virtual UF_INT8 GetReceivedCount()
	{
		return m_pFlow->GetCount();
	}

	/**处理收到的一个消息
	*@param pMessage 收到的一个消息
	*/
	virtual void HandleMessage(CXTPPackage *pMessage)
	{
		pMessage->Push(XTPHLEN);
		m_pFlow->Append(pMessage->Address(), pMessage->Length());
		pMessage->Pop(XTPHLEN);
	}

protected:
	CFlow *m_pFlow;						/**< 保存收到的消息的Flow */
	unsigned int m_nSubjectNo;					/**< 要订阅的主题代码 */
};

#endif
