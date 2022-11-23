// TimerQueue.h: interface for the CTimerQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMERQUEUE_H__2CEE8EF5_9969_48A0_83C2_857FDF4D006A__INCLUDED_)
#define AFX_TIMERQUEUE_H__2CEE8EF5_9969_48A0_83C2_857FDF4D006A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "EventHandler.h"

/**CTimerQueue是一个定时器管理器的基类，定时器的管理可以使用多种方法，
*如堆、链表、数组等，不同的管理方法针不同情况下的运行效率不同，
*但都需要继承本接口
*/
class CTimerQueue  
{
public:
	/**为事件处理器注册一个定时器
	* @param pEventHandler 事件处理器 
	* @param nIDEvent 定时器的标示 
	* @param nElapse 定时器的时间间隔 毫秒
	*/
	virtual void RegisterTimer(CEventHandler *pEventHandler, int nIDEvent, int nElapse) = 0;

	/**注销事件处理器的定时器
	* @param pEventHandler 事件处理器 
	* @param nIDEvent 定时器的标示，=0 注销该事件处理器所有的定时器所
	*        >0 注销该事件处理器指定标识的定时器
	*/
	virtual void RemoveTimer(CEventHandler *pEventHandler, int nIDEvent) = 0;

	/**检查各定时器是否到期
	*@param nClock 当前时间（毫秒计）
	*/
	virtual void Expire(unsigned int nClock) = 0;
	
	
	/**add 检查各注册的定时器是否到期
	*@param nClock 当前时间（毫秒计）
	*/
	virtual bool CheckExpire(unsigned int nClock)=0;
	
protected:
	virtual ~CTimerQueue(){};
};

#endif // !defined(AFX_TIMERQUEUE_H__2CEE8EF5_9969_48A0_83C2_857FDF4D006A__INCLUDED_)

