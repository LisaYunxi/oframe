#ifndef AFX_COMPSTATE_H__
#define AFX_COMPSTATE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CompServer.h"

// 排队服务状态机的状态基类
class CCompState
{
public:
	CCompState(CCompServer *pCommServer);
	virtual ~CCompState();

	//获取本状态的状态代码
	// @return 返回状态代码
	virtual TCompStateID GetStateID() = 0;

	//进入本状态时调用
	// @param nLastStateID 上一状态
	virtual void OnEntry(TCompStateID nLastStateID) = 0;

	//处理心跳线送上来的包
	// @param pPackage 通过心跳线送上来的包
	virtual void OnHotlinePackageReceived(CHotlinePackage *pHotlinePackage);

	//处理心跳线连接中断的事件
	// @param pSession 与另一排队服务相连的会话
	// @param ErrorCode 会话中断原因
	virtual void OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode);

	//处理仲裁机发出的包
	// @param pPackage 仲裁机发出的包
	// @remark 根据仲裁的指令进行状态切换
	virtual void OnGuardPackageReceived(CGuardPackage *pArbPackage);

	//处理与仲裁机连接中断的事件
	// @param pSession 与仲裁机相连的会话
	// @param ErrorCode 会话中断原因
	virtual void OnGuardChannelLost(CGuardSession *pSession, int ErrorCode);

protected:
	CCompServer *m_pCommServer;			// 排队服务
};

#endif
