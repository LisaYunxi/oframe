// Reactor.h: interface for the CReactor class .
//
// 20170111 yangyong 创建该文件
//////////////////////////////////////////////////////////////////////

#if !defined(__BLOCKREACTOR_H__)
#define __BLOCKREACTOR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BlockEventDispatcher.h"

/**CReactor是建立在一个独立线程基础上的运行引擎，它通过select调用
*来发现各种事件。在需要的时候它调用已经注册的CEventHandler对象，其
*主要方法是Run()。对于IO类CEventHandler对象它包存在一个list中，
*对于定时器类CEventHandler对象它包存在一个链表中。
*/
class  CBlockReactor :public CBlockEventDispatcher 
{
public:
	/**构造函数
	*/
	CBlockReactor(SPEED_MODE sm);
	
	/**析构函数
	*/
	virtual ~CBlockReactor();
	
	/**注册一个IO类可运行对象
	* @param pEventHandler 可运行对象的指针 
	*/
	virtual void RegisterIO(CBlockEventHandler *pEventHandler);
	
	/**从对象list中移去一个IO类可运行对象
	* @param pEventHandler 可运行对象的指针 
	*/
	virtual void RemoveIO(CBlockEventHandler *pEventHandler);
	
	/**设置特殊事件标志
	* @param bFlag true:有特殊事件发生 false:无特殊事件发生
	*/
	virtual void SetSpeciaFlag(bool bFlag);
	
	virtual bool GetSpeciaFlag(void);
	
protected:
	/**处理其它任务，派生类可继承来处理自已的一些需要反复执行的工作
	* @return true 本次任务没有全部完成
	* @return false 本次任务全部完成
	*/
	virtual bool HandleOtherTask();
	
protected:
	typedef list<CBlockEventHandler *> CEventHandlerList; /**< 定义了事件处理器列表 */
	CEventHandlerList m_IOList;	/**< 存放IO事件处理器的列表*/
	bool m_bIOListHasNull;          /**< IO事件处理器列表的内容标志*/
	bool m_bSpeciaFlag;             /**<特殊事件标志*/
};

#endif // !defined(__BLOCKREACTOR_H__)
