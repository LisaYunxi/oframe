/////////////////////////////////////////////////////////////////////////
///@system OFKernelApi��ʵ��
///@file	Kernel.cpp
///@history 
///���һ�		�������ļ�
/////////////////////////////////////////////////////////////////////////

#include "OutFogServiceApi.h"
#include "KernelApp.h"
#include "CompositorApp.h"

class COfKernelApiImp:public COutFogServiceApi,CKernelAppCallback
{
public:
	COfKernelApiImp(COutFogServiceCallback *pCallback,int nServiceID);
	virtual ~COfKernelApiImp(void);
	virtual const char* getVersion();
	virtual bool Start();
	virtual bool ReqField(const TOutFogMetaRef* pOutFogReqRef,const void *pStream);
	virtual bool SyncReqField(const TOutFogMetaRef* pOutFogReqRef,const void *pReqStream,TOutFogMetaRef* pOutFogRspRef,void *pRspStream)
	{
		return true;
	}
	virtual bool RspPackage(CXTPPackage *pMessage);
private:
	CKernelApp* m_pKernelapp;
	int m_nKernelID;
	int m_nReliableMode;
	TOutFogMetaRef m_OutFogMetaRef;
};
