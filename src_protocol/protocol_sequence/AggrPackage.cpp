#include "public.h"
#include "AggrPackage.h"

CAggrPackage::CAggrPackage()
{
	m_nHeadLength = sizeof(TAggrHeader);
}

CAggrPackage::~CAggrPackage()
{

}

unsigned int CAggrPackage::GetActiveID()
{
	return m_header.AggregationID;
}

int CAggrPackage::ValidPackage()
{
	int pklen = Length();
	if(Length() < AGGRHLEN)
	{
		return -1;			//包头长度不对
	}

	m_header.FromStream(m_pHead);

	if(m_header.Length != Length()-AGGRHLEN)
	{
		return -2;			//包体长度与包头不符
	}

	Pop(AGGRHLEN);

	return m_header.Length+AGGRHLEN;
}

int CAggrPackage::MakePackage()
{
	m_header.Length = Length();

	char *buf = Push(AGGRHLEN);
	if(buf == NULL)
	{
		EMERGENCY_EXIT("package buffer too small");
	}

	m_header.ToStream(buf);

	return 0;
}

