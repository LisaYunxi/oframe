// ArbPackage.cpp: implementation of the CGuardPackage class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "ArbPackage.h"



CGuardPackage::CGuardPackage()
{
	m_nHeadLength = sizeof(TArbHeader);
}

CGuardPackage::~CGuardPackage()
{

}

unsigned int CGuardPackage::GetActiveID()
{
	return 0;
}

int CGuardPackage::ValidPackage()
{
	int pklen = Length();
	if(Length() < ARBHLEN)
	{
		return -1;			//��ͷ���Ȳ���
	}

	m_header.FromStream(m_pHead);

	if(m_header.Length != Length()-ARBHLEN)
	{
		return -2;			//���峤�����ͷ����
	}

	Pop(ARBHLEN);

	return m_header.Length+ARBHLEN;
}

int CGuardPackage::MakePackage()
{
	m_header.Length = Length();

	char *buf = Push(ARBHLEN);
	if(buf == NULL)
	{
		EMERGENCY_EXIT("package buffer too small");
	}

	m_header.ToStream(buf);

	return 0;
}

