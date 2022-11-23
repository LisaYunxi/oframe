#ifndef _TCPNETWORKFACTORY_H__
#define _TCPNETWORKFACTORY_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetworkFactory.h"

class CTcpNetworkFactory : public CNetworkFactory  
{
public:
	CTcpNetworkFactory();
	virtual ~CTcpNetworkFactory();

	/**����һ��TCP�������ˡ����Service Name��ƥ����������������೧��������
	* @param pName	��������
	* @return  �õ��ķ�������
	*/
	virtual CServerBase *CreateServer(CServiceName *pName);
	
	/**����һ��TCP�ͻ��ˡ����Service Name��ƥ����������������೧��������
	* @param	pName	Ҫ���ӵķ�������
	* @return   �õ��Ŀͻ���
	*/
	virtual CClientBase *CreateClient(CServiceName *pName);

//private:
	static CTcpNetworkFactory m_instance;
};

#endif
