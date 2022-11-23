#include "public.h"
#include "ShmFront.h"
#include "Config.h"

CShmFront::CShmFront(CFlow* pSeriesFlow, char* pFrontAddress, int nFrontid) :COutFogServiceApi()
{
	m_pSeriesFlow = pSeriesFlow;
	m_rspPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);

	strcpy(m_FrontAddress, pFrontAddress);
	m_dwFrontNo = nFrontid;

	REPORT_EVENT(LOG_CRITICAL, "CShmFront", "Open Port:%s", m_FrontAddress);
}

CShmFront::~CShmFront()
{
}

const char* CShmFront::getVersion()
{
	return "front";
}

bool CShmFront::ReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pStream)
{
	if (pOutFogReqRef->Type == OUTFOG_TYPE_START)
	{
		REPORT_EVENT(LOG_CRITICAL, "ShmFrontStartWork", "Open Port!");
	}
	return true;
}


