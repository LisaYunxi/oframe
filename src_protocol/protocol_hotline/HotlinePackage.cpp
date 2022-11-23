#include "public.h"
#include "HotlinePackage.h"

CHotlinePackage::CHotlinePackage()
{
	m_nHeadLength = sizeof(THotlineHeader);
}

CHotlinePackage::~CHotlinePackage()
{
}

unsigned int CHotlinePackage::GetActiveID()
{
	return 0;
}

int CHotlinePackage::ValidPackage()
{
	int pklen = Length();
	if(Length() < HOTLINEHLEN)
	{
		return -1;			//��ͷ���Ȳ���
	}

	m_header.FromStream(m_pHead);

	if(m_header.Length != Length()-HOTLINEHLEN)
	{
		return -2;			//���峤�����ͷ����
	}

	Pop(HOTLINEHLEN);

	return m_header.Length+HOTLINEHLEN;
}

int CHotlinePackage::MakePackage()
{
	m_header.Length = Length();

	char *buf = Push(HOTLINEHLEN);
	if(buf == NULL)
	{
		EMERGENCY_EXIT("package buffer too small");
	}

	m_header.ToStream(buf);

	return 0;
}

