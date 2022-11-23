// CompSecondaryState.h: interface for the CCompSecondaryState class.
//////////////////////////////////////////////////////////////////////

#ifndef _COMPSECONDARYSTATE_H__
#define _COMPSECONDARYSTATE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CompState.h"

/**排队服务状态机的从机状态（Secondary）
*/
class CCompSecondaryState : public CCompState  
{
public:
	/**构造函数
	* @param pCommServer 排队服务
	*/
	CCompSecondaryState(CCompServer *pCompServer);
	
	/**析构函数
	*/
	virtual ~CCompSecondaryState();

	/**获取本状态的状态代码
	* @return 返回状态代码，即从机状态(Secondary)
	*/
	virtual TCompStateID GetStateID();
	
	virtual void OnEntry(TCompStateID nLastStateID);

	/**处理心跳线送上来的包
	*/
	virtual void OnHotlinePackageReceived(CHotlinePackage *pHotlinePackage);

	/**处理心跳线连接中断的事件
	*/
	virtual void OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode);

	/**处理仲裁机送上来的包
	*/
	virtual void OnGuardPackageReceived(CGuardPackage *pArbPackage);

	/**处理与仲裁机连接中断的事件
	*/
	virtual void OnGuardChannelLost(CGuardSession *pSession, int ErrorCode);

};

#endif
