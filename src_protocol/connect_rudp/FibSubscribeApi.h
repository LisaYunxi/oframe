// FibSubscribeApi.h: interface for the CFibSubscribeApi class.
//
// 20070905 xuzh 增加了UnSubscribe方法
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBSUBSCRIBEAPI_H__0A7C37A1_9D5D_45CE_8AB2_F37DD0CDDF47__INCLUDED_)
#define AFX_FIBSUBSCRIBEAPI_H__0A7C37A1_9D5D_45CE_8AB2_F37DD0CDDF47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Reactor.h"
#include "Subscriber.h"

class CFibSubscribeApi  
{
public:
	/** 获取本接口关联的网络地址
	*@return 本接口关联的网络地址
	*/
	virtual const char *GetAddress() = 0;

	/*订阅一个主题
	* @param pSubscriber 订阅者
	* @param dwSubscriberID 订阅者代码
	*/
	virtual void Subscribe(CSubscriber *pSubscriber, DWORD dwSubscriberID) = 0;

	/*订阅一个主题
	* @param pSubscriber 订阅者
	*/
	virtual void UnSubscribe(CSubscriber *pSubscriber) = 0;

	/**设置所管理的订阅者的CPN
	* @param nCommPhaseNo 要设置的新CPN
	*/
	virtual void SetSubCommPhaseNo(DWORD nCommPhaseNo) = 0;

	/**删除本接口
	*/
	virtual void Release() = 0;

	/**创建接口
	* @param pReactor 驱动反应器
	* @param nNodeID  节点标识
	* @return 创建出的接口
	*/
	static CFibSubscribeApi *CreateSubscribeApi(CReactor *pReactor, DWORD nNodeID, char *pszAddress);

protected:
	virtual ~CFibSubscribeApi(){};
};

#endif // !defined(AFX_FIBSUBSCRIBEAPI_H__0A7C37A1_9D5D_45CE_8AB2_F37DD0CDDF47__INCLUDED_)
