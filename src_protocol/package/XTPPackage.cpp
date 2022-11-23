#include "XTPPackage.h"

CXTPPackage::CXTPPackage()
{
	m_nHeadLength = XTPHLEN;
}

CXTPPackage::~CXTPPackage()
{
}

//unsigned int CXTPPackage::GetActiveID()
//{
//	if (m_XTPHeader.Type == XTP_Type_PUBLISH)
//	{
//		return m_XTPHeader.SubjectNo;
//	}
//	return 0;
//}

int CXTPPackage::ValidPackage()
{
	if (Length() < XTPHLEN)
		return -1;			//包头长度不对

	m_XTPHeader.FromStream(m_pHead);

	/*	char *pData=m_pHead+XTPHLEN;
		if(m_XTPHeader.Length!=Length()-XTPHLEN)
		{
			DEBUG_LOG3("TNo=%08x,Length()-XTPHLEN=%d m_XTPHeader.Length=%d\n",
				m_XTPHeader.TNo, Length(), m_XTPHeader.Length);
			return -2;			//包体长度与包头不符
		}*/


	Pop(XTPHLEN);
	Truncate(m_XTPHeader.Length);
	return m_XTPHeader.Length + XTPHLEN;
}

int CXTPPackage::MakePackage()
{
	m_XTPHeader.Length = Length();
	char *pBuffer = Push(XTPHLEN);

	if (pBuffer == NULL)
		return -1;

	m_XTPHeader.ToStream(pBuffer);
	//	DEBUG_LOG0("CXTPPackage::MakePackage End\n");
	return 0;
}

//int CXTPPackage::MakeNoBufPackage()
//{
//	m_XTPHeader.Length = Length();
//	 m_pHead-=XTPHLEN;
//	char *pBuffer = m_pHead;
//	if (pBuffer == NULL) 
//	{
//		return -1;
//	}
//	m_XTPHeader.ToStream(pBuffer);
//	return 0;
//}

void CXTPPackage::PrepareRequest(unsigned int tid, unsigned char chain)
{
	AllocateToAdd();
	Truncate(0);
	SetSize(0);
	memset(&m_XTPHeader, 0, sizeof(TXTPHeader));
	m_XTPHeader.MessageNo = tid;
	m_XTPHeader.Chain = chain;
	m_XTPHeader.Type = XTP_Type_REQUEST;
}

void CXTPPackage::PrepareResponse(CXTPPackage *pRequest, unsigned int tid, unsigned char chain)
{
	AllocateToAdd();
	Truncate(0);
	SetSize(0);
	memset(&m_XTPHeader, 0, sizeof(TXTPHeader));
	TXTPHeader *pReqHeader = pRequest->GetXTPHeader();
	m_XTPHeader.Type = XTP_Type_RESPONSE;
	m_XTPHeader.Chain = chain;
	m_XTPHeader.SubjectNo = pReqHeader->SubjectNo;
	m_XTPHeader.MessageNo = tid;
	m_XTPHeader.RequestNo = pReqHeader->RequestNo;
	m_XTPHeader.SessionNo = pReqHeader->SessionNo;
	m_XTPHeader.FrontNo = pReqHeader->FrontNo;
	m_XTPHeader.BusinessNo = pReqHeader->BusinessNo;
	m_XTPHeader.SequenceNo = 0;
	m_XTPHeader.GlobalNo = 0;
	m_XTPHeader.Length = 0;
	m_nSize = 0;
}

void CXTPPackage::PrepareResponse(unsigned int SessionNo, unsigned int FrontNo, unsigned int RequestNo, unsigned int tid, unsigned char chain)
{
	AllocateToAdd();
	Truncate(0);
	SetSize(0);
	memset(&m_XTPHeader, 0, sizeof(TXTPHeader));
	m_XTPHeader.Type = XTP_Type_RESPONSE;
	m_XTPHeader.Chain = chain;
	m_XTPHeader.Length = 0;
	m_XTPHeader.SubjectNo = 0;
	m_XTPHeader.MessageNo = tid;
	m_XTPHeader.SequenceNo = 0;
	m_XTPHeader.GlobalNo = 0;
	m_XTPHeader.BusinessNo = 0;
	m_XTPHeader.RequestNo = RequestNo;
	m_XTPHeader.SessionNo = SessionNo;
	m_XTPHeader.FrontNo = FrontNo;
}

void CXTPPackage::PreparePublish(unsigned int tid, unsigned char chain)
{
	AllocateToAdd();
	Truncate(0);
	SetSize(0);
	memset(&m_XTPHeader, 0, sizeof(TXTPHeader));
	m_XTPHeader.MessageNo = tid;
	m_XTPHeader.Chain = chain;
	m_XTPHeader.Type = XTP_Type_PUBLISH;
	m_nSize = 0;
}

CXTPPackage *CXTPPackage::CreatePackage(int nHeaderReserve)
{
	CXTPPackage *pPackage = new CXTPPackage;
	pPackage->ConstructAllocate(PACKAGE_MAX_SIZE, nHeaderReserve);
	return pPackage;
}

char* CXTPPackage::OutputHeader(char* header)
{
	TXTPHeader *pHeader = GetXTPHeader();
	sprintf(header, "XTP PACKAGE HEADER\n");
	sprintf(header + strlen(header), "\tType[%c],Chain[%c],Leng[%d],MsgNo[0X%X]", pHeader->Type, pHeader->Chain, pHeader->Length & 0xFFFF, pHeader->MessageNo);
//	sprintf(header + strlen(header), ",ServiceNo[%d],SubjectNo[%d],SequenceNo[%.0f],GlobalNo[%.0f],BusinessNo[%.0f]\n", pHeader->ServiceNo, pHeader->SubjectNo, pHeader->SequenceNo, pHeader->GlobalNo, pHeader->BusinessNo);
	sprintf(header + strlen(header), ",SubNo[%d],SeqNo[%s],GlobalNo[%s],BusiNo[%s]\n", pHeader->SubjectNo, LargeNumberToString(pHeader->SequenceNo).c_str(), LargeNumberToString(pHeader->GlobalNo).c_str(), LargeNumberToString(pHeader->BusinessNo).c_str());
	sprintf(header + strlen(header), "\tFrontNo[%d],SessionNo[%d],ReqNo[%d]", pHeader->FrontNo, pHeader->SessionNo, pHeader->RequestNo);
	sprintf(header + strlen(header), ",Remark[%s],Index[%s],UserID[%s],Reserve[%s]\n", pHeader->Remark, pHeader->SubIndexName, pHeader->UserID, pHeader->Reserve);
	return header;
}
