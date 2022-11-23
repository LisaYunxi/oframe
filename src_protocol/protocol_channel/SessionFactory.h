// SessionFactory.h: interface for the CSessionFactory class.
//////////////////////////////////////////////////////////////////////

#ifndef AFX_SESSIONFACTORY_H__
#define AFX_SESSIONFACTORY_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "SelectReactor.h"
#include "ListenCtrl.h"
#include "Session.h"
#include "NetworkFactory.h"
#include "HashMap.h"

const int UM_LISTEN_RESULT			= 0x40901 + SM_USER;
const int UM_CONNECT_RESULT			= 0x40902 + SM_USER;
const int UM_TRY_CONNECT			= 0x40903 + SM_USER;
const int UM_CONNECT_LOOP			= 0x40904 + SM_USER;

const int TIMER_ID_CONNECT = 1;

class CSessionConnecter
{
public:
	CSessionConnecter(const char *location, unsigned int dwMark);
	void CheckChannel(CChannel *pChannel);
	void OnConnected(CChannel *pChannel);
	CChannel *GetChannel();
	CChannel *Connect();
	CServiceName *GetServiceName();
	unsigned int GetMark();
private:
	CServiceName m_ServiceName;
	CChannel *m_pChannel;
	CEventHandler *m_pSessionFactory;
	unsigned int m_dwMark;
};

class CConnecterManager : public CEventHandler
{
public:
	CConnecterManager(CReactor *pReactor, CEventHandler *pSessionFactory);
	~CConnecterManager();
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);
	void AppendConnecter(CSessionConnecter *pConnecter);
	void Clear();
	int Connect(bool bRandom);
	int ConnectNext();
	void OnConnected(CChannel *pChannel);
	void OnDiscconnected(CChannel *pChannel);
	void CancelConnect();
	bool IsConnecting();
private:
	void RandomConnecters();
private:
	vector<CSessionConnecter *> m_connecters;
	CEventHandler *m_pSessionFactory;
	int m_nConnecterPos;
};

class CSessionListener : public CListenCtrl
{
public:
	CSessionListener(CReactor *pReactor, CEventHandler *pSessionFactory, CServerBase *pServer,unsigned int dwMark);
	int OnConnected(CChannel *pChannel);
private:
	CEventHandler *m_pSessionFactory;
	unsigned int m_dwMark;
};

class CSessionFactory : public CEventHandler, public CSessionCallback
{
public:
	CSessionFactory(CReactor *pReactor, int nMaxSession);
	virtual ~CSessionFactory();
	virtual void Start();
	void Stop();
	void DisconnectAll(int nReason);
	virtual void OnTimer(int nIDEvent);
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);
	void RegisterConnecter(const char *location, unsigned int dwMark = 0);
	void RegisterListener(const char *location, unsigned int dwMark = 1);
	void EnableConnecter(bool bEnable);
	void EnableListener(bool bEnable);
	void SetConnectMode(bool bRandomConnect);
	inline bool IsConnecterEnable();
	inline bool IsListenerEnable();
	CSession *GetSession(unsigned int nSessionID);
protected:
	virtual CSession *CreateSession(CChannel *pChannel, unsigned int dwMark) = 0;
	virtual void OnSessionConnected(CSession *pSession);
	virtual void OnSessionDisconnected(CSession *pSession, int nReason);
	virtual void OnSessionWarning(CSession *pSession, int nReason, int nParam);	

	void OnChannelCreated(CChannel *pChannel, unsigned int dwMark);
protected:
	CSelectReactor m_reactorConnect;
	vector<CSessionListener *> m_listeners;
	CConnecterManager *m_pConnecterManager;
	int m_nMaxSession;
	
	bool m_bRandomConnect;
	bool m_bConnecterEnable;
	bool m_bListenerEnable;

	typedef CHashMap<unsigned int, CSession *, HashInt> CSessionMap;
	CSessionMap m_mapSession;
	string m_sLocation;
};

inline bool CSessionFactory::IsConnecterEnable()
{
	return m_bConnecterEnable;
}

inline bool CSessionFactory::IsListenerEnable()
{
	return m_bListenerEnable;
}

#endif
