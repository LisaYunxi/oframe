#include "public.h"
#include "SCPPackage.h"

CSCPPackage::CSCPPackage()
{
	m_nHeadLength = sizeof(TSCPHeader);
}

CSCPPackage::~CSCPPackage()
{

}

unsigned int CSCPPackage::GetActiveID()
{
	return 0;
}

int CSCPPackage::ValidPackage()
{
	if (Length() < SCPHLEN)
		return -1;			//包头长度不对

	m_header.FromStream(m_pHead);

	if (m_header.Length != Length() - SCPHLEN)
	{
		return -2;			//包体长度与包头不符
	}

	Pop(SCPHLEN);

	return m_header.Length + SCPHLEN;
}

int CSCPPackage::MakePackage()
{
	m_header.Length = Length();

	char *buf = Push(SCPHLEN);
	if (buf == NULL)
	{
		EMERGENCY_EXIT("package buffer too small");
	}

	m_header.ToStream(buf);

	return 0;
}


