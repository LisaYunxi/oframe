// SslChannel.h: interface for the CSslChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSLCHANNEL_H__DB087329_E818_4519_8E44_E96B24B95F17__INCLUDED_)
#define AFX_SSLCHANNEL_H__DB087329_E818_4519_8E44_E96B24B95F17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcpChannel.h"

#include "openssl/rsa.h"      
#include "openssl/crypto.h"
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/rand.h"

class CSslChannel : public CTcpChannel  
{
public:
	CSslChannel(SSL* pSSL, const char *pszCommonName, int id);
	virtual ~CSslChannel();

protected:
	/**基于SSL的读取实现函数
	* @param number	buffer缓冲区的长度
	* @param buffer	存放缓冲区
	* @return >0 实际读取得长度
	* @return 0  读取时遭中断返回
	* @return -1 连结断开
	*/
	virtual int ReadImp(int number, char *buffer);


	/**基于SSL的写出实现函数
	* @param number	准备发送的长度
	* @param buffer	存放缓冲区
	* @return >0 实际发送得长度
	* @return 0  读取时遭中断返回
	* @return -1 发送失败
	*/
	virtual int WriteImp(int number, char *buffer);
	
	/**获取对方的标识
	* @return	对方的标识
	* @remark   返回NULL表示未知
	*/
	virtual char *GetRemoteCommonName(void);
private:
	SSL * m_pSSL;
	char *m_pszCommonName;
};

#endif // !defined(AFX_SSLCHANNEL_H__DB087329_E818_4519_8E44_E96B24B95F17__INCLUDED_)
