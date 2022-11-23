#include "public.h"
#include "SessionFactory.h"
#include "BaseFunction.h"
#include "monitorIndex.h"

const int TIMER_INTERVAL_CONNECT = 5000;

//#include "TcpNetworkFactory.h"
//#include "UdpNetworkFactory.h"
//#include "MultiNetworkFactory.h"

//extern CNetworkFactory g_NetworkFactory;
//CTcpNetworkFactory CTcpNetworkFactory::m_instance;
//CUdpNetworkFactory CUdpNetworkFactory::m_instance;
//CMultiNetworkFactory CMultiNetworkFactory::m_instance;


//////////////////////////////////////////////////////////////////////
CSessionConnecter::CSessionConnecter(const char *location, unsigned int dwMark) :m_ServiceName(location)
{
	m_pChannel = NULL;
	m_dwMark = dwMark;
}

CChannel *CSessionConnecter::GetChannel()
{
	return m_pChannel;
}

CServiceName *CSessionConnecter::GetServiceName()
{
	return &m_ServiceName;
}

unsigned int CSessionConnecter::GetMark()
{
	return m_dwMark;
}

void CSessionConnecter::CheckChannel(CChannel *pChannel)
{
	if (m_pChannel == pChannel)
	{
		m_pChannel = NULL;
	}
}

void CSessionConnecter::OnConnected(CChannel *pChannel)
{
	m_pChannel = pChannel;
}

CChannel *CSessionConnecter::Connect()
{
	CChannel *pNewChannel = CNetworkFactory::GetInstance()->CreateChannel(&m_ServiceName);
	if (pNewChannel == NULL)
	{
		REPORT_EVENT(LOG_INFO, "Connect", "Connect to %s:%d Error", m_ServiceName.GetHost(), m_ServiceName.GetPort());
	}
	else
	{
		REPORT_EVENT(LOG_INFO, "Connect", "Connect to %s:%d", m_ServiceName.GetHost(), m_ServiceName.GetPort());
	}
	return pNewChannel;
}

CConnecterManager::CConnecterManager(CReactor *pReactor, CEventHandler *pSessionFactory) :CEventHandler(pReactor)
{
	m_pSessionFactory = pSessionFactory;
	Clear();
}

CConnecterManager::~CConnecterManager()
{
	Clear();
}

void CConnecterManager::AppendConnecter(CSessionConnecter *pConnecter)
{
	m_connecters.push_back(pConnecter);
}

void CConnecterManager::Clear()
{
	for (int i = 0; i < m_connecters.size(); i++)
	{
		delete m_connecters[i];
	}
	m_connecters.clear();
	m_nConnecterPos = -1;
}

void CConnecterManager::OnDiscconnected(CChannel *pChannel)
{
	for (int i = 0; i < m_connecters.size(); i++)
	{
		m_connecters[i]->CheckChannel(pChannel);
	}
	m_pSessionFactory->PostEvent(UM_CONNECT_LOOP, 0, this);
}

void CConnecterManager::OnConnected(CChannel *pChannel)
{
	m_connecters[m_nConnecterPos]->OnConnected(pChannel);
}

void CConnecterManager::RandomConnecters()
{
	int nConnecterSize = (int)m_connecters.size();
	if (nConnecterSize <= 0)
	{
		return;
	}
	int nStartPos = rand() % nConnecterSize;
	//	NET_CONNECT_LOG1("nStartPos = %d\n", nStartPos);
	for (int i = 0; i < nStartPos; i++)
	{
		CSessionConnecter *pBack = m_connecters.back();
		m_connecters.pop_back();
		m_connecters.insert(m_connecters.begin(), pBack);
	}
}

int CConnecterManager::Connect(bool bRandom)
{
	if (bRandom)
	{
		RandomConnecters();
	}
	m_nConnecterPos = -1;
	return ConnectNext();
}

int CConnecterManager::ConnectNext()
{
	for (m_nConnecterPos++; m_nConnecterPos < m_connecters.size(); m_nConnecterPos++)
	{
		CSessionConnecter *pConnecter = m_connecters[m_nConnecterPos];
		if (pConnecter->GetChannel() == NULL)
		{
			PostEvent(UM_TRY_CONNECT, 0, pConnecter);
			return m_nConnecterPos;
		}
	}
	m_nConnecterPos = -1;
	m_pSessionFactory->SendEvent(UM_CONNECT_LOOP, 0, this);
	return m_nConnecterPos;
}

