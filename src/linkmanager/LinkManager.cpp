#include "public.h"
#include "LinkManager.h"
#include "FieldDescribe.h"
#include "FileFlow.h"
#include "CFilelogService.h"
#include "BaseFunction.h"
#include "ServiceResponser.h"

#ifdef LINUX
#include <poll.h>
#endif

#include "OfferLink.h"

CHttpSessionMap g_SessionMap;
CRWLock g_SessionMapRWLock;

int g_nMaxSpeed = 50;
int g_nMaxSubTopics = 10;
int g_nMaxSendPackages = 32;
int g_nMaxSessions = 512;
int g_nMaxPackages = 3000;
bool g_bLowLatency = false;
int g_nWSHeartBeat = 60;

CLinkManager::CLinkManager()
{
	clear();
	//	m_pPubPackage = new CXTPPackage();
	//	m_pPubPackage->ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);

	m_pLinkManagerRsp = new CLinkManagerRsp();
	m_pServiceLoad = new COutFogService();

	m_nFlowCacheSize = 0x100000;
	int nFlowCacheSize = GetConfigInt(INI_FILE_NAME, "FlowCacheSize");
	if (nFlowCacheSize > 0)
		m_nFlowCacheSize = nFlowCacheSize;

	CFlow* pFlow = new COneTimeFlow();
	//if (!strcmp(m_pConfig->getConfig("PersistOtherFlows"), "yes"))
	if(GetConfigString(INI_FILE_NAME,"PersistOtherFlows")=="yes")
	{
		//CFileFlow* pFileFlow = new CFileFlow("input_link_mangerrsp", m_pConfig->getConfig("FlowPath"), false, atoi(m_pConfig->getConfig("PersistGap")));
		CFileFlow* pFileFlow = new CFileFlow("input_link_mangerrsp", GetConfigString(INI_FILE_NAME,"FlowPath").c_str(), false, GetConfigInt(INI_FILE_NAME, "PersistGap"));
		pFlow->AttachUnderFlow(pFileFlow);
	}

	setLinkApi(LINK_MANAGER_RSP, m_pLinkManagerRsp, pFlow, NULL);
	setLinkInitOver(LINK_MANAGER_RSP);

	m_iCurrLinkItem = begin();
	m_pCurrLinkItem = NULL;
	m_nCurrCount = 0;

	g_nMaxSpeed = GetConfigInt(INI_FILE_NAME, "MaxSpeed");
	if (g_nMaxSpeed <= 0)
		g_nMaxSpeed = 50;
	REPORT_EVENT(LOG_CRITICAL, "LinkManager", "Front MaxSpeed is: %d/s", g_nMaxSpeed);

	g_nMaxSubTopics = GetConfigInt(INI_FILE_NAME, "MaxSubTopics");
	if (g_nMaxSubTopics <= 0)
		g_nMaxSubTopics = 10;
	REPORT_EVENT(LOG_CRITICAL, "LinkManager", "Front MaxSubTopics is: %d", g_nMaxSubTopics);

	g_nMaxSendPackages = GetConfigInt(INI_FILE_NAME, "MaxSendPackages");
	if (g_nMaxSendPackages <= 0)
		g_nMaxSendPackages = 32;
	REPORT_EVENT(LOG_CRITICAL, "LinkManager", "Front MaxSendPackages is: %d", g_nMaxSendPackages);

	g_nMaxSessions = GetConfigInt(INI_FILE_NAME, "MaxSessions");
	if (g_nMaxSessions <= 0)
		g_nMaxSessions = 512;
	REPORT_EVENT(LOG_CRITICAL, "LinkManager", "Front MaxSessions is: %d", g_nMaxSessions);

	g_nMaxPackages = GetConfigInt(INI_FILE_NAME, "MaxPackages");
	if (g_nMaxPackages <= 0)
		g_nMaxPackages = 3000;
	REPORT_EVENT(LOG_CRITICAL, "LinkManager", "Front MaxPackages is: %d", g_nMaxPackages);

	g_nWSHeartBeat = GetConfigInt(INI_FILE_NAME, "WSHeartBeat");
	if (g_nWSHeartBeat <= 0)
		g_nWSHeartBeat = 60;
	REPORT_EVENT(LOG_CRITICAL, "LinkManager", "Front WSHeartBeat is: %d", g_nWSHeartBeat);

	if (GetConfigString(INI_FILE_NAME, "LowLatency") == "yes")
		g_bLowLatency = true;
	REPORT_EVENT(LOG_CRITICAL, "LinkManager", "LowLatency=%d", g_bLowLatency);
}

