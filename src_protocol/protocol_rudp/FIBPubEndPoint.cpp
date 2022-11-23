// FIBPubEndPoint.cpp: implementation of the CFIBPubEndPoint class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "FIBPubEndPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFIBPubEndPoint::CFIBPubEndPoint(CReadOnlyFlow *pFlow, WORD nSequenceSeries, int nStartId,
								 CProtocol *pSendProtocol)
{
	m_pFlow = pFlow;
	m_nSequenceSeries = nSequenceSeries;
	m_pSendProtocol = pSendProtocol;
	m_SendPackage.ConstructAllocate(XTP_PACKAGE_MAX_SIZE+XTPHLEN, 1000);
	m_flowReader.AttachFlow(pFlow, nStartId);
}

CFIBPubEndPoint::~CFIBPubEndPoint()
{
}

CPackage *CFIBPubEndPoint::GetNextDataPackage()
{
	m_SendPackage.AllocateMax();
	if(m_flowReader.GetNext(&m_SendPackage))
	{
		TXTPHeader  *pHeader = m_SendPackage.GetXTPHeader();
		pHeader->SequenceNo = m_flowReader.GetId();
		pHeader->SubjectNo = m_nSequenceSeries;
		return &m_SendPackage;
	}
	return NULL;
}

int CFIBPubEndPoint::PublishSend()
{
	int nCount = 0;
	for(; nCount<40; nCount++)
	{
		CPackage *pPackage = GetNextDataPackage();	
		if (pPackage == NULL)
		{
			break;
		}
		if (m_pSendProtocol->Send(pPackage) != 0)
		{
			break;
		}			
	}
	return nCount;
}

void CFIBPubEndPoint::MoveTo(int nOffset)
{
	if (m_flowReader.GetId() == nOffset)
	{
		return;
	}
	m_flowReader.SetId(nOffset);
}
