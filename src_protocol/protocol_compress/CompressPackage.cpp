#include "public.h"
#include "CompressPackage.h"

CCompressPackage::CCompressPackage()
{
	m_nHeadLength = CRPHLEN;
}

CCompressPackage::~CCompressPackage()
{
}

unsigned int CCompressPackage::GetActiveID()
{
	return (unsigned int)m_header.Type;
}

int CCompressPackage::ValidPackage()
{
	int pkglen = Length();
	if(pkglen < CRPHLEN){			//��ͷ��δ��ȫ
		return -1;
	}

	m_header.FromStream(m_pHead);

	Pop(CRPHLEN);

	return pkglen;
}

int CCompressPackage::MakePackage()
{
	char *buf = Push(CRPHLEN);
	if(buf == NULL)
	{
		return -1;
	}

	m_header.ToStream(buf);
	
	return 0;
}
