#include "public.h"
#include "GatewayFront.h"

CGateWayFront::CGateWayFront(CFlow* pSeriesFlow, const char* pGateWayAddress, int nPortNumber, int nThreads) :COutFogServiceApi()
{
	if (nThreads <= 0)
		nThreads = 0;
	PRINT_TO_STD("CGateWayFront: GateWay's nThreads:%d", nThreads);
	m_pSeriesFlow = pSeriesFlow;
	m_pGateWayServer = CXtpServer::CreateServerApi(NULL, m_pSeriesFlow, nThreads);

	vector<string> sSplitThird = Txtsplit(pGateWayAddress, ":");
	int nPort = atoi(sSplitThird[2].c_str());
	char sFrontAddress[256];
	sprintf(sFrontAddress, "%s:%s:%d", sSplitThird[0].c_str(), sSplitThird[1].c_str(), nPort + nPortNumber - 1);

	m_pGateWayServer->Init(sFrontAddress);
	PRINT_TO_STD("CGateWayFront: GateWay's IPArddress:%s", sFrontAddress);
}

CGateWayFront::~CGateWayFront()
{
	delete m_pGateWayServer;
}

bool CGateWayFront::ReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pStream)
{
	if (pOutFogReqRef->Type == OUTFOG_TYPE_START)
	{
		if (m_pGateWayServer != NULL)
		{
			PRINT_TO_STD("CGateWayFront:FrontStartWork: Gateway Open Port!");
			m_pGateWayServer->EnableListener(true);
		}
	}
	return true;
}

void CGateWayFront::RegisterFlow(CReadFlow *pFlow, int nSubjectNo)
{
	if (m_pGateWayServer != NULL)
	{
		m_pGateWayServer->RegisterFlow(pFlow, nSubjectNo);
	}
}