#include "public.h"
#include "FTDCPackage.h"

CFTDCPackage::CFTDCPackage()
{
	m_FTDCHeader.Version = FTD_VERSION;
	m_FTDCHeader.Chain = FTDC_CHAIN_LAST;
	m_nHeadLength = FTDCHLEN;
}

CFTDCPackage::~CFTDCPackage()
{
}
//
//unsigned int CFTDCPackage::GetActiveID()
//{
//	return m_FTDCHeader.SubjectNo;
//}

int CFTDCPackage::ValidPackage()
{
	if (Length() < FTDCHLEN)
		return -1;			//包头长度不对

	m_FTDCHeader.FromStream(m_pHead);

	char *pData = m_pHead + FTDCHLEN;
	if (m_FTDCHeader.Length != Length() - FTDCHLEN)
	{
//		NET_EXCEPTION_LOG3("FTDPackage Length Inconsistent: MessageNo=%08x,Length()-FTDCHLEN=%d m_FTDCHeader.Length=%d\n",
	//		m_FTDCHeader.MessageNo, Length(), m_FTDCHeader.Length);
		return -2;			//包体长度与包头不符
	}

	Pop(FTDCHLEN);

	return m_FTDCHeader.Length + FTDCHLEN;
}

int CFTDCPackage::MakePackage()
{
	m_FTDCHeader.Length = Length();
//	m_FTDCHeader.FieldCount = 0;

	//CFieldTypeIterator itor = GetFieldTypeIterator();
	//while (!itor.IsEnd())
	//{
	//	m_FTDCHeader.FieldCount++;
	//	itor.Next();
	//}

	char *pBuffer = Push(FTDCHLEN);
	if (pBuffer == NULL)
		return -1;

	m_FTDCHeader.ToStream(pBuffer);
	//	DEBUG_LOG0("CFTDCPackage::MakePackage End\n");
	return 0;
}

void CFTDCPackage::PreparePackage(unsigned int tid, unsigned char chain)
{
	AllocateToAdd();
	Truncate(0);
	memset(&m_FTDCHeader, 0, sizeof(TFTDCHeader));
	m_FTDCHeader.Chain = chain;
	m_FTDCHeader.MessageNo = tid;
	m_FTDCHeader.Version = FTD_VERSION;
}

void CFTDCPackage::PrepareResponse(CFTDCPackage *pRequest, unsigned int tid, unsigned char chain)
{
	PreparePackage(tid, chain);
	//SetRequestId(pRequest->GetRequestId());
	m_FTDCHeader.RequestNo = pRequest->GetFTDCHeader()->RequestNo;
}

CFTDCPackage *CFTDCPackage::CreatePackage(int nHeaderReserve)
{
	CFTDCPackage *pPackage = new CFTDCPackage;
	pPackage->ConstructAllocate(PACKAGE_MAX_SIZE, nHeaderReserve);
	return pPackage;
}

bool CFTDCPackage::OutputHeader(char* header)
{
	TFTDCHeader *pHeader = GetFTDCHeader();
	sprintf(header, "FTDC PACKAGE HEADER\n");
	sprintf(header + strlen(header), "\tVer[%d],Chain[%c],MsgNo[0X%X],", pHeader->Version, pHeader->Chain, pHeader->MessageNo);
	sprintf(header + strlen(header), "SubNo[%d],SeqNo[%s],", pHeader->SubjectNo & 0xFFFF, LargeNumberToString(pHeader->SequenceNo).c_str());
//	sprintf(header + strlen(header), "Count[%d],Length[%d],RequestNo[%d],BusinessNo[%0.f]\n", pHeader->FieldCount, pHeader->Length, pHeader->RequestNo, pHeader->BusinessNo);
	sprintf(header + strlen(header), "Leng[%d],ReqNo[%d],Remark[%s],Index[%s],BusiNo[%s]\n", pHeader->Length, pHeader->RequestNo, pHeader->Remark, pHeader->SubIndexName, LargeNumberToString(pHeader->BusinessNo).c_str());
	return true;
}
