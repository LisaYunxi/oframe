// ListenCtrl.h: interface for the CListenCtrl class .
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTENCTRL_H__203EA6D7_4E89_4512_9A8C_7D762BAF2D9F__INCLUDED_)
#define AFX_LISTENCTRL_H__203EA6D7_4E89_4512_9A8C_7D762BAF2D9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "EventHandler.h"
#include "Server.h"
#include "Channel.h"

/**是一个基于TCP的网络服务器端，是EventHandler的子类，接受客户端的连结请求
*/
class  CListenCtrl : public CEventHandler  
{
public:
	/**构造函数
	* @param pReactor  运行引擎指针
	* @param pServer   一个TcpServer的指针
	*/
	CListenCtrl(CReactor *pReactor,CServerBase *pServer);
	
	/**析构函数
	*/
	virtual ~CListenCtrl();
	
	/**由Reactor调用，处理连结请求事件
	* @return 0 成功
	* @return -1 失败
	*/
	virtual int HandleInput();
	
	/**获得监听id
	* @param pReadId   一个Server套接字Id指针，用于返回
	* @param pWriteId  保留
	*/
	virtual void GetIds(int *pReadId, int *pWriteId);
protected:

	/**当有新的连结时，调用的方法，子类必须实现该方法
	* @param pChannel  一个新的Channel指针
	* @return 0 成功
	*/
	virtual int OnConnected(CChannel *pChannel)=0;
protected:
	CServerBase *m_pServer;		/**< 一个Server指针 */
};

/**
 * 是一个基于UDP的网络服务器端，是EventHandler的子类，监听UDP数据报广播。
 */
class  CUdpListenCtrl : public CEventHandler  
{
public:
	/**构造函数
	 * @param pReactor  运行引擎指针
	 * @param pServer   一个TcpServer的指针
	 */
	CUdpListenCtrl(CReactor *pReactor, CServerBase *pServer);
	
	/**析构函数
	 */
	virtual ~CUdpListenCtrl();
	
	/**由Reactor调用，处理连结请求事件
	 * @return 0 成功
	 * @return -1 失败
	 */
	virtual int HandleInput();
	
	/**获得监听id
	 * @param pReadId   一个Server套接字Id指针，用于返回
	 * @param pWriteId  保留
	 */
	virtual void GetIds(int *pReadId, int *pWriteId);

protected:

	/**
	 * 当有新的udp 数据报到达时，调用的方法，子类必须实现该方法
	 * @param pChannel  一个的Channel指针
	 * @return 0 成功
	 */
	virtual int OnRecvDatagram(CChannel *pChannel)=0;
protected:
	CChannel *m_pChannel;		/**< 一个Channel指针 */
	CServerBase *m_pServer;	/**< 一个Server指针 */
};

#endif !defined(AFX_LISTENCTRL_H__203EA6D7_4E89_4512_9A8C_7D762BAF2D9F__INCLUDED_)
