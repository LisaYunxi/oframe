#ifndef _KERNEL_FRONT_H_
#define _KERNEL_FRONT_H_

#include "CachedFlow.h"
#include "ReadFlow.h"
#include "FlowManager.h"
#include "FTDCPackage.h"
#include "SelectReactor.h"
#include "Config.h"
#include "LinkManager.h"
#include "printinfo.h"
//#include "OutFogServiceApi.h"

class CFlowDispatcher:public CThread
{
public:
	//pTradeResult是前置的输入,pTSeries是前置的输出
	CFlowDispatcher(CReadFlow* pTradeResult, CFlowManager *pFlowManager, int nKernelid);
	virtual ~CFlowDispatcher();
	//virtual bool HandleOtherTask();
	void HandleMessage(CXTPPackage *pMessage);
	bool HandleNotify(CXTPPackage *pMessage);
	virtual void Run();

private:
	CXTPPackage m_Package;
	CFTDCPackage m_rspPackage;

	CFlowReader m_ResultReader;
	CFlowManager *m_pFlowManager;

	int m_nKernelid;
};

#endif
