#ifndef _TIMERHEAP_H__
#define _TIMERHEAP_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "TimerQueue.h"

struct CTimerHeapNode
{
	CEventHandler *pTimer;	/**<��ʱ�������ĵ�ָ��*/
	int nIDEvent;			/**<��ʱ�������ı�ʾ*/
	int nElapse	;			/**<��ʱʱ����*/
	unsigned int nExpire;	/**<����ʱ��*/
};

//struct CCompareTimerHeapNode : public binary_function<CTimerHeapNode, CTimerHeapNode, bool>
//{
//   bool operator()(const CTimerHeapNode& x, const CTimerHeapNode& y) const
//	{
//		return y.nExpire < x.nExpire;
//	}
//};

struct CCompareTimerHeapNode
{
   bool operator()(const CTimerHeapNode& x, const CTimerHeapNode& y) const
	{
		return y.nExpire < x.nExpire;
	}
};

//CTimerHeap�����öѣ�Heap��ʵ�ֵ�һ����ʱ��������

class CTimerHeap : public CTimerQueue, public priority_queue<CTimerHeapNode, vector<CTimerHeapNode>, CCompareTimerHeapNode>  
{
public:
	CTimerHeap(time_t nClock);
	virtual ~CTimerHeap();

	//ע��һ����ʱ������ж���
	//@param pEventHandler ��ʱ����ָ�� 
	//@param nIDEvent ��ʱ���ı�ʾ 
	//@param nElapse ��ʱ����ʱ���� ����
	//
	virtual void RegisterTimer(CEventHandler *pEventHandler, int nIDEvent, int nElapse);

	//�Ӷ�ʱ����������ȥһ����ʱ������
	//@param pEventHandler ��ʱ�������ָ�� 
	//@param nIDEvent ��ʱ����CReatctor�ı�ʾ��=0 ��ȥ�ö�ʱ������ע��
	//       >0 ��ȥ�ö�ʱ��ָ��ע��
	//
	virtual void RemoveTimer(CEventHandler *pEventHandler, int nIDEvent);

	//����ע��Ķ�ʱ���Ƿ���
	//@param nClock ��ǰʱ�䣨����ƣ�
	virtual void Expire(unsigned int nClock);
	
	//����ע��Ķ�ʱ���Ƿ���
	//@param nClock ��ǰʱ�䣨����ƣ�
	virtual bool CheckExpire(unsigned int nClock);
	
	
private:
	//ͬ��ʱ��
	//@param nClock ��ǰʱ�䣨����ƣ�
	//@remark ��Ҫ����ʱ�������unsigned int�����ֻ���ò���50�죩
	void SyncTime(time_t nClock);
private:
	unsigned int m_nClock;
	time_t m_nClockAlter;
};

#endif