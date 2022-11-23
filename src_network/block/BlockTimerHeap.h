// TimerHeap.h: interface for the CTimerHeap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__BLOCKTIMERHEAP_H__)
#define __BLOCKTIMERHEAP_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BlockTimerQueue.h"

struct CBlockTimerHeapNode
{
	CBlockEventHandler *pTimer;	/**<��ʱ�������ĵ�ָ��*/
	int nIDEvent;			/**<��ʱ�������ı�ʾ*/
	int nElapse	;			/**<��ʱʱ����*/
	DWORD nExpire;			/**<����ʱ��*/
};

struct CCompareTimerHeapNode : public binary_function<CBlockTimerHeapNode, CBlockTimerHeapNode, bool>
{
    bool operator()(const CBlockTimerHeapNode& x, const CBlockTimerHeapNode& y) const
	{
		return y.nExpire < x.nExpire;
	}
};

/**CTimerHeap�����öѣ�Heap��ʵ�ֵ�һ����ʱ��������
*/
class CBlockTimerHeap : public CBlockTimerQueue, public priority_queue<CBlockTimerHeapNode, vector<CBlockTimerHeapNode>, CCompareTimerHeapNode>  
{
public:
	CBlockTimerHeap(DWORD nClock);
	virtual ~CBlockTimerHeap();

	/**ע��һ����ʱ������ж���
	* @param pEventHandler ��ʱ����ָ�� 
	* @param nIDEvent ��ʱ���ı�ʾ 
	* @param nElapse ��ʱ����ʱ���� ����
	*/
	virtual void RegisterTimer(CBlockEventHandler *pEventHandler, int nIDEvent, int nElapse);

	/**�Ӷ�ʱ����������ȥһ����ʱ������
	* @param pEventHandler ��ʱ�������ָ�� 
	* @param nIDEvent ��ʱ����CReatctor�ı�ʾ��=0 ��ȥ�ö�ʱ������ע��
	*        >0 ��ȥ�ö�ʱ��ָ��ע��
	*/
	virtual void RemoveTimer(CBlockEventHandler *pEventHandler, int nIDEvent);

	/**����ע��Ķ�ʱ���Ƿ���
	*@param nClock ��ǰʱ�䣨����ƣ�
	*/
	virtual void Expire(DWORD nClock);
	
	/**add by zbz ����ע��Ķ�ʱ���Ƿ���
	*@param nClock ��ǰʱ�䣨����ƣ�
	*/
	virtual bool CheckExpire(DWORD nClock);
	
	virtual size_t Count() const;
	
private:
	/**ͬ��ʱ��
	*@param nClock ��ǰʱ�䣨����ƣ�
	*@remark ��Ҫ����ʱ�������DWORD�����ֻ���ò���50�죩
	*/
	void SyncTime(DWORD nClock);
private:
	DWORD m_nClock;
	DWORD m_nClockAlter;
};

#endif // !defined(__BLOCKTIMERHEAP_H__)
