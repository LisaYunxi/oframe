// SslServer.h: interface for the CSslServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSLSERVER_H__B4F0C4E2_BB8F_46ED_8AFC_9856897454F6__INCLUDED_)
#define AFX_SSLSERVER_H__B4F0C4E2_BB8F_46ED_8AFC_9856897454F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcpServer.h"
#include "SslChannel.h"

class CSslServer : public CTcpServer 
{
public:
	CSslServer(CServiceName *pName, SSL_CTX *pCTX);
	virtual ~CSslServer();

protected:
	virtual CChannel *CreateChannel(int id);

private:
	SSL_CTX *m_pCTX;
};

#endif // !defined(AFX_SSLSERVER_H__B4F0C4E2_BB8F_46ED_8AFC_9856897454F6__INCLUDED_)
