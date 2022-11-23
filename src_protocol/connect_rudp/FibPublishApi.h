// FibPublishApi.h: interface for the CFibPublishApi class.
//
// 20070905 xuzh 增加了CancelPublish方法
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBPUBLISHAPI_H__CA980195_08A4_4448_BD75_54E61FEE2B30__INCLUDED_)
#define AFX_FIBPUBLISHAPI_H__CA980195_08A4_4448_BD75_54E61FEE2B30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Reactor.h"
#include "XTPPackage.h"
#include "Subscriber.h"
#include "ReadOnlyFlow.h"

class CFibPublishApi
{
public:
	/** 获取本接口关联的网络地址
	*@return 本接口关联的网络地址
	*/
	virtual const char *GetAddress() = 0;

	/**创建一个消息
	* @return 创建出的消息
	*/	
	virtual CXTPPackage *CreateMessage() = 0;

	//-------------------发布订阅接口-------------------------------
	/**注册消息队列
	* @param pFlow 要注册的消息队列
	* @param nSubjectNo 要发布的主题
	*/
	virtual void RegisterFlow(CReadOnlyFlow *pFlow, DWORD nSubjectNo) = 0;

	/**取消发布主题
	* @param nSubjectNo 要取消发布的主题
	*/
	virtual void CancelPublish(DWORD nSubjectNo) = 0;

	/**删除本接口
	*/
	virtual void Release() = 0;

	/**创建接口
	* @param pReactor 驱动反应器
	* @param nNodeID  节点标识
	* @param pszAddress 发布的网络地址，格式为"protocol://address:port"
	* @return 创建出的接口
	*/
	static CFibPublishApi *CreatePublishApi(CReactor *pReactor, DWORD nNodeID, char *pszAddress);

protected:
	virtual ~CFibPublishApi(){};
};

#endif // !defined(AFX_FIBPUBLISHAPI_H__CA980195_08A4_4448_BD75_54E61FEE2B30__INCLUDED_)
