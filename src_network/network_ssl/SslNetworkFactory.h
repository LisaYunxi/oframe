// SslNetworkFactory.h: interface for the CSslNetworkFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSLNETWORKFACTORY_H__E628B755_AAF6_4DB7_B9E0_FDD38E2327FE__INCLUDED_)
#define AFX_SSLNETWORKFACTORY_H__E628B755_AAF6_4DB7_B9E0_FDD38E2327FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SslChannel.h"
#include "NetworkFactory.h"

class CSslCertificateCallback
{
public:
	virtual bool GetVerifyMode() = 0;
	virtual char *GetCertFileName() = 0;
	virtual char *GetKeyFileName() = 0;
	virtual char *GetCaFileName() = 0;
	virtual char *GetKeyFilePassword() = 0;
};

class CSslNetworkFactory  : public CNetworkFactory
{
public:
	CSslNetworkFactory();
	virtual ~CSslNetworkFactory();

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

	void RegisterCertificateCallback(CSslCertificateCallback *pCertificateCallback);

	int RegisterCertificateFile(bool bVerifyPeer, const char *pszCertFileName, 
		const char *pszKeyFileName, const char *pszCaFileName,
		const char *pszKeyFilePassword);
public:
	static CSslNetworkFactory m_instance;

private:
	static int pem_passwd_cb(char *buf, int size, int rwflag, void *userdata);
	int SetCertificateFile();

private:
	SSL_CTX *m_pCTX;
	CSslCertificateCallback *m_pCertificateCallback;
};

#endif // !defined(AFX_SSLNETWORKFACTORY_H__E628B755_AAF6_4DB7_B9E0_FDD38E2327FE__INCLUDED_)
