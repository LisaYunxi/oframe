// SslChannel.cpp: implementation of the CSslChannel class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "SslChannel.h"
#include "SocketInit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSslChannel::CSslChannel(SSL* pSSL, const char *pszCommonName, int id)
:CTcpChannel(id)
{
	m_pSSL = pSSL;
	m_pszCommonName = NULL;
	if (pszCommonName != NULL)
	{
		m_pszCommonName = strdup(pszCommonName);
	}
}

CSslChannel::~CSslChannel()
{
	SSL_free (m_pSSL);
	delete m_pszCommonName;
}

int CSslChannel::ReadImp(int number, char *buffer)
{
	int nCount = SSL_read(m_pSSL,buffer,number);
	if(nCount <= 0)
	{
		int n = SSL_get_error(m_pSSL, nCount);
		switch(SSL_get_error(m_pSSL, nCount))
		{
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_READ:
			return 0;
		case SSL_ERROR_NONE:
			break;
		default:
			return -1;
		}
	}
	return nCount;
}

int CSslChannel::WriteImp(int number, char *buffer)
{
	int nCount = SSL_write(m_pSSL,buffer,number);
	if(nCount <= 0)
	{
		switch(SSL_get_error(m_pSSL, nCount))
		{
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_READ:
			return 0;
		case SSL_ERROR_NONE:
			break;
		default:
			return -1;
		}
	}
	return nCount;
}

char *CSslChannel::GetRemoteCommonName()
{
	return m_pszCommonName;
}