CLinkManager::~CLinkManager()
{
}

bool CLinkManager::RspError(int nLinkID)
{
	//报单错误回报
//	CRspInfoField Rsp;
//	Rsp.ErrorID = "20";
//	Rsp.ErrorMsg = "通道不可用";

	//组包
	//	m_pPubPackage->PreparePublish(TID_RspError);
//	m_pPubPackage->GetXTPHeader()->ServiceNo = nLinkID;
	//	m_pPubPackage->SetSubjectNo(DIALOG_FLOW);
	//	XTP_ADD_FIELD(m_pPubPackage,&Rsp);	
	//	m_pLinkManagerRsp->OnRspMessage(m_pPubPackage);

		//使用了rsp回复到核心，这里必然认定req是正确的，通过rsperror来说明订单的错误
	return true;
}

bool CLinkManager::ReqField(CXTPPackage *pPackage)
{
	int nLinkID = pPackage->GetXTPHeader()->BusinessNo;
	if (0 == nLinkID)
	{
		CLinkItemMap::iterator iter = begin();
		for (; iter != end(); iter++)
		{
			if (iter->second->m_pSendClient != NULL)
			{
				pPackage->GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
				pPackage->MakePackage();
				iter->second->m_pSendClient->RequestToDialogFlow(pPackage);
			}
		}
	}
	else
	{
		CLinkItemMap::iterator iter = find(nLinkID);
		if (iter == end())
			return false;

		if (iter->second->m_pSendClient != NULL)
		{
			pPackage->GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
			pPackage->MakePackage();
			iter->second->m_pSendClient->RequestToDialogFlow(pPackage);
		}
	}
	return true;
}

bool CLinkManager::ReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pStream)
{
	if (pOutFogReqRef->Type == OUTFOG_TYPE_INITLINK)
	{
		COutFogServiceApi* pLinkApi = initLink(pOutFogReqRef->SubIndexName, pOutFogReqRef->BusinessNo);
		if (pLinkApi != NULL)
		{
			pLinkApi->ReqField(pOutFogReqRef, pStream);
			return true;
		}
		else
		{
			return false;
		}
	}

	if (pOutFogReqRef->Type == OUTFOG_TYPE_SUBTOPIC)
	{
		setLinkInitOver(pOutFogReqRef->BusinessNo);
		return true;
	}

	// 	if(pPackage->GetXTPHeader()->DirectionType == ServiceToKernel)
	// 	{
	// 		CLinkItem* pLinkItem = getLinkItem(LINK_MANAGER_RSP);
	// 		pPackage->MakePackage();
	// 		pLinkItem->m_pSeriesFlow->Append(pPackage->Address(),pPackage->Length());
	// 		return true;
	// 	}

		//如果没有指定通道，全部发送一遍
	int nLinkID = pOutFogReqRef->BusinessNo;
	if (0 == nLinkID)
	{
		CLinkItemMap::iterator iter = begin();
		for (; iter != end(); iter++)
		{
			COutFogServiceApi *pLinkApi = iter->second->m_pLinkApi;
			if (pLinkApi != NULL)
				pLinkApi->ReqField(pOutFogReqRef, pStream);
		}
	}
	else
	{
		COutFogServiceApi* pLinkApi = getLinkApi(pOutFogReqRef->BusinessNo);
		if (pLinkApi == NULL)
		{
			RspError(pOutFogReqRef->BusinessNo);
			return false;
		}
		pLinkApi->ReqField(pOutFogReqRef, pStream);
	}

	//如果是初始化关系
	return true;
}

