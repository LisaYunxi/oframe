#include "public.h"
#include "FieldSet.h"

//////////////////////////////////////////////////////////////////////
CFieldIteratorBase::CFieldIteratorBase()
{

};

CFieldIteratorBase::CFieldIteratorBase(char *pHead, char *pEnd, CFieldDescribe *pFieldDescribe)
{
	m_pData = NULL;
	m_pCurr = pHead;
	m_pEnd = pEnd;
	m_pFieldDescribe = pFieldDescribe;
	Next();
};

void CFieldIteratorBase::Next()
{
	m_pData = NULL;
	while (m_pData == NULL)
	{
		if (DataRemain() < sizeof(TFieldHeader))
			break;

		GetHeader();
		m_pCurr += sizeof(TFieldHeader);

		if (DataRemain() < m_fieldHeader.Size)
			break;

		if (m_pFieldDescribe == NULL || m_fieldHeader.FieldID == m_pFieldDescribe->m_FieldID)
			m_pData = m_pCurr;

		m_pCurr += m_fieldHeader.Size;
	}
}

CNamedFieldIterator::CNamedFieldIterator(char *pHead, char *pEnd, CFieldDescribe *pFieldDescribe) :CFieldIteratorBase(pHead, pEnd, pFieldDescribe)
{
}

int CNamedFieldIterator::Retrieve(void *pStruct)
{
	if (m_pData == NULL)
		return 0;

	m_pFieldDescribe->StreamToStruct((char *)pStruct, m_pData);
	return m_fieldHeader.FieldID;
}

CFieldTypeIterator::CFieldTypeIterator()
{
}

CFieldTypeIterator::CFieldTypeIterator(char *pHead, char *pEnd) :CFieldIteratorBase(pHead, pEnd, NULL)
{
}

int CFieldTypeIterator::Retrieve(CFieldDescribe *pFieldDescribe, void *pStruct)
{
	if (m_pData == NULL)
		return 0;

	pFieldDescribe->StreamToStruct((char *)pStruct, m_pData);
	return pFieldDescribe->GetStreamSize();
}

TFieldHeader CFieldTypeIterator::Retrieve(void *pStruct)
{
	if (m_pData != NULL)
		memcpy(pStruct, m_pData, m_fieldHeader.Size);

	return m_fieldHeader;
}

CFieldSet::CFieldSet()
{
	m_nSize = 0;
}

CFieldSet::~CFieldSet()
{
}

int CFieldSet::GetSingleField(CFieldDescribe *pFieldDescribe, void *pField)
{
	CNamedFieldIterator itor = GetNamedFieldIterator(pFieldDescribe);
	if (itor.IsEnd())
		return 0;

	return itor.Retrieve(pField);
}

//只分配了空间
char * CFieldSet::AllocField(unsigned short wFieldID, unsigned short wSize)
{
	//新域加入后，包长不能超过缓冲区的长度
	if (m_pTail + wSize + sizeof(TFieldHeader) >= m_pPackageBuffer->Data() + m_pPackageBuffer->Length())
		return NULL;

	TFieldHeader fh;
	fh.FieldID = wFieldID;
	fh.Size = wSize;
	CHANGE_ENDIAN(fh.FieldID);
	CHANGE_ENDIAN(fh.Size);

	memcpy(m_pTail, &fh, sizeof(fh));

	char *pBuf = m_pTail + sizeof(TFieldHeader);
	m_pTail += sizeof(TFieldHeader) + wSize;
	return pBuf;
}

int CFieldSet::AddField(CFieldDescribe *pFieldDescribe, void *pField)
{
	char *buf = AllocField(pFieldDescribe->m_FieldID, pFieldDescribe->m_nStreamSize);

	if (buf == NULL)
		return -1;
	pFieldDescribe->StructToStream(buf, (char *)pField);
	m_nSize++;
	return 0;
}

void CFieldSet::SetSize(int nSize)
{
	m_nSize = nSize;
}

int CFieldSet::GetSize()
{
	return m_nSize;
}

bool CFieldSet::AddStream(unsigned short nFieldID, const char *pField, int nLength)
{
	if (pField == NULL)
		return true;

	char *buf = NULL;
	buf = AllocField(nFieldID, nLength);

	if (buf == NULL)
		return false;

	memcpy(buf, pField, nLength);
	m_nSize++;
	return true;
}