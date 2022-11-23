// Reactor.h: interface for the CReactor class .
//
// 20170111 yangyong �������ļ�
//////////////////////////////////////////////////////////////////////

#if !defined(__BLOCKREACTOR_H__)
#define __BLOCKREACTOR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BlockEventDispatcher.h"

/**CReactor�ǽ�����һ�������̻߳����ϵ��������棬��ͨ��select����
*�����ָ����¼�������Ҫ��ʱ���������Ѿ�ע���CEventHandler������
*��Ҫ������Run()������IO��CEventHandler������������һ��list�У�
*���ڶ�ʱ����CEventHandler������������һ�������С�
*/
class  CBlockReactor :public CBlockEventDispatcher 
{
public:
	/**���캯��
	*/
	CBlockReactor(SPEED_MODE sm);
	
	/**��������
	*/
	virtual ~CBlockReactor();
	
	/**ע��һ��IO������ж���
	* @param pEventHandler �����ж����ָ�� 
	*/
	virtual void RegisterIO(CBlockEventHandler *pEventHandler);
	
	/**�Ӷ���list����ȥһ��IO������ж���
	* @param pEventHandler �����ж����ָ�� 
	*/
	virtual void RemoveIO(CBlockEventHandler *pEventHandler);
	
	/**���������¼���־
	* @param bFlag true:�������¼����� false:�������¼�����
	*/
	virtual void SetSpeciaFlag(bool bFlag);
	
	virtual bool GetSpeciaFlag(void);
	
protected:
	/**������������������ɼ̳����������ѵ�һЩ��Ҫ����ִ�еĹ���
	* @return true ��������û��ȫ�����
	* @return false ��������ȫ�����
	*/
	virtual bool HandleOtherTask();
	
protected:
	typedef list<CBlockEventHandler *> CEventHandlerList; /**< �������¼��������б� */
	CEventHandlerList m_IOList;	/**< ���IO�¼����������б�*/
	bool m_bIOListHasNull;          /**< IO�¼��������б�����ݱ�־*/
	bool m_bSpeciaFlag;             /**<�����¼���־*/
};

#endif // !defined(__BLOCKREACTOR_H__)
