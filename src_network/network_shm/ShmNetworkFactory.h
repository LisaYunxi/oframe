// ShmNetworkFactory.h: interface for the CShmNetworkFactory class.
//
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ShmNETWORKFACTORY_H__9C863AA5_2C99_445B_B6FD_C1459913BB4F__INCLUDED_)
#define AFX_ShmNETWORKFACTORY_H__9C863AA5_2C99_445B_B6FD_C1459913BB4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetworkFactory.h"

class CShmNetworkFactory : public CNetworkFactory  
{
public:
	CShmNetworkFactory();
	virtual ~CShmNetworkFactory();

	/**创建一个Shm服务器端。如果Service Name不匹配则调用链表后面的类厂来创建。
	* @param pName	服务名称
	* @return  得到的服务器端
	*/
	virtual CServerBase *CreateServer(CServiceName *pName);
	
	/**创建一个Shm客户端。如果Service Name不匹配则调用链表后面的类厂来创建。
	* @param	pName	要连接的服务名称
	* @return   得到的客户端
	*/
	virtual CClientBase *CreateClient(CServiceName *pName);

private:
	static CShmNetworkFactory m_instance;
};

CShmNetworkFactory CShmNetworkFactory::m_instance;

#endif // !defined(AFX_ShmNETWORKFACTORY_H__9C863AA5_2C99_445B_B6FD_C1459913BB4F__INCLUDED_)
