// Server.h: interface for the CServer class .
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVER_H__FE5CED62_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_SERVER_H__FE5CED62_6EC3_11D7_BB64_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "ServiceName.h"
#include "Channel.h"

/**CServer时一个抽象的未完成连接的通讯服务器端。实现了一个基于有连
*接通讯的服务器端基本功能。不同类型的网络通讯需要继承这个类，以实
*其真正功能。
*/
class  CServerBase 
{
public:
	/**构造函数，在指定的服务名上侦听网络的连接
	* @param  pName	指定的服务名称
	*/
	CServerBase(CServiceName *pName);
	
	/**析构函数，释放占用的资源
	*/
	virtual ~CServerBase();
	
	/**等待一个客户端的连接，本方法将使用acceptImpl完成真正的操作
	* @param  wait	表示需要等待到连接才返回，0表示不等待
	* @return NULL   新Channel未建成功
	* @return 非NULL 新的Channel
	*/
	virtual CChannel *Accept(int wait)=0;
	
	/**获取最近的错误信息
	* @return 非NULL 最近的错误信息
	* @return NULL   表示最近的操作获得成功
	*/
	inline char *GetErrorMsg(void);

	/**获取本服务器端的服务名称
	* @return	服务名称
	*/
	inline CServiceName *GetServiceName(void);
	
	/**获取本服务器端在进行select时，使用的文件号。是纯虚函数，继承对象需要实现此方法。
	*如果本网络实现使用的是查询式的，则应当返回-1
	* @return select时的文件号
	*/
	virtual int GetId(void)=0;
protected:
	/**设置最近发生的错误信息
	* @param msg	错误信息
	*/
	inline void SetErrorMsg(char *pszMsg);
	
private:
	CServiceName *m_pServiceName;   /**< 存储本服务器端的服务名称 */

	char *m_pszErrorMsg;		/**< 存储最近发生的错误信息 */
};

inline CServiceName *CServerBase::GetServiceName(void)
{
	return m_pServiceName;
}

inline char *CServerBase::GetErrorMsg(void)
{
	return m_pszErrorMsg;
}

inline void CServerBase::SetErrorMsg(char *pszMsg)
{
	m_pszErrorMsg = pszMsg;
}



#endif !defined(AFX_SERVER_H__FE5CED62_6EC3_11D7_BB64_444553540000__INCLUDED_)
