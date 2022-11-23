#include "public.h"
#include "Package.h"

CPackageBuffer::CPackageBuffer(int length)
{
	m_pData = new char[length];

	memset(m_pData, 0, length);
	m_bReleaseData = true;
	m_nLength = length;
	m_nRefCount = 1;
}

CPackageBuffer::CPackageBuffer(char *pData, int length)
{
	m_pData = pData;
	m_bReleaseData = false;
	m_nLength = length;
	m_nRefCount = 1;
}

CPackageBuffer::~CPackageBuffer()
{
	if (m_bReleaseData)
		delete[] m_pData;
}

void CPackageBuffer::AddRef()
{
	m_nRefCount++;
}

void CPackageBuffer::Release()
{
	if (--m_nRefCount <= 0)
		delete this;
}

////////////////////////////////////////////////////////////////////////////

CPackage::CPackage()
{
	m_pPackageBuffer = NULL;
	m_nReserve = PACKAGE_RESERVE_SIZE;
	Clear();
	m_nRefCount = 1;
	m_nHeadLength = 0;
}

CPackage::~CPackage()
{
	BufRelease();
}

unsigned int CPackage::GetActiveID()
{
	return 0;
}

int CPackage::ValidPackage()
{
	return Length();
}

int CPackage::MakePackage()
{
	return 0;
}

void CPackage::Clear()
{
	if (m_pPackageBuffer == NULL)
	{
		m_pHead = m_pTail = NULL;
	}
	else
	{
		m_pHead = m_pPackageBuffer->Data() + m_pPackageBuffer->Length();
		m_pTail = m_pHead;
	}
}

char *CPackage::Push(int size)
{
	if (size <= 0)
		return NULL;
	//if((m_pHead-size)>=m_pPackageBuffer->Data()){
	//	m_pHead-=size;
	//	return m_pHead;
	//}
	//return NULL;

	m_pHead -= size;
	return m_pHead;
}

char *CPackage::Pop(int size)
{
	if (Length() < size)
		return NULL;
	char *tmp = m_pHead;
	m_pHead += size;
	return tmp;
}

void CPackage::BufAddRef(CPackage *pPackage)
{
	BufRelease();
	m_pPackageBuffer = pPackage->m_pPackageBuffer;
	if (m_pPackageBuffer != NULL)
	{
		m_pPackageBuffer->AddRef();
		m_pHead = pPackage->m_pHead;
		m_pTail = pPackage->m_pTail;
	}
}

CPackageBuffer *CPackage::BufRelease()
{
	CPackageBuffer *old = m_pPackageBuffer;
	if (m_pPackageBuffer != NULL)
	{
		m_pPackageBuffer->Release();
		m_pPackageBuffer = NULL;
		m_pHead = m_pTail = NULL;
	}
	return old;
}

void CPackage::ConstructAllocate(int nCapacity, int nReserve)
{
	delete m_pPackageBuffer;
	m_pPackageBuffer = new CPackageBuffer(nCapacity + nReserve);
	m_nReserve = nReserve;
	m_nCapacity = nCapacity;
	Clear();
}

char * CPackage::AllocateToAdd()
{
	if (m_pPackageBuffer == NULL)
		return NULL;
	m_pHead = m_pPackageBuffer->Data() + m_nReserve;
	m_pTail = m_pPackageBuffer->Data() + m_pPackageBuffer->Length();
	return m_pHead;
}

char * CPackage::AllocateToRead()
{
	if (m_pPackageBuffer == NULL)
		return NULL;
	m_pHead = m_pPackageBuffer->Data() + m_nReserve - m_nHeadLength;
	m_pTail = m_pPackageBuffer->Data() + m_pPackageBuffer->Length();
	return m_pHead;
}

char * CPackage::Allocate(int length)
{
	if (m_pPackageBuffer == NULL)
		return NULL;

	if (length > m_pPackageBuffer->Length())
		return NULL;

	m_pTail = m_pPackageBuffer->Data() + m_pPackageBuffer->Length();
	m_pHead = m_pTail - length;
	return m_pHead;
}

void CPackage::Truncate(int newLength)
{
	//xuzh 如果出现报文比Length大的情况，如果不扩展一下，导致后续拷贝全部出错
	//	if (Length() > newLength)
	//	{
	m_pTail = m_pHead + newLength;
	//	}
}

void CPackage::Release()
{
	m_nRefCount--;
	if (m_nRefCount <= 0)
		delete this;
}

void CPackage::AddRef()
{
	m_nRefCount++;
}

void CPackage::DupPackage(CPackage *pSrcPackage)
{
	Allocate(pSrcPackage->Length());
	memcpy(Address(), pSrcPackage->Address(), pSrcPackage->Length());
	Truncate(pSrcPackage->Length());
}

void CPackage::AttachBuffer(CPackageBuffer *pPackageBuffer)
{
	BufRelease();
	m_pPackageBuffer = pPackageBuffer;
	m_pPackageBuffer->AddRef();
	m_pHead = pPackageBuffer->Data();
	m_pTail = m_pHead + pPackageBuffer->Length();
}

void CPackage::SetData(char *pData, int nDataLen)
{
	BufRelease();
	m_pHead = pData;
	m_pTail = pData + nDataLen;
}

CPackageBuffer* CPackage::GetPackageBuffer()
{
	return m_pPackageBuffer;
}

char* CPackage::OutputHeader(char* header)
{
	return header;
}