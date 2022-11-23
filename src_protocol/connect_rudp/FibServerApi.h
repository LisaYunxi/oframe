// FibServerApi.h: interface for the CFibServerApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBSERVERAPI_H__022BDC62_A62D_46E8_86E7_E725BD80D558__INCLUDED_)
#define AFX_FIBSERVERAPI_H__022BDC62_A62D_46E8_86E7_E725BD80D558__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPPackage.h"
#include "Reactor.h"

class CService
{
public:
	virtual void OnRequest(CXTPPackage *pRequest, DWORD nSessionID) = 0;
	virtual void OnSessionConnected(DWORD nSessionID) = 0;
	virtual void OnSessionDisconnected(DWORD nSessionID) = 0;
	virtual ~CService(){};
};

class CFibServerApi  
{
public:
	//-------------------配置接口函数-------------------------------
	/**获取节点标识
	*/
	virtual DWORD GetServerID() = 0;

	/**注册请求/应答模式服务器的监听地址
	* @param pszAddress 要监听的地址，格式为"protocol://address:port"
	*/
	virtual void RegisterListener(char *pszAddress) = 0;

	//-------------------消息接口函数-------------------------------
	/**创建一个消息
	* @return 创建出的消息
	*/
	virtual CXTPPackage *CreateMessage() = 0;
	
	//-------------------请求应答接口函数-------------------------------
	/**发送一个应答
	* @param pMessage 请求消息
	* @return FIB_RESULT_OK 请求成功发出
	*         <0 发送失败
	*/
	virtual int Response(CXTPPackage *pMessage, DWORD nSessionID) = 0;

	/**注册服务
	* @param pService 服务
	*/
	virtual void RegisterService(CService *pService) = 0;

	/**删除本接口
	*/
	virtual void Release() = 0;
	
	/**获取引擎
	*/
	virtual CReactor *GetReactor() = 0;
	//-------------------创建接口函数-------------------------------
	/**创建接口
	* @param pReactor 驱动反应器
	* @param nServerID  服务器标识，提供相同服务的一组服务器使用同一个ServerID
	* @return 创建出的接口
	*/
	static CFibServerApi *CreateServerApi(CReactor *pReactor, DWORD nServerID, int nMaxSession);

protected:
	virtual ~CFibServerApi(){};
};

#endif // !defined(AFX_FIBSERVERAPI_H__022BDC62_A62D_46E8_86E7_E725BD80D558__INCLUDED_)
