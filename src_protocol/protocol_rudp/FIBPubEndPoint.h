// FIBPubEndPoint.h: interface for the CFIBPubEndPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBPUBENDPOINT_H__93E840F9_7AEE_406D_A53E_283C938D4713__INCLUDED_)
#define AFX_FIBPUBENDPOINT_H__93E840F9_7AEE_406D_A53E_283C938D4713__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Protocol.h"
#include "FlowReader.h"
#include "XTPPackage.h"

class CFIBPubEndPoint 
{
public:
	CFIBPubEndPoint(CReadOnlyFlow *pFlow, WORD nSequenceSeries, int nStartId,
								 CProtocol *pSendProtocol);
	virtual ~CFIBPubEndPoint();
	int PublishSend();
	void MoveTo(int nOffset);
	inline WORD GetSequenceSeries();
private:
	CPackage *GetNextDataPackage();
private:
	WORD m_nSequenceSeries;
	CReadOnlyFlow *m_pFlow;
	CProtocol *m_pSendProtocol; 
	CXTPPackage m_SendPackage;
	CFlowReader m_flowReader;
};

inline WORD CFIBPubEndPoint::GetSequenceSeries()
{
	return m_nSequenceSeries;
}

#endif // !defined(AFX_FIBPUBENDPOINT_H__93E840F9_7AEE_406D_A53E_283C938D4713__INCLUDED_)
