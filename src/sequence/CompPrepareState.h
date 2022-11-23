// CompPrepareState.h: interface for the CCompPrepareState class
//////////////////////////////////////////////////////////////////////

#ifndef _COMPPREPARESTATE_H__
#define _COMPPREPARESTATE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CompState.h"

/**排队服务状态机的从机准备状态（Prepare）
*/
class CCompPrepareState : public CCompState  
{
public:
	/**构造函数
	* @param pCommServer 排队服务
	*/
	CCompPrepareState(CCompServer *pCommServer);

	/**析构函数
	*/
	virtual ~CCompPrepareState();

	/**获取本状态的状态代码
	* @return 返回状态代码，即从机准备状态(Prepare)
	*/
	virtual TCompStateID GetStateID();
	
	/**进入本状态时调用
	*/
	virtual void OnEntry(TCompStateID nLastStateID);

	/**处理心跳线送上来的包
	* @param pPackage 通过心跳线送上来的包
	*/
	virtual void OnHotlinePackageReceived(CHotlinePackage *pHotlinePackage);

	/**处理心跳线连接中断的事件
	* @param pSession 与另一排队服务相连的会话
	* @param ErrorCode 会话中断原因
	*/
	virtual void OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode);

	/**处理仲裁机发出的包
	* @param pPackage 仲裁机发出的包
	* @remark 根据仲裁的指令进行状态切换
	*/
	virtual void OnGuardPackageReceived(CGuardPackage *pArbPackage);

	/**处理与仲裁机连接中断的事件
	* @param pSession 与仲裁机相连的会话
	* @param ErrorCode 会话中断原因
	*/
	virtual void OnGuardChannelLost(CGuardSession *pSession, int ErrorCode);

};

#endif !defined(AFX_COMPPREPARESTATE_H__43A3B345_CBAC_4A2C_8EC4_ADC524725487__INCLUDED_)
