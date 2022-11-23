// EventHandler.h: interface for the CPipeEventHandler class .
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPipeEventHandler_H__D0DAF147_AD16_4C5C_9393_78E357E0A93E__INCLUDED_)
#define AFX_CPipeEventHandler_H__D0DAF147_AD16_4C5C_9393_78E357E0A93E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "EventHandler.h"
//#include "CachedFlow.h"
#include "public.h"

class CSelectReactor;

/**
* CPipeEventHandler是一个通知处理对象，主要用来触发CSelectReactor,防治其深度睡眠，
* 从而导致其不能及时处理来自队列的任务
*/
class CPipeEventHandler: public CEventHandler,public CFlowCallback
{
public:
	/**构造函数
	* @param pReactor 当前运行对象归属的引擎 
	*/
	CPipeEventHandler(CReactor *pReactor);
	
	/**析构函数
	*/
	virtual ~CPipeEventHandler();
	
	/**处理IO操作的读操作，由CReactor调用
	* @return 保留
	*/
	virtual int HandleInput();
	
	/**获取读、写socket，供select使用
	* @param pReadId   输出参数，返回读所使用的socket，为0表示不需要读
	* @param pWriteId  输出参数，返回写所使用的socket，为0表示不需要写 
	*/
	virtual void GetIds(int *pReadId, int *pWriteId);
	
	/**产生通知
	*/
	virtual void Notify();
	
	/*获取毫秒时间
	*/
	unsigned int GettMilSec();

protected:
	void CreatePipe(int &p0, int &p1);

	int CreateTmpServer(int &port);
	int CreateTmpClient(int port, int &p1);

protected:
	CReactor *m_pDestReactor;
	int             m_nReadPipe;
	int             m_nWritePipe;
	unsigned int 		m_nPreClock;
	unsigned int 		m_nPerCnt;
	bool			m_SendFlag;
	char 			m_buffer[10];
	
	volatile unsigned int     m_nWriteCnt;
	volatile unsigned int     m_nReadCnt;

//	int	m_nLocalPivatePort;  //WIN32专用
};

#endif !defined(AFX_EVENTHANDLER_H__D0DAF147_AD16_4C5C_9393_78E357E0A93E__INCLUDED_)