void CConnecterManager::CancelConnect()
{
	m_nConnecterPos = -1;
}

bool CConnecterManager::IsConnecting()
{
	return m_nConnecterPos != -1;
}

int CConnecterManager::HandleEvent(int nEventID, unsigned int dwParam, void *pParam)
{
	switch (nEventID)
	{
	case UM_TRY_CONNECT:
	{
		CSessionConnecter *pConnecter = (CSessionConnecter *)pParam;
		CChannel *pNewChannel = pConnecter->Connect();
		m_pSessionFactory->SendEvent(UM_CONNECT_RESULT, pConnecter->GetMark(), pNewChannel);
	}
	return 0;
	default:
		break;
	}
	return CEventHandler::HandleEvent(nEventID, dwParam, pParam);
}

//////////////////////////////////////////////////////////////////////
CSessionListener::CSessionListener(CReactor *pReactor, CEventHandler *pSessionFactory,
	CServerBase *pServer, unsigned int dwMark)
	:CListenCtrl(pReactor, pServer)
{
	m_pSessionFactory = pSessionFactory;
	m_dwMark = dwMark;
}

int CSessionListener::OnConnected(CChannel *pChannel)
{
	m_pSessionFactory->SendEvent(UM_LISTEN_RESULT, m_dwMark, pChannel);
	return 0;
}

CSessionFactory::CSessionFactory(CReactor *pReactor, int nMaxSession) :CEventHandler(pReactor)
{
	srand(time(NULL));
	m_nMaxSession = nMaxSession;
	m_bConnecterEnable = true;
	m_bListenerEnable = true;
	m_bRandomConnect = true;
	m_pConnecterManager = new CConnecterManager(&m_reactorConnect, this);

	//静态库的调用中对 nerwork中的全局/静态变量不会初始化，这里显示调用，会激发全局变量的初始化
//	g_NetworkFactory.libinit();
//	CTcpNetworkFactory::m_instance.libinit();
//	CUdpNetworkFactory::m_instance.libinit();
//	CMultiNetworkFactory::m_instance.libinit();

	//进行过期时间控制
// 	char * CurrDate = getCurrDate();
// 	if(strcmp(CurrDate,"20170909") > 0)
// 	{
// 		printf("The Permit Is Expired, Contact To Get The Next Software!");
// 		exit(-1);
// 	}
}

CSessionFactory::~CSessionFactory()
{
	Stop();
	for (int i = 0; i < m_listeners.size(); i++)
	{
		delete m_listeners[i];
	}
	m_listeners.clear();
	delete m_pConnecterManager;
}

void CSessionFactory::Start()
{
	m_reactorConnect.Create();
	OnTimer(TIMER_ID_CONNECT);
}

void CSessionFactory::Stop()
{
	m_reactorConnect.Stop();
	m_reactorConnect.Join();
	DisconnectAll(0);
}

void CSessionFactory::DisconnectAll(int nReason)
{
	for (;;)
	{
		CSessionMap::iterator itor = m_mapSession.Begin();
		if (itor.IsEnd())
		{
			break;
		}
		(*itor)->Disconnect(nReason);
	}
}

void CSessionFactory::RegisterConnecter(const char *location, unsigned int dwMark)
{
	vector<string> addrs1 = Txtsplit(location, ";");
	for (int j = 0; j < addrs1.size(); j++)
	{
		vector<string> addrs2 = Txtsplit(addrs1[j], ",");
		for (int i = 0; i < addrs2.size(); i++)
		{
			char eachAddr[128] = { 0 };
			strncpy(eachAddr, addrs2[i].c_str(), addrs2[i].size());
			CSessionConnecter *pConnecter = new CSessionConnecter(eachAddr, dwMark);
			m_pConnecterManager->AppendConnecter(pConnecter);
			REPORT_EVENT(LOG_CRITICAL, "CSessionFactory", "Connect to Port:%s", eachAddr);
		}
	}
}

