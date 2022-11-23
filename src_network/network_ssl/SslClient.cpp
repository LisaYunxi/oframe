// SslClient.cpp: implementation of the CSslClient class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "SslClient.h"
#include "SocketInit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSslClient::CSslClient(SSL_CTX *pCTX)
{
	m_pCTX = pCTX;
}

CSslClient::~CSslClient()
{

}

#define CREATE_CHANNEL_ERROR(msg)			\
	SetErrorMsg(msg);						\
	closesocket(id);						\
	if (ssl != NULL)						\
	{										\
		SSL_free(ssl);						\
	}										\
	return NULL;							\


CChannel *CSslClient::CreateChannel(int id)
{
	SSL *ssl = SSL_new (m_pCTX);                         
	if(ssl==NULL)
	{
		CREATE_CHANNEL_ERROR("client SSL_new error");
	}

	SSL_set_fd (ssl, id);

	fd_set writefds, readfds;
	struct timeval timeout;
	for (;;)
	{
		int err = SSL_connect (ssl);
		if (err > 0)
		{
			break;
		}
		/* use select to wait for it */
		FD_ZERO(&writefds);
		FD_ZERO(&readfds);
		int errorcode = SSL_get_error(ssl,err);
		if (errorcode == SSL_ERROR_WANT_WRITE)
		{
			FD_SET(id,&writefds);
		}
		else if (errorcode == SSL_ERROR_WANT_READ)
		{
			FD_SET(id,&readfds);
		}
		else
		{
			CREATE_CHANNEL_ERROR("client SSL_new error");
		}
		timeout.tv_sec=5;
		timeout.tv_usec=0;
		int ret=select(id+1,&readfds,&writefds,NULL,&timeout);
		
		/* timeout? then return with fail */
		if (ret==0)
		{
			CREATE_CHANNEL_ERROR("Can not connect to SSL server due to timeout");
		}
	}
	int nVerifyMode = SSL_CTX_get_verify_mode(m_pCTX);

	if (nVerifyMode != SSL_VERIFY_NONE)
	{
		//得到对方的证书
		X509 *peer_cert = SSL_get_peer_certificate (ssl);
		
		if (peer_cert == NULL)
		{
			CREATE_CHANNEL_ERROR("Get perr certificate error");
		}
		
		char buf[200];
		if( X509_NAME_get_text_by_NID(X509_get_subject_name (peer_cert),
			NID_commonName, buf, sizeof(buf)) < 0)
		{
			CREATE_CHANNEL_ERROR("Get certificate common name error");
		}

		
		X509_free (peer_cert);  //将证书释放

		return new CSslChannel(ssl, buf, id);
		
	}
	
	return new CSslChannel(ssl, NULL, id);
}

