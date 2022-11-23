#include "OfferLink.h"
#include "CachedFlow.h"
#include "FileFlow.h"
#include "BaseFunction.h"
#include "Config.h"
#include "monitorIndex.h"
#include "ServiceResponser.h"

COfferLink::COfferLink( CFlow* pSeriesFlow, const char* pAddress, int nLinkID, int nSubjectNo, UF_INT8 nSequenceNo)
	:COutFogServiceApi()
{
	m_pPubPackage = new CXTPPackage();
	m_pPubPackage->ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
	m_pSeriesFlow = pSeriesFlow;
	m_pSendClient = CXtpClient::CreateClientApi(NULL, pSeriesFlow, nSubjectNo, nSequenceNo);
	SetFrontIp(pAddress);
}

COfferLink::~COfferLink()
{
	delete m_pPubPackage;
}


void COfferLink::StartWork()
{
	m_pSendClient->Init();
}

bool COfferLink::ReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pField)
{
	if (pOutFogReqRef->Type == OUTFOG_TYPE_START)
	{
		StartWork();
		return true;
	}
	if (m_pSendClient == NULL)
	{
	 	REPORT_EVENT(LOG_CRITICAL, "OfferLink", "no m_pSendClient");
	 	return false;
	}
	m_pPubPackage->PreparePublish(pOutFogReqRef->MessageNo, OUTFOG_CHAIN_SINGLE);
	CopyHeadMeta(m_pPubPackage->GetXTPHeader(),pOutFogReqRef);
	m_pPubPackage->GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_KERNEL;
	m_pPubPackage->AddStream(pOutFogReqRef->FieldNo, (char *)pField, pOutFogReqRef->Length);
	m_pPubPackage->MakePackage();
	return m_pSendClient->RequestToDialogFlow(m_pPubPackage);
}

void COfferLink::SetFrontIp(const char *pIpFrontIP)
{
	m_pSendClient->RegisterReportServer(pIpFrontIP);
}
