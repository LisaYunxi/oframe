// TcpClient.h: interface for the CTcpClient class .
//
// 2006.10.11	hwp	为支持SSL，增加方法CreateChannel 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_TCPCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Client.h"
/**CTcpClient是一个基于TCP的网络客户端
*/
class  CTcpClient : public CClientBase
{
public:
	/**构造函数，完成初始化
	*/
	CTcpClient();
	
	/**析构函数
	*/
	virtual ~CTcpClient();

	/**基于TCP的连接实现函数
	* @param pName 服务器的连结信息
	* @return 非NULL 成功，已建立的Channel
	* @return NULL 失败
	*/
	virtual CChannel *Connect(CServiceName *pName);
	
protected:
	virtual CChannel *CreateChannel(int id);
};


#endif // !defined(AFX_TCPCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_)
