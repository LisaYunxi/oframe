// SslNetworkFactory.cpp: implementation of the CSslNetworkFactory class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "SslNetworkFactory.h"
#include "SslClient.h"
#include "SslServer.h"

const char *SSL_NETWORK_NAME = "ssl";

CSslNetworkFactory CSslNetworkFactory::m_instance;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSslNetworkFactory::CSslNetworkFactory()
{
	OpenSSL_add_ssl_algorithms(); //初始化

	m_pCTX = NULL;
	m_pCertificateCallback = NULL;
	
	RegisterFactory();
}

CSslNetworkFactory::~CSslNetworkFactory()
{
	if (m_pCTX != NULL)
	{
		SSL_CTX_free (m_pCTX);
	}
}

CServerBase *CSslNetworkFactory::CreateServer(CServiceName *pName)
{
	if (strcmp(pName->GetChannel(), SSL_NETWORK_NAME) == 0)
	{
		if (m_pCTX == NULL)
		{
			SSL_METHOD *meth = (SSL_METHOD *)TLSv1_server_method();	//采用什么协议(SSLv2/SSLv3/TLSv1)在此指定
			m_pCTX = SSL_CTX_new (meth);
			SSL_CTX_set_default_passwd_cb(m_pCTX, pem_passwd_cb);
			SSL_CTX_set_default_passwd_cb_userdata(m_pCTX, this);

			SetCertificateFile();
			
			SSL_CTX_set_cipher_list(m_pCTX, "RC4-MD5");
		}

		return new CSslServer(pName, m_pCTX);
	}
	return CNetworkFactory::CreateServer(pName);
}
	
CClientBase *CSslNetworkFactory::CreateClient(CServiceName *pName)
{
	if (strcmp(pName->GetChannel(), SSL_NETWORK_NAME) == 0)
	{
		if (m_pCTX == NULL)
		{
			SSL_METHOD *meth =(SSL_METHOD *) TLSv1_client_method();	//采用什么协议(SSLv2/SSLv3/TLSv1)在此指定
			m_pCTX = SSL_CTX_new (meth);
			SSL_CTX_set_default_passwd_cb(m_pCTX, pem_passwd_cb);
			SSL_CTX_set_default_passwd_cb_userdata(m_pCTX, this);

			int seed_int[100];
			//构建随机数生成机制,WIN32平台必需
			srand( (unsigned)time( NULL ) );
			for( int i = 0; i < 100; i++ )
			{
				seed_int[i] = rand();
			}
			RAND_seed(seed_int, sizeof(seed_int));

			SetCertificateFile();
		}
		return new CSslClient(m_pCTX);
	}
	return CNetworkFactory::CreateClient(pName);
}

int CSslNetworkFactory::pem_passwd_cb(char *buf, int size, int rwflag, void *userdata)
{
	CSslNetworkFactory *pSslNetworkFactory = (CSslNetworkFactory *)userdata;
	CSslCertificateCallback *pCertificateCallback = 
		pSslNetworkFactory->m_pCertificateCallback;
	if (pCertificateCallback == NULL)
	{
		buf[0] = '\0';
	}
	else
	{
		strncpy(buf, pCertificateCallback->GetKeyFilePassword(), size);
		buf[size - 1] = '\0';
	}
	return(strlen(buf));
}