bool CLinkManager::RspField(const TOutFogMetaRef* pOutFogRspRef, const void *pStream)
{
	CLinkItem * pLinkItem = getLinkItem(pOutFogRspRef->BusinessNo);
	if (pLinkItem != NULL)
	{
		CXTPPackage	PubPackage;
		PubPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
		PubPackage.PreparePublish(pOutFogRspRef->MessageNo, OUTFOG_CHAIN_LAST);
		CopyHeadMeta(PubPackage.GetXTPHeader(), pOutFogRspRef);
		PubPackage.AddStream(pOutFogRspRef->FieldNo, (char *)pStream, pOutFogRspRef->Length);
		PubPackage.MakePackage();
		if (pOutFogRspRef->Type == OUTFOG_TYPE_PUBLISH)
			pLinkItem->m_pSeriesFlow->Append(PubPackage.Address(), PubPackage.Length());
		else
			pLinkItem->m_pRspFlow->Append(PubPackage.Address(), PubPackage.Length());
		return true;
	}
	return false;
}

bool CLinkManager::GetLinkMessage(CLinkItem *pLinkItem, CXTPPackage *pPackage)
{
	bool ret = false;
	if (pLinkItem->m_pRspReader != NULL)
	{
		if (pLinkItem->m_pRspReader->GetNext(pPackage))
			return true;
	}

	if (pLinkItem->m_bBeginSeries)
	{
		if (pLinkItem->m_pSeriesReader->GetNext(pPackage))
			return true;
	}
	return false;
}

bool CLinkManager::GetNextMessage(CXTPPackage *pPackage)
{
	if (m_pCurrLinkItem != NULL)
	{
		if (m_nCurrCount < 32)
		{
			if (GetLinkMessage(m_pCurrLinkItem, pPackage))
			{
				m_nCurrCount++;
				return true;
			}
		}
	}

	//如果此时没有返回，说明需要找到下一个Link
	if (m_iCurrLinkItem == end())
		m_iCurrLinkItem = begin();
	else
		m_iCurrLinkItem++;

	if (m_iCurrLinkItem == end())
		m_pCurrLinkItem = NULL;
	else
		m_pCurrLinkItem = m_iCurrLinkItem->second;
	m_nCurrCount = 0;

	return false;
}

COutFogServiceApi* CLinkManager::initOfferLink(int nLinkID, const char* ipAddress, int nSubjectNo, UF_INT8 nSequenceNo)
{
	CFlow *pFlow = new COneTimeFlow();
	if (GetConfigString(INI_FILE_NAME, "PersistOtherFlows") == "yes")
	{
		char flowname[256] = { 0 };
		sprintf(flowname, "input_link_%d_sub%d", nLinkID, nSubjectNo);
		CFileFlow* pFileFlow = new CFileFlow(flowname, GetConfigString(INI_FILE_NAME, "FlowPath").c_str(), false, GetConfigInt(INI_FILE_NAME, "PersistGap"));
		pFlow->AttachUnderFlow(pFileFlow);
	}

	COfferLink *pTradeLink = NULL;
	pTradeLink = new COfferLink(pFlow, ipAddress, nLinkID, nSubjectNo, nSequenceNo);
	if (NULL == pTradeLink)
	{
		REPORT_EVENT(LOG_CRITICAL, "initLink", "new COfferLink failed");
		return NULL;
	}
	else
	{
		REPORT_EVENT(LOG_CRITICAL, "initLink", "new COfferLink[%s] success!", ipAddress);
	}

	CLinkItem* pLinkItem = setLinkApi(nLinkID, pTradeLink, pFlow, NULL);
	pLinkItem->m_pSendClient = pTradeLink->m_pSendClient;

	return pTradeLink;
}

COutFogServiceApi* CLinkManager::initLink(const char* sLinkDll, int nLinkID)
{
	CLinkItemMap::iterator iter = find(nLinkID);
	if (iter != end())
	{
		REPORT_EVENT(LOG_CRITICAL, "setLinkApi", "nLinkID[%d] is Exist:%d", nLinkID);
		return NULL;
	}

	COutFogServiceApi *pTradeLink = NULL;

	pTradeLink = m_pServiceLoad->getOutFogService(sLinkDll);

	if (pTradeLink == NULL)
	{
		REPORT_EVENT(LOG_CRITICAL, "setLinkApi", "can not load Dll: %s", sLinkDll);
		return NULL;
	}

	CFlow *pQryFlow = new COneTimeFlow();
	if (GetConfigString(INI_FILE_NAME, "PersistOtherFlows") == "yes")
	{
		char flowname[256] = { 0 };
		sprintf(flowname, "input_link_%d_rsp", nLinkID);
		CFileFlow* pFileFlow = new CFileFlow(flowname, GetConfigString(INI_FILE_NAME, "FlowPath").c_str(), false, GetConfigInt(INI_FILE_NAME, "PersistGap"));
		pQryFlow->AttachUnderFlow(pFileFlow);
	}

	setLinkApi(nLinkID, pTradeLink, NULL, pQryFlow);

	return pTradeLink;
}

