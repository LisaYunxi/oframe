// CompInitState.h: interface for the CCompInitState class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _COMPINITSTATE_H__
#define _COMPINITSTATE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CompState.h"

/**排队服务状态机的初始状态（Init）
*/
class CCompInitState : public CCompState
{
public:
	/**构造函数
	* @param pCommServer 排队服务
	*/
	CCompInitState(CCompServer *pCommServer);

	/**析构函数
	*/
	virtual ~CCompInitState();

	/**获取本状态的状态代码
	* @return 返回状态代码，即初始状态(INIT)
	*/
	virtual TCompStateID GetStateID();

	/**进入本状态时调用
	* @param nLastStateID 上一状态
	*/
	virtual void OnEntry(TCompStateID nLastStateID);

	/**处理仲裁机发出的包
	* @param pPackage 仲裁机发出的包
	* @remark 根据仲裁的指令进行状态切换
	*/
	virtual void OnGuardPackageReceived(CGuardPackage *pArbPackage);
};

#endif
