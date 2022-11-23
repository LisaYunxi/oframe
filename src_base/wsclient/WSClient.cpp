#include "WSClient.h"
#include "platform.h"

int CWSClient::s_is_connected = 0;

void CWSClient::EvHhandler(struct mg_connection* nc, int ev, void* ev_data)
{
	CWSClient* pWSClient = (CWSClient *)nc->user_data;;

	switch (ev) 
	{
	case MG_EV_CONNECT: 
	{
		int status = *((int*)ev_data);
		if (status != 0) 
			printf("-- Connection error: %d\n", status);
		break;
	}
	case MG_EV_WEBSOCKET_HANDSHAKE_DONE: 
	{
		struct http_message* hm = (struct http_message*) ev_data;
		if (hm->resp_code == 101) 
		{
			printf("-- Connected\n");
			s_is_connected = 1;
			pWSClient->OnConnected();
		}
		else 
		{
			printf("-- Connection failed! HTTP code %d\n", hm->resp_code);
			/* Connection will be closed after this. */
		}
		break;
	}
	case MG_EV_POLL: 
	{
		pWSClient->OnPoll(nc);
		break;
	}
	case MG_EV_WEBSOCKET_FRAME: 
	{
		struct websocket_message* wm = (struct websocket_message*) ev_data;
		pWSClient->OnFrame(wm);
		break;
	}
	case MG_EV_CLOSE: 
	{
		if (s_is_connected)
		{
			printf("-- Disconnected\n");
			s_is_connected = 0;
		}
		pWSClient->OnDisConnected();
		break;
	}
  }
}

#ifdef _WIN32
DWORD WINAPI CWSClient::_ThreadEntry(LPVOID pParam)
#else
void* CWSClient::_ThreadEntry(void* pParam)
#endif
{
	CWSClient* pWSClient = (CWSClient*)pParam;

	pWSClient->Run();

	return NULL;
}

CWSClient::CWSClient()
	: m_nc(NULL)
	, m_hThread(NULL)
	, m_IDThread(0)
	, m_nStatus(0)
	, m_pSpi(NULL)
{
	mg_mgr_init(&m_mgr, NULL);
	mg_set_nameserver(&m_mgr, "8.8.8.8");
}

CWSClient::~CWSClient()
{
	mg_mgr_free(&m_mgr);
}

bool CWSClient::Start()
{
	if (m_nc == NULL)
		return false;

	if (m_hThread != (THREAD_HANDLE)0)
		return true;

	bool ret = true;
#ifdef _WIN32
	m_hThread = ::CreateThread(NULL, 0, _ThreadEntry, this, 0, &m_IDThread);
	if (m_hThread == NULL)
		ret = false;
#else
	ret = (::pthread_create(&m_hThread, NULL, &_ThreadEntry, this) == 0);
#endif
	return ret;
}

int CWSClient::Connect(const string& url)
{
	m_strUrl = url;

	m_nc = mg_connect_ws(&m_mgr, EvHhandler, url.c_str(), NULL, NULL);
	if (m_nc == NULL) 
	{
		fprintf(stderr, "Invalid address\n");
		return 1;
	}
	m_nStatus = 1;
	m_nc->user_data = this;
	return 0;
}

void CWSClient::RegisterSpi(CWSClientSpi* pSpi)
{
	m_pSpi = pSpi;
}

int CWSClient::Send(const string& msg)
{
	if (2 != m_nStatus)
		return -1;

	CAutoLock< ManualLock > autoLock(m_lock);
	m_messages.push_back(msg);

	return 0;
}

void CWSClient::Run()
{
	while (1) 
	{
		mg_mgr_poll(&m_mgr, 1000);
		if (0 == m_nStatus)
		{
			Connect(m_strUrl);
		}
	}
}

void CWSClient::OnConnected()
{
	m_nStatus = 2;
	if (m_pSpi != NULL)
		m_pSpi->OnWSConnected();
}

void CWSClient::OnDisConnected()
{
	m_nStatus = 0;
	if (m_pSpi != NULL)
		m_pSpi->OnWSDisConnected();
}

void CWSClient::OnFrame(websocket_message* wm)
{
	//printf("%.*s\n", (int)wm->size, wm->data);
	if (m_pSpi != NULL)
		m_pSpi->OnData(wm->data, wm->size);
}

void CWSClient::OnPoll(mg_connection* nc)
{
	if (m_pSpi != NULL)
		m_pSpi->OnPoll();

	CAutoLock< ManualLock > autoLock(m_lock);
	if (m_messages.empty())
		return;

	for (size_t i = 0; i < m_messages.size(); i++)
	{
		mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, m_messages[i].c_str(), m_messages[i].size());
	}
	m_messages.clear();
}
