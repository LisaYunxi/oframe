// TimerQueue.h: interface for the CTimerQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__BLOCKTIMERQUEUE_H__)
#define __BLOCKTIMERQUEUE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BlockEventHandler.h"

/**CTimerQueue��һ����ʱ���������Ļ��࣬��ʱ���Ĺ������ʹ�ö��ַ�����
*��ѡ���������ȣ���ͬ�Ĺ������벻ͬ����µ�����Ч�ʲ�ͬ��
*������Ҫ�̳б��ӿ�
*/
class CBlockTimerQueue  
{
public:
	/**Ϊ�¼�������ע��һ����ʱ��
	* @param pEventHandler �¼������� 
	* @param nIDEvent ��ʱ���ı�ʾ 
	* @param nElapse ��ʱ����ʱ���� ����
	*/
	virtual void RegisterTimer(CBlockEventHandler *pEventHandler, int nIDEvent, int nElapse) = 0;

	/**ע���¼��������Ķ�ʱ��
	* @param pEventHandler �¼������� 
	* @param nIDEvent ��ʱ���ı�ʾ��=0 ע�����¼����������еĶ�ʱ����
	*        >0 ע�����¼�������ָ����ʶ�Ķ�ʱ��
	*/
	virtual void RemoveTimer(CBlockEventHandler *pEventHandler, int nIDEvent) = 0;

	/**������ʱ���Ƿ���
	*@param nClock ��ǰʱ�䣨����ƣ�
	*/
	virtual void Expire(DWORD nClock) = 0;
	
	
	/**add by zbz ����ע��Ķ�ʱ���Ƿ���
	*@param nClock ��ǰʱ�䣨����ƣ�
	*/
	virtual bool CheckExpire(DWORD nClock)=0;
	
	virtual size_t Count() const = 0;

protected:
	virtual ~CBlockTimerQueue(){};
};

#endif // !defined(__BLOCKTIMERQUEUE_H__)

