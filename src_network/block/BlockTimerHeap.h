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
	CBlockEventHandler *pTimer;	/**<定时处理器的的指针*/
	int nIDEvent;			/**<定时处理器的标示*/
	int nElapse	;			/**<定时时间间隔*/
	DWORD nExpire;			/**<到期时间*/
};

struct CCompareTimerHeapNode : public binary_function<CBlockTimerHeapNode, CBlockTimerHeapNode, bool>
{
    bool operator()(const CBlockTimerHeapNode& x, const CBlockTimerHeapNode& y) const
	{
		return y.nExpire < x.nExpire;
	}
};

/**CTimerHeap是利用堆（Heap）实现的一个定时器管理器
*/
class CBlockTimerHeap : public CBlockTimerQueue, public priority_queue<CBlockTimerHeapNode, vector<CBlockTimerHeapNode>, CCompareTimerHeapNode>  
{
public:
	CBlockTimerHeap(DWORD nClock);
	virtual ~CBlockTimerHeap();

	/**注册一个定时类可运行对象
	* @param pEventHandler 定时器的指针 
	* @param nIDEvent 定时器的标示 
	* @param nElapse 定时器的时间间隔 毫秒
	*/
	virtual void RegisterTimer(CBlockEventHandler *pEventHandler, int nIDEvent, int nElapse);

	/**从定时器链表中移去一个定时器对象
	* @param pEventHandler 定时器对象的指针 
	* @param nIDEvent 定时器在CReatctor的标示，=0 移去该定时器所有注册
	*        >0 移去该定时器指定注册
	*/
	virtual void RemoveTimer(CBlockEventHandler *pEventHandler, int nIDEvent);

	/**检查各注册的定时器是否到期
	*@param nClock 当前时间（毫秒计）
	*/
	virtual void Expire(DWORD nClock);
	
	/**add by zbz 检查各注册的定时器是否到期
	*@param nClock 当前时间（毫秒计）
	*/
	virtual bool CheckExpire(DWORD nClock);
	
	virtual size_t Count() const;
	
private:
	/**同步时间
	*@param nClock 当前时间（毫秒计）
	*@remark 需要考虑时间溢出（DWORD存毫秒只能用不到50天）
	*/
	void SyncTime(DWORD nClock);
private:
	DWORD m_nClock;
	DWORD m_nClockAlter;
};

#endif // !defined(__BLOCKTIMERHEAP_H__)
