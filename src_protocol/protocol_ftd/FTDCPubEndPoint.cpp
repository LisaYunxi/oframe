#include "public.h"
#include "FTDCPubEndPoint.h"

CFTDCPubEndPoint::CFTDCPubEndPoint(CReadFlow *pFlow, unsigned short nSequenceSeries, UF_INT8 nStartId, CProtocol *pSendProtocol, UF_INT8 nBusinessNo, int nReaderTimes)
{
	m_pFlow = pFlow;
	m_nSequenceSeries = nSequenceSeries;
	m_pSendProtocol = pSendProtocol;
	m_SendPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
	m_flowReader.AttachFlow(pFlow, nStartId, nBusinessNo, nReaderTimes);
}

CFTDCPubEndPoint::~CFTDCPubEndPoint()
{
}

int CFTDCPubEndPoint::PublishSend()
{
	int nCount = 0;
	for (; nCount < 40; nCount++)
	{
		if (!m_flowReader.GetNext(&m_SendPackage))
			break;
		if (m_flowReader.GetBusinessNo() != UF_INT8_MAX)
		{
			if (m_SendPackage.GetFTDCHeader()->BusinessNo < m_flowReader.GetBusinessNo())
				continue;
			else
				m_flowReader.SetBusinessNo(UF_INT8_MAX);
		}
		m_SendPackage.GetFTDCHeader()->SequenceNo = m_flowReader.GetId();
		m_SendPackage.GetFTDCHeader()->SubjectNo = m_nSequenceSeries;
		if (m_pSendProtocol->Send(&m_SendPackage) != 0)
			break;
	}
	return nCount;
}

void CFTDCPubEndPoint::MoveTo(UF_INT8 nOffset)
{
	if (m_flowReader.GetId() == nOffset)
		return;
	m_flowReader.SetId(nOffset);
}