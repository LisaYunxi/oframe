#include "public.h"
#include "XMPPackage.h"

CXMPPackage::CXMPPackage()
{
	m_XMPHeader.Init();
	m_ExtHeader.Init();
}

CXMPPackage::~CXMPPackage()
{

}

unsigned int CXMPPackage::GetActiveID()
{
	return (unsigned int)(m_XMPHeader.Type);
}

int CXMPPackage::ValidPackage()
{
	int pklen=Length();
	if(pklen<XMPHLEN)
	{
		//包头尚未收全
		return -1;
	}

	m_XMPHeader.FromStream(m_pHead);
	int bodylen=m_XMPHeader.Length;
	if(bodylen<0 || bodylen>XMP_PACKAGE_MAX_SIZE)
	{
		return -2;
	}

	int extlen=m_XMPHeader.ExtensionLen;
	if(extlen<0 || (extlen>0 && extlen<XMPEXTHLEN) || extlen>XMP_EXTHEAD_MAX_SIZE)
	{	
		return -3;
	}
	
	if (extlen > 0)
	{
		memcpy(&m_ExtHeader, m_pHead+XMPHLEN, extlen);
	}
	else
	{
		memset(&m_ExtHeader, 0, sizeof(m_ExtHeader));
	}
	
	if(pklen>=(XMPHLEN+extlen+bodylen))
	{
		m_pTail=m_pHead+XMPHLEN+bodylen+extlen;

		Pop(XMPHLEN + extlen);
		
		return XMPHLEN+bodylen+extlen;
	}
	
	return -1;
}

int CXMPPackage::MakePackage()
{
//	DEBUG_LOG0("CXMPPackage::MakePackage Start\n");
	char *buf;
	m_XMPHeader.Length = Length();
	if(m_ExtHeader.Tag != XMPTagNone)
	{
		m_XMPHeader.ExtensionLen = XMPEXTHLEN + m_ExtHeader.TagLen;
		buf = Push(m_XMPHeader.ExtensionLen);
		memcpy(buf, &m_ExtHeader, m_XMPHeader.ExtensionLen);
	}
	else
	{
		m_XMPHeader.ExtensionLen = 0;
	}

	buf = Push(XMPHLEN);
	if(buf == NULL)
	{
		return -1;
	}

	m_XMPHeader.ToStream(buf);

//	DEBUG_LOG0("CXMPPackage::MakePackage End\n");
	return 0;
}

void CXMPPackage::SetExtHeader(unsigned char Tag, unsigned char TagLen, unsigned char *pData)
{
	m_ExtHeader.Tag = Tag;
	m_ExtHeader.TagLen = TagLen;
	memcpy(m_ExtHeader.Data, pData, TagLen);
}


