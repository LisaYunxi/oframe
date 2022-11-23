#include "public.h"
#include "MultiFront.h"
#include "Config.h"

CMultiFront::CMultiFront(CReadFlow* pTradeResult,CFlow* pSeriesFlow,char* pFrontAddress,int nFrontid):COutFogServiceApi()
{
	m_pSeriesFlow=pSeriesFlow;
	m_rspPackage.ConstructAllocate(PACKAGE_MAX_SIZE,PACKAGE_RESERVE_SIZE);

	strcpy(m_FrontAddress,pFrontAddress);
	m_dwFrontNo = nFrontid;
	
	REPORT_EVENT(LOG_CRITICAL, "CMultiFront", "Open Port:%s",m_FrontAddress);
}

CMultiFront::~CMultiFront()
{	
}

const char* CMultiFront::getVersion()
{
	return "front";
}

bool CMultiFront::ReqField(const TOutFogMetaRef* pOutFogReqRef,const void *pStream)
{
	if(pOutFogReqRef->Type == OUTFOG_TYPE_START)
	{
	REPORT_EVENT(LOG_CRITICAL,"MultiFrontStartWork","Open Port!");
	}
	return true;
}
	


