#ifndef __INNERTRADE_LINK_H_
#define __INNERTRADE_LINK_H_

#include "platform.h"
#include "OutFogServiceApi.h"
#include "XTPPackage.h"
#include "FlowReader.h"
#include "Flow.h"
#include "XtpClient.h"
#include "Config.h"

//����ϯλ�ӿ�
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
	bool m_bLogin;//�Ƿ��¼�ɹ�
	CXtpClient *m_pSendClient;

private:
	CXTPPackage  *m_pPubPackage;        //д���õİ�
	CFlow *m_pSeriesFlow;
};

#endif