int CSslNetworkFactory::SetCertificateFile()
{
	if (m_pCertificateCallback == NULL)
	{
		return 0;
	}
	if (m_pCertificateCallback->GetVerifyMode())
	{
		SSL_CTX_set_verify(m_pCTX, SSL_VERIFY_PEER, NULL);   //验证与否
	}
	if (SSL_CTX_load_verify_locations(m_pCTX, m_pCertificateCallback->GetCaFileName(), NULL) <= 0)
	{
		NET_EXCEPTION_LOG1("SSL_CTX_load_verify_locations [%s] error\n",
			m_pCertificateCallback->GetCaFileName());
		return -1;
	}
	
	if (SSL_CTX_use_certificate_file(m_pCTX, m_pCertificateCallback->GetCertFileName(), SSL_FILETYPE_PEM) <= 0) {
		NET_EXCEPTION_LOG1("SSL_CTX_use_certificate_file [%s] error\n",
			m_pCertificateCallback->GetCertFileName());
		return -2;
	}

	if (SSL_CTX_use_PrivateKey_file(m_pCTX, m_pCertificateCallback->GetKeyFileName(), SSL_FILETYPE_PEM) <= 0) {
		NET_EXCEPTION_LOG1("SSL_CTX_use_PrivateKey_file [%s] error\n",
			m_pCertificateCallback->GetKeyFileName());
		return -3;
	}
	
	if (!SSL_CTX_check_private_key(m_pCTX)) {
		NET_EXCEPTION_LOG0("SSL_CTX_check_private_key error\n");
		return -4;
	} 

	return 0;
}

void CSslNetworkFactory::RegisterCertificateCallback(CSslCertificateCallback *pCertificateCallback)
{
	m_pCertificateCallback = pCertificateCallback;
}

class CStaticSslCertificateCallback : public CSslCertificateCallback
{
public:
	CStaticSslCertificateCallback(bool bVerifyPeer, const char *pszCertFileName, 
		const char *pszKeyFileName, const char *pszCaFileName,
		const char *pszKeyFilePassword)
	{
		m_bVerifyPeer = bVerifyPeer;
		m_pszCertFileName = strdup(pszCertFileName);
		m_pszKeyFileName = strdup(pszKeyFileName);
		m_pszCaFileName = strdup(pszCaFileName);
		m_pszKeyFilePassword = strdup(pszKeyFilePassword);
	}
	~CStaticSslCertificateCallback()
	{
		delete m_pszCertFileName; 
		delete m_pszKeyFileName; 
		delete m_pszCaFileName;
		delete m_pszKeyFilePassword;	
	}
	virtual bool GetVerifyMode()
	{
		return m_bVerifyPeer;
	}
	virtual char *GetCertFileName()
	{
		return m_pszCertFileName;
	}
	virtual char *GetKeyFileName()
	{
		return m_pszKeyFileName;
	}
	virtual char *GetCaFileName()
	{
		return m_pszCaFileName;
	}
	virtual char *GetKeyFilePassword()
	{
		return m_pszKeyFilePassword;
	}
private:
	bool m_bVerifyPeer;
	char *m_pszCertFileName; 
	char *m_pszKeyFileName; 
	char *m_pszCaFileName;
	char *m_pszKeyFilePassword;	
};

int CSslNetworkFactory::RegisterCertificateFile(bool bVerifyPeer, const char *pszCertFileName, 
	const char *pszKeyFileName, const char *pszCaFileName,
	const char *pszKeyFilePassword)
{
	CStaticSslCertificateCallback *s_pCallback = NULL;
	delete s_pCallback;
	s_pCallback = new CStaticSslCertificateCallback(bVerifyPeer, 
		pszCertFileName, pszKeyFileName, pszCaFileName, pszKeyFilePassword);

	RegisterCertificateCallback(s_pCallback);

	int nRet = 0;
	if (m_pCTX == NULL)
	{
		SSL_METHOD *meth = (SSL_METHOD *) TLSv1_client_method();	//采用什么协议(SSLv2/SSLv3/TLSv1)在此指定
		m_pCTX = SSL_CTX_new (meth);
		SSL_CTX_set_default_passwd_cb(m_pCTX, pem_passwd_cb);
		SSL_CTX_set_default_passwd_cb_userdata(m_pCTX, this);

		nRet = SetCertificateFile();

		SSL_CTX_free (m_pCTX);
		m_pCTX = NULL;
	}
	else
	{
		nRet = SetCertificateFile();
	}

	return nRet;
}