void CSessionFactory::RegisterListener(const char *location, unsigned int dwMark)
{
	vector<string> addrs1 = Txtsplit(location, ";");
	for (int j = 0; j < addrs1.size(); j++)
	{
		vector<string> addrs2 = Txtsplit(addrs1[j], ",");
		for (int i = 0; i < addrs2.size(); i++)
		{
			char eachAddr[128] = { 0 };
			strncpy(eachAddr, addrs2[i].c_str(), addrs2[i].size());
			CServiceName srvname(eachAddr);
			CServerBase *pServer = CNetworkFactory::GetInstance()->CreateServer(&srvname);
			if (pServer == NULL)
				return;
			CSessionListener *pListener = new CSessionListener(m_pReactor, this, pServer, dwMark);
			m_pReactor->RegisterIO(pListener);
			m_listeners.push_back(pListener);

			REPORT_EVENT(LOG_CRITICAL, "CSessionFactory", "Open Port:%s", eachAddr);
		}
	}
	m_sLocation = location;
}

void CSessionFactory::OnTimer(int nIDEvent)
{
	if (nIDEvent == TIMER_ID_CONNECT)
	{
		KillTimer(TIMER_ID_CONNECT);
		if (m_mapSession.GetCount() < m_nMaxSession && IsConnecterEnable() && !m_pConnecterManager->IsConnecting())
		{
			m_pConnecterManager->Connect(m_bRandomConnect);
		}
		return;
	}
	CEventHandler::OnTimer(nIDEvent);
}

int CSessionFactory::HandleEvent(int nEventID, unsigned int dwParam, void *pParam)
{
	switch (nEventID)
	{
	case UM_CONNECT_RESULT:
	{
		CChannel *pChannel = (CChannel *)pParam;
		if (pChannel != NULL)
		{
			m_pConnecterManager->OnConnected(pChannel);
			OnChannelCreated(pChannel, dwParam);
		}
		if (m_mapSession.GetCount() < m_nMaxSession && IsConnecterEnable())
		{
			m_pConnecterManager->ConnectNext();
		}
		else
		{
			m_pConnecterManager->CancelConnect();
		}
	}
	return 0;
	case UM_CONNECT_LOOP:
	{
		SetTimer(TIMER_ID_CONNECT, TIMER_INTERVAL_CONNECT);
	}
	return 0;
	case UM_LISTEN_RESULT:
	{
		CChannel *pChannel = (CChannel *)pParam;
		if (m_mapSession.GetCount() > m_nMaxSession || !IsListenerEnable())
		{
			pChannel->Disconnect();
		}
		else
		{
			OnChannelCreated(pChannel, dwParam);
		}
	}
	return 0;
	default:
		break;
	}
	return CEventHandler::HandleEvent(nEventID, dwParam, pParam);
}

void CSessionFactory::OnChannelCreated(CChannel *pChannel, unsigned int dwMark)
{
	CSession *pSession = CreateSession(pChannel, dwMark);
	if (pSession != NULL)
	{
		pSession->getReactor()->RegisterIO(pSession);
		pSession->RegisterSessionCallback(this);
		OnSessionConnected(pSession);
	}
}

void CSessionFactory::OnSessionConnected(CSession *pSession)
{
	REPORT_EVENT(LOG_INFO, "CSessionFactory", "[%s]:[%s][%d] Connected",
		m_sLocation.c_str(), pSession->GetChannel()->GetRemoteName(), pSession->GetSessionID());
	m_mapSession.Insert(pSession->GetSessionID(), pSession);
}

void CSessionFactory::OnSessionDisconnected(CSession *pSession, int nReason)
{
	REPORT_EVENT(LOG_INFO, "CSessionFactory", "[%s]:[%s][%d] Disconnected,reason=[0x%08x]",
		m_sLocation.c_str(), pSession->GetChannel()->GetRemoteName(), pSession->GetSessionID(), nReason);
	m_mapSession.Erase(pSession->GetSessionID());
	if (m_pConnecterManager != NULL)
	{
		m_pConnecterManager->OnDiscconnected(pSession->GetChannel());
	}
}

void CSessionFactory::OnSessionWarning(CSession *pSession, int nReason, int nParam)
{
}

void CSessionFactory::EnableConnecter(bool bEnable)
{
	m_bConnecterEnable = bEnable;
	OnTimer(TIMER_ID_CONNECT);
}

void CSessionFactory::EnableListener(bool bEnable)
{
	m_bListenerEnable = bEnable;
}

void CSessionFactory::SetConnectMode(bool bRandomConnect)
{
	m_bRandomConnect = bRandomConnect;
}

CSession *CSessionFactory::GetSession(unsigned int nSessionID)
{
	CSession **pFind = m_mapSession.Find(nSessionID);
	if (pFind == NULL)
	{
		return NULL;
	}
	return *pFind;
}
