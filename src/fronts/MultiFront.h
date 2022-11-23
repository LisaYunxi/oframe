#ifndef _MULTI_FRONT_H_
#define _MULTI_FRONT_H_

#include "CachedFlow.h"
#include "SelectReactor.h"
#include "Config.h"
#include "LinkManager.h"
#include "printinfo.h"
#include "OutFogServiceApi.h"

class CMultiFront:public COutFogServiceApi
{
public:
	//pTradeResult是前置的输入,pTSeries是前置的输出
	CMultiFront(CReadFlow* pTradeResult,CFlow* pSeriesFlow,char* pFrontAddress,int nFrontid);	
	virtual ~CMultiFront();

	virtual const char* getVersion();
	virtual bool Start(){return true;};
	virtual bool ReqField(const TOutFogMetaRef* pOutFogReqRef,const void *pStream);
	virtual bool SyncReqField(const TOutFogMetaRef* pOutFogReqRef,const void *pReqStream,TOutFogMetaRef* pOutFogRspRef,void *pRspStream)
	{
		return true;
	}
private:

	CFlow* m_pSeriesFlow;
	CXTPPackage m_rspPackage;
	int m_dwFrontNo;
	char m_FrontAddress[512];
};

#endif
