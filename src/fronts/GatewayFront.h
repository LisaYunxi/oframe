#ifndef __CGateWayFront_H__
#define __CGateWayFront_H__

#include "SessionFactory.h"
#include "TcpXtpSession.h"
#include "Reactor.h"
#include "CachedFlow.h"
#include "LinkManager.h"
#include "XtpServer.h"
#include "Config.h"

class CGateWayFront :public COutFogServiceApi
{
public:
	CGateWayFront(CFlow* pSeriesFlow,const char* pGateWayAddress, int nPortNumber, int nThreads);
	~CGateWayFront();
	virtual const char* getVersion(){return "CGateWayFront";};
	virtual bool Start(){return true;};
	virtual bool ReqField(const TOutFogMetaRef* pOutFogReqRef,const void *pStream);
	virtual bool SyncReqField(const TOutFogMetaRef* pOutFogReqRef,const void *pReqStream,TOutFogMetaRef* pOutFogRspRef,void *pRspStream)
	{
		return true;
	}
	void RegisterFlow(CReadFlow *pFlow, int nSubjectNo);
private:
	CXtpServer *m_pGateWayServer;
	CFlow *m_pSeriesFlow;
};
#endif
