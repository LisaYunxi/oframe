// SslClient.h: interface for the CSslClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSLCLIENT_H__20D81522_3061_4731_B530_B23268EC0D85__INCLUDED_)
#define AFX_SSLCLIENT_H__20D81522_3061_4731_B530_B23268EC0D85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcpClient.h"
#include "SslChannel.h"

class CSslClient : public CTcpClient 
{
public:
	CSslClient(SSL_CTX *pCTX);
	virtual ~CSslClient();

protected:
	virtual CChannel *CreateChannel(int id);

private:
	SSL_CTX *m_pCTX;
};

#endif // !defined(AFX_SSLCLIENT_H__20D81522_3061_4731_B530_B23268EC0D85__INCLUDED_)
