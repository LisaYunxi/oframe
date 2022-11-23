#include "CompositorApp.h"

class CCompStateMonitorIndex : public CMonitorIndex
{
public:
	CCompStateMonitorIndex(const char *name, CCompServer *pCompServer, int frequncy) :CMonitorIndex(frequncy)
	{
		m_name = name;
		m_pCompServer = pCompServer;
	}
	virtual void report(CProbeLogger *pProbeLogger)
	{
		pProbeLogger->SendProbeMessage(m_name, m_pCompServer->GetCurrStateID());
	}
private:
	const char *m_name;
	CCompServer *m_pCompServer;
};

const unsigned int sequence_APP_ID_BASE = 0x1100;

CCompReactor::CCompReactor(int nsequenceID, CStatusCtrlInterfaces *pStatusCtrlInf, CCompFlowFactory *pFlowFactory,const char *sPreName)
{
	m_nsequenceID = nsequenceID;
	m_pCompServer = NULL;
	m_pStatusCtrlEng = pStatusCtrlInf;
	m_pFlowFactory = pFlowFactory;
	strncpy(m_sPreName, sPreName, strlen(sPreName) + 1);
}

bool CCompReactor::InitInstance()
{
	m_pCompServer = new CCompServer(this, m_pFlowFactory, 1000, sequence_APP_ID_BASE + m_nsequenceID);
	m_pCompServer->SetStatusCtrlEng(m_pStatusCtrlEng);
	new CCompStateMonitorIndex("sequenceState", m_pCompServer, 20);

	char tmpbuffer[128] = { 0 };
	sprintf(tmpbuffer, "guard_%s", m_sPreName);
	string ArbPort = GetConfigString(NetworkConfigFile, tmpbuffer);
	m_pCompServer->RegisterConnecter(ArbPort.c_str());
	REPORT_EVENT(LOG_CRITICAL, "CSequenceReactor", "Connect Guard Port:%s", ArbPort.c_str());

	if (m_nsequenceID == 1)
	{
		sprintf(tmpbuffer, "sequence_%s_1", m_sPreName);
		string ComPort = GetConfigString(NetworkConfigFile, tmpbuffer);
		m_pCompServer->RegisterHotlineListen(ComPort.c_str());
		REPORT_EVENT(LOG_CRITICAL, "CSequenceReactor", "Listen Sequence1 Port:%s", ComPort.c_str());


		sprintf(tmpbuffer, "sequence_%s_2", m_sPreName);
		ComPort = GetConfigString(NetworkConfigFile, tmpbuffer);
		m_pCompServer->RegisterHotlineListen(ComPort.c_str());
		REPORT_EVENT(LOG_CRITICAL, "CSequenceReactor", "Listen Sequence2 Port:%s", ComPort.c_str());
	}
	else
	{
		sprintf(tmpbuffer, "sequence_%s_2", m_sPreName);
		string ComPort = GetConfigString(NetworkConfigFile, tmpbuffer);
		m_pCompServer->RegisterHotlineListen(ComPort.c_str());
		REPORT_EVENT(LOG_CRITICAL, "CSequenceReactor", "Listen Sequence2 Port:%s", ComPort.c_str());


		sprintf(tmpbuffer, "sequence_%s_1", m_sPreName);
		ComPort = GetConfigString(NetworkConfigFile, tmpbuffer);
		m_pCompServer->RegisterHotlineListen(ComPort.c_str());
		REPORT_EVENT(LOG_CRITICAL, "CSequenceReactor", "Listen Sequence1 Port:%s", ComPort.c_str());
	}

	int nTimeOut = GetConfigInt(NetworkConfigFile, "GuardTimeout");
	if (nTimeOut > 0)
		m_pCompServer->SetArbHeartbeatTimeout(nTimeOut);

	nTimeOut = GetConfigInt(NetworkConfigFile, "SequenceTimeout");
	if (nTimeOut > 0)
		m_pCompServer->SetHotlineHeartbeatTimeout(nTimeOut);

	m_bShouldRun = true;

	m_pCompServer->Start();

	return true;
}

void CCompReactor::ExitInstance()
{
	delete m_pCompServer;
	delete m_pFlowFactory;
}

