#ifndef _FTDCPUBENDPOINT_H__
#define _FTDCPUBENDPOINT_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "Protocol.h"
#include "FlowReader.h"
#include "FTDCPackage.h"

class CFTDCPubEndPoint
{
public:
	CFTDCPubEndPoint(CReadFlow *pFlow, unsigned short nSequenceSeries, UF_INT8 nStartId, CProtocol *pSendProtocol, UF_INT8 nBusinessNo = UF_INT8_MAX, int nReaderTimes = 1);
	virtual ~CFTDCPubEndPoint();
	int PublishSend();
	void MoveTo(UF_INT8 nOffset);
	inline unsigned short GetSequenceSeries();
private:
	unsigned short m_nSequenceSeries;
	CReadFlow *m_pFlow;
	CProtocol *m_pSendProtocol;
	CFTDCPackage m_SendPackage;
	CFlowReader m_flowReader;
};

inline unsigned short CFTDCPubEndPoint::GetSequenceSeries()
{
	return m_nSequenceSeries;
}

#endif
