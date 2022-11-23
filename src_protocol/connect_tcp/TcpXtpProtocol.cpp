#include "public.h"
#include "TcpXtpProtocol.h"

CTcpXtpProtocol::CTcpXtpProtocol(CReactor *pReactor,unsigned int nSubjectNo)
:CProtocol(pReactor, XTPHLEN, new CXTPPackage(), new CXTPPackage())
{
	m_pFlow=NULL;
	m_SendPackage.ConstructAllocate(PACKAGE_MAX_SIZE,PACKAGE_RESERVE_SIZE);
	m_nSubjectNo = nSubjectNo;
}

CTcpXtpProtocol::~CTcpXtpProtocol()
{
}

CXTPPackage *CTcpXtpProtocol::GetNextDataPackage()
{
	if(m_flowReader.GetNext(&m_SendPackage))
	{
		TXTPHeader  *pHeader = m_SendPackage.GetXTPHeader();
		pHeader->SequenceNo = m_flowReader.GetId();
	//	pHeader->SubjectNo = m_nSubjectNo;
		return &m_SendPackage;
	}
	return NULL;
}

int CTcpXtpProtocol::PublishSend()
{
	int nCount = 0;
	for(; nCount<40; nCount++)
	{
		CXTPPackage *pPackage = GetNextDataPackage();
		if (pPackage == NULL)
			break;

		if (Send(pPackage) != 0)
			break;		
	}
	return nCount;
}

void  CTcpXtpProtocol::PublishFlow(CReadFlow *pFlow, UF_INT8 nStartId)
{
	m_pFlow=pFlow;
	m_flowReader.AttachFlow(m_pFlow,nStartId);
}

void CTcpXtpProtocol::DetachFlow()
{
	m_flowReader.DetachFlow();
}
