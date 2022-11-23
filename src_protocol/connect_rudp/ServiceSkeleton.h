// ServiceSkeleton.h: interface for the CServiceSkeleton class.
//
// 20080727 xuzh NT-0048:扩展CSessionFactory::CreateSession参数，便于创建
//                 会话时区分不同的连接
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICESKELETON_H__7B255FA5_6400_48EE_A24A_A8A0A1C65664__INCLUDED_)
#define AFX_SERVICESKELETON_H__7B255FA5_6400_48EE_A24A_A8A0A1C65664__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FibServerApi.h"
#include "SessionFactory.h"
#include "ServiceSession.h"

class CServiceSkeleton : public CSessionFactory, public CFibServerApi, public CXTPReqCallback
{
public:
	/**构造函数
	* @param pReactor 反应器
	* @param nServerID  服务器标识，提供相同服务的一组服务器使用同一个ServerID
	*/
	CServiceSkeleton(CReactor *pReactor, DWORD nServerID, int nMaxSession);
	
	//-------------------配置接口函数-------------------------------
	/**获取节点标识
	*/
	virtual DWORD GetServerID();

	/**注册请求/应答模式服务器的监听地址
	* @param pszAddress 要监听的地址，格式为"protocol://address:port"
	*/
	virtual void RegisterListener(char *pszAddress);

	//-------------------消息接口函数-------------------------------
	/**创建一个消息
	* @return 创建出的消息
	*/
	virtual CXTPPackage *CreateMessage();
	
	//-------------------请求应答接口函数-------------------------------
	/**发送一个应答
	* @param pMessage 请求消息
	* @return FIB_RESULT_OK 请求成功发出
	*         <0 发送失败
	*/
	virtual int Response(CXTPPackage *pMessage, DWORD nSessionID);

	/**注册服务
	* @param pService 服务
	*/
	virtual void RegisterService(CService *pService);

	/**删除本接口
	*/
	virtual void Release();

	/**获取引擎
	*/
	virtual CReactor *GetReactor() { return m_pReactor; }
	virtual CSession *CreateSession(CChannel *pChannel, DWORD);

	int HandleRequest(CXTPPackage *pXTPPackage, CXTPSession *pSessin);

	virtual void OnSessionConnected(CSession *pSession);
	virtual void OnSessionDisconnected(CSession *pSession, int nReason);

protected:
	virtual ~CServiceSkeleton();

protected:
	typedef CHashMap<DWORD, CServiceSession *, HashInt> CServiceSessionMap;
	CServiceSessionMap m_mapServiceSession;
	CService* m_pService;
	DWORD m_nServerID;
	CReactor *m_pReactor;
};

#endif // !defined(AFX_SERVICESKELETON_H__7B255FA5_6400_48EE_A24A_A8A0A1C65664__INCLUDED_)
