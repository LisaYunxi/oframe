#ifndef AFX_FLOWSUBSCRIBER_H__
#define AFX_FLOWSUBSCRIBER_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Subscriber.h"
#include "Flow.h"

/**CFlowSubscriber��һ������Ķ����ߡ���������һ���������յ���
*  ��Ϣ�������С�
*/
class CFlowSubscriber : public CSubscriber  
{
public:
	/**���캯��
	*@param pFlow �����յ�����Ϣ��Flow
	*@param nSubscribeSubjectNo Ҫ���ĵ��������
	*/
	CFlowSubscriber(CFlow *pFlow, unsigned int nSubscribeSubjectNo)
	{
		m_pFlow = pFlow;
		m_nSubjectNo = nSubscribeSubjectNo;
	}

	virtual ~CFlowSubscriber(){};
	
	/**��ȡ���ĵ��������
	*@return ���ĵ��������
	*/
	virtual unsigned int GetSubjectNo()
	{
		return m_nSubjectNo;
	}

	/**��ȡ���յ�����Ϣ����
	*@return ���յ�����Ϣ����
	*/
	virtual UF_INT8 GetReceivedCount()
	{
		return m_pFlow->GetCount();
	}

	/**�����յ���һ����Ϣ
	*@param pMessage �յ���һ����Ϣ
	*/
	virtual void HandleMessage(CXTPPackage *pMessage)
	{
		pMessage->Push(XTPHLEN);
		m_pFlow->Append(pMessage->Address(), pMessage->Length());
		pMessage->Pop(XTPHLEN);
	}

protected:
	CFlow *m_pFlow;						/**< �����յ�����Ϣ��Flow */
	unsigned int m_nSubjectNo;					/**< Ҫ���ĵ�������� */
};

#endif
