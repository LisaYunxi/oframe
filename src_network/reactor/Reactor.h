#ifndef AFX_REACTOR_H__
#define AFX_REACTOR_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "EventDispatcher.h"

//CReactor�ǽ�����һ�������̻߳����ϵ��������棬��ͨ��select����
//�����ָ����¼�������Ҫ��ʱ���������Ѿ�ע���CEventHandler������
//��Ҫ������Run()������IO��CEventHandler������������һ��list�У�
//���ڶ�ʱ����CEventHandler������������һ�������С�

class  CReactor :public CEventDispatcher 
{
public:
	CReactor();
	virtual ~CReactor();
	
	//ע��һ��IO������ж���
	//param pEventHandler �����ж����ָ�� 
	virtual void RegisterIO(CEventHandler *pEventHandler);
	
	//�Ӷ���list����ȥһ��IO������ж���
	//@param pEventHandler �����ж����ָ�� 
	virtual void RemoveIO(CEventHandler *pEventHandler);
	
	//���������¼���־
	//@param bFlag true:�������¼����� false:�������¼�����
	virtual void SetSpeciaFlag(bool bFlag);
	virtual bool GetSpeciaFlag(void);
	
protected:
	//������������������ɼ̳����������ѵ�һЩ��Ҫ����ִ�еĹ���
	//@return true ��������û��ȫ�����
	//@return false ��������ȫ�����
	virtual bool HandleOtherTask();
	
protected:
	typedef list<CEventHandler *> CEventHandlerList; //�������¼��������б�
	CEventHandlerList m_IOList;		//���IO�¼����������б�
	bool m_bIOListHasNull;          //IO�¼��������б�����ݱ�־
	bool m_bSpeciaFlag;             //�����¼���־
};

#endif