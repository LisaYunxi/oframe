// UdpNetworkFactory.h: interface for the CUdpNetworkFactory class.
//
// 2006.10.11	xuzh	将变量UDP_NETWORK_NAME移入CPP中，
//						防止多次包含引起的重复定义 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPNETWORKFACTORY_H__4EB0F1E7_5C35_43B0_B0B2_8160B72651D0__INCLUDED_)
#define AFX_UDPNETWORKFACTORY_H__4EB0F1E7_5C35_43B0_B0B2_8160B72651D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "NetworkFactory.h"

class CUdpNetworkFactory : public CNetworkFactory  
{
public:
	CUdpNetworkFactory();
	virtual ~CUdpNetworkFactory();
	
	/**创建一个TCP服务器端。如果Service Name不匹配则调用链表后面的类厂来创建。
	* @param pName	服务名称
	* @return  得到的服务器端
	*/
	virtual CServerBase *CreateServer(CServiceName *pName);
	
	/**创建一个TCP客户端。如果Service Name不匹配则调用链表后面的类厂来创建。
	* @param	pName	要连接的服务名称
	* @return   得到的客户端
	*/
	virtual CClientBase *CreateClient(CServiceName *pName);

//private:
	static CUdpNetworkFactory m_instance;
};

#endif // !defined(AFX_UDPNETWORKFACTORY_H__4EB0F1E7_5C35_43B0_B0B2_8160B72651D0__INCLUDED_)
