// FibClientApi.h: interface for the CFibClientApi class.
//
// 20070822	xuzh	配合SessionFactory修改，支持随机连接服务器
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBCLIENTAPI_H__16C39C84_4AAF_466A_AD14_7D8C09CC10B6__INCLUDED_)
#define AFX_FIBCLIENTAPI_H__16C39C84_4AAF_466A_AD14_7D8C09CC10B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPPackage.h"
#include "Reactor.h"

class CResponseHandler
{
public:
	virtual void OnResponse(CXTPPackage *pPackage) = 0;
	virtual ~CResponseHandler(){};
};

class CFibClientApi  
{
public:
	//-------------------配置接口函数-------------------------------
	/**获取节点标识
	*/
	virtual DWORD GetNodeID() = 0;

	/**注册请求/应答模式服务器地址
	* 可以注册多个服务器地址，API会逐个尝试连接
	* @param nServerID 提供服务的服务器标识
	* @param pszAddress 要连接的服务器地址，格式为"protocol://address:port"
	* @param pRspHandler 应答处理对象
	*/
	virtual void RegisterServer(DWORD nServerID, char *pszAddress, CResponseHandler *pRspHandler) = 0;

	/**启动接口连接
	*/
	virtual void Start() = 0;
	//-------------------消息接口函数-------------------------------
	/**创建一个消息
	* @return 创建出的消息
	*/
	virtual CXTPPackage *CreateMessage() = 0;
	
	//-------------------请求应答接口函数-------------------------------
	/**发送一个请求
	* @param nServiceID 服务端口
	* @param pMessage 请求消息
	* @return FIB_RESULT_OK 请求成功发出
	*         <0 发送失败
	*/
	virtual int Request(DWORD nServerID, CXTPPackage *pMessage) = 0;

	/**删除本接口
	*/
	virtual void Release() = 0;

	//-------------------创建接口函数-------------------------------
	/**创建接口
	* @param pReactor 驱动反应器
	* @param nNodeID  节点标识
	* @return 创建出的接口
	*/
	static CFibClientApi *CreateClientApi(CReactor *pReactor, DWORD nNodeID);

protected:
	virtual ~CFibClientApi(){};
};

#endif // !defined(AFX_FIBCLIENTAPI_H__16C39C84_4AAF_466A_AD14_7D8C09CC10B6__INCLUDED_)
