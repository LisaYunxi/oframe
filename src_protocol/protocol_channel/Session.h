#ifndef AFX_SESSION_H__
#define AFX_SESSION_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "EventHandler.h"
#include "ChannelProtocol.h"

class CSession;

class CSessionCallback
{
public:
	virtual void OnSessionConnected(CSession *pSession) = 0;
	virtual void OnSessionDisconnected(CSession *pSession, int nReason) = 0;
	virtual void OnSessionWarning(CSession *pSession, int nReason, int nParam) = 0;
	virtual ~CSessionCallback(){};
};


/**是一个存放协议栈的容器，容器的最下层协议是CSession。
该类不能实例化，一般作为面向TCP连接的协议栈的基类 */
class  CSession : public CEventHandler  
{
public:
	CSession(CReactor *pReactor,CChannel *pChannel,int MaxPackageSize);
	virtual ~CSession();
	
	inline unsigned int GetSessionID();
//	inline long GetSessionID();
	//为防止不同的线程中SessionID相同
	//@param nID 改变ID的高位
	void ReSetSessionID(int nThreadID);

	//当网络有可读数据包时被调用
	//@return 0   成功
	//@return <>0 失败
	virtual int HandleInput();
	
	//当Reactor发现需要向网络发送数据包时被调用
	//@return >=0   成功
	//@return <0    失败
	virtual int HandleOutput();
	
	//处理一条消息
	//@param nEventID 消息ID
	//@param dwParam 第一个参数
	//@param pParam 第二个参数
	//@return 处理结果
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);

	//取最下层协议绑定的Channel
	//@return 最下层协议绑定的Channel
	inline CChannel *GetChannel();

	//断开网络连接
	//@param ErrorCode  断路的原因
	virtual void Disconnect(int nErrorCode);

	//获取读、写socket，供select使用
	//@param pReadId   输出参数，返回读所使用的socket，为0表示不需要读
	//@param pWriteId  输出参数，返回写所使用的socket，为0表示不需要写 
	virtual void GetIds(int *pReadId, int *pWriteId);

	inline void RegisterSessionCallback(CSessionCallback *pSessionCallback);

	//挂起/恢复读操作
	//param bSuspend true 挂起读操作    false恢复读操作 
	void SuspendRead(bool bSuspend);

	//检查Channel是否有要写出的内容
	//@return true 有要写出的内容, false 没有要写出的内容
	bool IsChannelWriteBusy();

protected:	
	//网络断路消息处理程序，每当发现网络断路时叫用此函数
	//@param ErrorCode  断路的原因
	virtual void OnChannelLost(int nErrorCode);
protected:
	unsigned int m_nSessionID;

	CChannel *m_pChannel;			//容器捆绑的Channel
	CChannelProtocol *m_pChannelProtocol;	//容器内的底层协议
	CSessionCallback *m_pSessionCallback;

	bool m_bSuspendRead;
};

inline CChannel *CSession::GetChannel()
{
	return m_pChannel;
}

inline void CSession::RegisterSessionCallback(CSessionCallback *pSessionCallback)
{
	m_pSessionCallback = pSessionCallback;
}

inline unsigned int CSession::GetSessionID()
//inline long CSession::GetSessionID()
{
	return m_nSessionID;
}

#endif
