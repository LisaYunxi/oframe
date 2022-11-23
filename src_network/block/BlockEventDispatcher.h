// BlockEventDispatcher.h: interface for the CBlockEventDispatcher class.
// 20170711 yangyong 创建该文件
//////////////////////////////////////////////////////////////////////

#if !defined(__BLOCKEVENTDISPATCHER_H__)
#define __BLOCKEVENTDISPATCHER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"
#include "BlockEventHandler.h"
#include "BlockEventQueue.hpp"
#include "BlockTimerQueue.h"

enum SPEED_MODE
{
	SM_HIGHT = 0,	// 高速
	SM_MEDIUM,		// 中速
	SM_LOW,			// 低速
};

class CBlockEventDispatcher : public CThread  
{
public:
	CBlockEventDispatcher(SPEED_MODE sm);
	virtual ~CBlockEventDispatcher();

	/**注册一个定时类可运行对象
	* @param pEventHandler 定时器的指针 
	* @param nIDEvent 定时器的标示 
	* @param nElapse 定时器的时间间隔 毫秒
	*/
	void RegisterTimer(CBlockEventHandler *pEventHandler, int nIDEvent, int nElapse);

	/**从定时器链表中移去一个定时器对象
	* @param pEventHandler 定时器对象的指针 
	* @param nIDEvent 定时器在CReatctor的标示，=0 移去该定时器所有注册
	*        >0 移去该定时器指定注册
	*/
	void RemoveTimer(CBlockEventHandler *pEventHandler, int nIDEvent);
	
	/*向EventHandler发送异步事件
	* @param pEventHander 目标EventHandler
	* @param nEventID 事件ID
	* @param dwParam 双字型参数
	* @param pParam 无符号指针型参数
	*/
	bool PostEvent(CBlockEventHandler *pEventHandler, int nEventID, DWORD dwParam, void *pParam, release_func pReleaseFunc);

	/*向EventHandler发送同步事件
	* @param pEventHander 目标EventHandler
	* @param nEventID 事件ID
	* @param dwParam 双字型参数
	* @param pParam 无符号指针型参数
	*/
	int SendEvent(CBlockEventHandler *pEventHandler, int nEventID, DWORD dwParam, void *pParam);

	/**没有事件处理器处理该事件时，由Reactor本身处理这条事件,
	* @param nEventID 事件ID
	* @param dwParam 第一个参数
	* @param pParam 第二个参数
	* @return 处理结果
	*/
	virtual int HandleEvent(int nEventID, DWORD dwParam, void *pParam, release_func pReleaseFunc);
	
	/**清除一个失效的EventHandler发出的所有的事件
	* @param pEventHander 失效的EventHandler
	*/
	//inline void ClearEvent(CEventHandler *pEventHandler);
	/**CReactor开始工作
	*/
	virtual void Run();
	
	/**获取当前系统时间
	* @return 当前系统时间
	*/
	inline DWORD Time();

	/**获取当前系统时间的毫秒部分
	* @return 当前系统时间的毫秒部分
	*/
	inline DWORD GetMilTime();
	
	/**获取当前系统滴哒数
	*@return  当前系统滴哒数（以毫秒计)
	*/
	inline DWORD GetClock();
	
	/**终止程序主循环
	*/
	void Stop(DWORD nStopCode = 0);
	
	inline DWORD GetStopCode();
protected:
	/**检查定时器链表是否有到期事件，如有则执行定时处理器的OnTimer()方法
	*/
	void CheckTimer();
	
	/**获取当前系统时间
	*/
	inline void SyncTime();

	inline DWORD SyncMilTime(); // 获取当前的毫秒

	/**从事件队列中取出事件，分派给相应的事件处理器
	*/
	void DispatchEvents();

protected:
	bool m_bShouldRun;				/**< 循环运行标志 */
	DWORD m_nCurrMilTime;			/**< 当前系统时间毫秒*/
	DWORD m_nCurrTime;				/**< 当前系统时间秒*/
	DWORD m_nCurrClock;				/**< 当前系统滴哒数*/

	ConcurrentQueue<CBlockEvent> m_queueEvent;		/**< 事件队列*/
	CBlockTimerQueue *m_pTimerQueue;

	DWORD m_nStopCode;

	CMutex m_lockActive;

	DWORD m_nUpdateTime;  // 最新更新的时间

	SPEED_MODE m_speedMode;
};

inline DWORD CBlockEventDispatcher::Time()
{
	return m_nCurrTime;
}

inline DWORD CBlockEventDispatcher::GetMilTime()
{
	return m_nCurrMilTime;
}

inline DWORD CBlockEventDispatcher::GetClock()
{
	return m_nCurrClock;
}

inline void CBlockEventDispatcher::SyncTime()
{
#ifdef WIN32	
   struct _timeb timebuffer;
   _ftime( &timebuffer );
	m_nCurrTime = (DWORD)timebuffer.time;	
	m_nCurrMilTime = timebuffer.millitm;
	m_nCurrClock = m_nCurrTime*1000 + timebuffer.millitm;
#else
	struct timeval timeout;
  gettimeofday(&timeout, 0);
	m_nCurrTime = timeout.tv_sec;	
	m_nCurrMilTime = timeout.tv_usec/1000;
	m_nCurrClock = m_nCurrTime*1000 + timeout.tv_usec/1000;
#endif
}

inline DWORD CBlockEventDispatcher::SyncMilTime()
{
	DWORD dwRet = 0;
#ifdef WIN32	
   struct _timeb timebuffer;
   _ftime( &timebuffer );
	dwRet = timebuffer.millitm;
#else
	struct timeval timeout;
	gettimeofday(&timeout, 0);
	dwRet = timeout.tv_usec/1000;
#endif
	return dwRet;
}


inline DWORD CBlockEventDispatcher::GetStopCode()
{
	return m_nStopCode;
}

#endif // !defined(__BLOCKEVENTDISPATCHER_H__)
