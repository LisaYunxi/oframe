// ServiceStub.h: interface for the CServiceStub class.
//
// 20070822	xuzh	配合SessionFactory修改，支持随机连接服务器
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICESTUB_H__9E755765_B46D_43A9_B128_55048BB2AD89__INCLUDED_)
#define AFX_SERVICESTUB_H__9E755765_B46D_43A9_B128_55048BB2AD89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EventHandler.h"
#include "SessionFactory.h"
#include "ServiceSession.h"
#include "FibClientApi.h"

class CServiceStub : public CFibClientApi
{
public:
	CServiceStub(CReactor *pReactor, DWORD nNodeID);
	//-------------------配置接口函数-------------------------------
	/**获取节点标识
	*/
	virtual DWORD GetNodeID();

	/**注册请求/应答模式服务器地址
	* 可以注册多个服务器地址，API会逐个尝试连接
	* @param nServerID 提供服务的服务器标识
	* @param pszAddress 要连接的服务器地址，格式为"protocol://address:port"
	* @param pRspHandler 应答处理对象
	*/
	virtual void RegisterServer(DWORD nServerID, char *pszAddress, CResponseHandler *pRspHandler);

	/**启动接口连接
	*/
	virtual void Start();
	//-------------------消息接口函数-------------------------------
	/**创建一个消息
	* @return 创建出的消息
	*/
	virtual CXTPPackage *CreateMessage();
		
	//-------------------请求应答接口函数-------------------------------
	/**发送一个请求
	* @param nServiceID 服务端口
	* @param pMessage 请求消息
	* @return FIB_RESULT_OK 请求成功发出
	*         <0 发送失败
	*/
	virtual int Request(DWORD nServerID, CXTPPackage *pMessage);

	/**删除本接口
	*/
	virtual void Release();

protected:
	virtual ~CServiceStub();
	CSessionFactory *GetSessionFactory(DWORD nServerID);
	CServiceSession *GetServerSession(DWORD nServerID);
protected:
	typedef CHashMap<DWORD, CSessionFactory *, HashInt> CServerMap;
	CServerMap m_mapServer;
	DWORD m_nNodeID;
	CReactor *m_pReactor;
};


#endif // !defined(AFX_SERVICESTUB_H__9E755765_B46D_43A9_B128_55048BB2AD89__INCLUDED_)
