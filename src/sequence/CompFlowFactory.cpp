#include "public.h"
#include "CompFlowFactory.h"
#include "FileFlow.h"
#include "XTPPackage.h"
#include "FlowReader.h"
#include "monitorIndex.h"

CCompFlowFactory::CCompFlowFactory(CCachedFlow *pPreSeries)
{
	m_pPreSeries = pPreSeries;
//	m_pSeries = pPreSeries->GetUnderFlow();	
//	new CLongPtrMonitorIndex("squenceLen",m_pSeries->GetCountPointer(),20);
}

CCompFlowFactory::~CCompFlowFactory()
{
	delete m_pPreSeries;
}

//void CCompFlowFactory::Rollback()
//{
//	UF_INT8 nConfimedCount = m_pSeries->GetCount();
//	m_pPreSeries->Truncate(nConfimedCount);
//}

