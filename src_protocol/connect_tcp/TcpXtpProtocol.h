#ifndef __CTcpXtpProtocol_H_
#define __CTcpXtpProtocol_H_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Protocol.h"
#include "XTPPackage.h"
#include "ReadFlow.h"
#include "FlowReader.h"

class CTcpXtpProtocol : public CProtocol  
{
public:
	CTcpXtpProtocol(CReactor *pReactor,unsigned int nSubjectNo = 0);
	virtual ~CTcpXtpProtocol();
	int PublishSend();
	void PublishFlow(CReadFlow *pFlow, UF_INT8 nStartId);
	void DetachFlow();
	CXTPPackage *GetNextDataPackage();
public:
	CFlowReader m_flowReader;
	CReadFlow *m_pFlow;
	CXTPPackage	m_SendPackage;
	unsigned int m_nSubjectNo;
};
#endif
