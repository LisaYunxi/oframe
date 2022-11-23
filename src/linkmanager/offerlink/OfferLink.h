#ifndef __INNERTRADE_LINK_H_
#define __INNERTRADE_LINK_H_

#include "platform.h"
#include "OutFogServiceApi.h"
#include "XTPPackage.h"
#include "FlowReader.h"
#include "Flow.h"
#include "XtpClient.h"
#include "Config.h"

//交易席位接口
class COfferLink : public COutFogServiceApi
{
public:
	COfferLink(CFlow *pSeriesFlow, const char * pAddress,int nLinkID,int nSubjectNo, UF_INT8 nSequenceNo);
	~COfferLink();
	
	void StartWork();
//	virtual bool handleMessage(CXTPPackage *pPackage);
	virtual bool ReqField(const TOutFogMetaRef* pOutFogReqRef,const void *pField);
	virtual bool SyncReqField(const TOutFogMetaRef* pOutFogReqRef,const void *pReqStream,TOutFogMetaRef* pOutFogRspRef,void *pRspStream)
	{
		return true;
	}
	void SetFrontIp(const char *pIpFrontIP);
	bool GetStatus(){return true;};
	virtual const char* getVersion()
	{
		return "offerlink";
	};	
	virtual bool Start()
	{
		return true;
	};	
	bool m_bLogin;//是否登录成功
	CXtpClient *m_pSendClient;

private:
	CXTPPackage  *m_pPubPackage;        //写流用的包
	CFlow *m_pSeriesFlow;
};

#endif
