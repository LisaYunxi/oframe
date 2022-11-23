// TimerQueue.h: interface for the CTimerQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__BLOCKTIMERQUEUE_H__)
#define __BLOCKTIMERQUEUE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BlockEventHandler.h"

/**CTimerQueue是一个定时器管理器的基类，定时器的管理可以使用多种方法，
*如堆、链表、数组等，不同的管理方法针不同情况下的运行效率不同，
*但都需要继承本接口
*/
class CBlockTimerQueue  
{
public:
	/**为事件处理器注册一个定时器
	* @param pEventHandler 事件处理器 
	* @param nIDEvent 定时器的标示 
	* @param nElapse 定时器的时间间隔 毫秒
	*/
	virtual void RegisterTimer(CBlockEventHandler *pEventHandler, int nIDEvent, int nElapse) = 0;

	/**注销事件处理器的定时器
	* @param pEventHandler 事件处理器 
	* @param nIDEvent 定时器的标示，=0 注销该事件处理器所有的定时器所
	*        >0 注销该事件处理器指定标识的定时器
	*/
	virtual void RemoveTimer(CBlockEventHandler *pEventHandler, int nIDEvent) = 0;

	/**检查各定时器是否到期
	*@param nClock 当前时间（毫秒计）
	*/
	virtual void Expire(DWORD nClock) = 0;
	
	
	/**add by zbz 检查各注册的定时器是否到期
	*@param nClock 当前时间（毫秒计）
	*/
	virtual bool CheckExpire(DWORD nClock)=0;
	
	virtual size_t Count() const = 0;

protected:
	virtual ~CBlockTimerQueue(){};
};

#endif // !defined(__BLOCKTIMERQUEUE_H__)