COutFogServiceApi *CLinkManager::getLinkApi(int nLinkID)
{
	CLinkItemMap::iterator iter = find(nLinkID);
	if (iter == end())
		return NULL;
	return iter->second->m_pLinkApi;
}

CLinkItem* CLinkManager::getLinkItem(int nLinkID)
{
	CLinkItemMap::iterator iter = find(nLinkID);

	if (iter == end())
	{
		REPORT_EVENT(LOG_CRITICAL, "getLinkItem", "can not find LinkItem[%d]", nLinkID);
		return NULL;
	}
	return iter->second;
}

bool CLinkManager::setLinkInitOver(int nLinkID)
{
	CLinkItemMap::iterator iter = find(nLinkID);

	if (iter == end())
	{
		REPORT_EVENT(LOG_CRITICAL, "setLinkInitOver", "can not find LinkItem[%d]", nLinkID);
		return false;
	}
	else
	{
		iter->second->m_bBeginSeries = true;
		return true;
	}
}

CLinkItem* CLinkManager::setLinkApi(int LinkID, COutFogServiceApi* pLinkApi, CFlow *pFlow, CFlow *pRspFlow)
{
	CLinkItem* pLinkItem = new CLinkItem();
	pLinkItem->m_pSeriesFlow = NULL;
	pLinkItem->m_pRspFlow = NULL;
	pLinkItem->m_pSeriesReader = NULL;
	pLinkItem->m_pRspReader = NULL;
	pLinkItem->m_bBeginSeries = false;
	pLinkItem->m_pLinkApi = pLinkApi;
	pLinkItem->m_pSendClient = NULL;

	if (pFlow == NULL)
	{
		CFlow* pFlow = new COneTimeFlow();
		if (GetConfigString(INI_FILE_NAME, "PersistOtherFlows") == "yes")
		{
			char sFlowName[128];
			sprintf(sFlowName, "input_link_%d_push", LinkID);
			CFileFlow* pFileFlow = new CFileFlow(sFlowName, GetConfigString(INI_FILE_NAME, "FlowPath").c_str(), false, GetConfigInt(INI_FILE_NAME, "PersistGap"));
			pFlow->AttachUnderFlow(pFileFlow);
		}
		pLinkItem->m_pSeriesFlow = pFlow;
	}
	else
	{
		pLinkItem->m_pSeriesFlow = pFlow;
	}
	pLinkItem->m_pSeriesReader = new CFlowReader();
	pLinkItem->m_pSeriesReader->AttachFlow(pLinkItem->m_pSeriesFlow, 0);

	pLinkItem->m_pRspFlow = pRspFlow;
	pLinkItem->m_nLinkID = LinkID;
	pLinkApi->setServiceCallback(this);

	if (pLinkItem->m_pRspFlow != NULL)
	{
		pLinkItem->m_pRspReader = new CFlowReader();
		pLinkItem->m_pRspReader->AttachFlow(pLinkItem->m_pRspFlow, 0);
	}
	else
	{
		pLinkItem->m_bBeginSeries = true;
	}

	this->insert(CLinkItemMap::value_type(LinkID, pLinkItem));
	return pLinkItem;
}

void CLinkManager::startWork()
{
	CLinkItemMap::iterator iter = begin();
	for (; iter != end(); iter++)
	{
		TOutFogMetaRef OutFogMetaRef;
		OutFogMetaRef.Type = OUTFOG_TYPE_START;
		COutFogServiceApi *pLinkApi = iter->second->m_pLinkApi;
		if (pLinkApi != NULL)
			pLinkApi->ReqField(&OutFogMetaRef, NULL);
	}
}

void CLinkManager::OnTimer()
{
	CLinkItemMap::iterator iter = begin();
	for (; iter != end(); iter++)
	{
		REPORT_EVENT(LOG_WARNING, "CLinkManager", "LinkItem[%d]FlowUnRead[%0.f]", iter->second->m_nLinkID, iter->second->m_pSeriesReader->GetUnRead());
	}
}