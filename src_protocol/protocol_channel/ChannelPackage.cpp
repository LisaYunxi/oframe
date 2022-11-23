#include "public.h"
#include "ChannelPackage.h"
#include "monitorIndex.h"

CChannelPackage::CChannelPackage(int nPackageMaxSize)
{
	ConstructAllocate(nPackageMaxSize, 0);
}

CChannelPackage::~CChannelPackage()
{
}

unsigned int CChannelPackage::GetActiveID()
{
	//ActiveID 永远是0
	return 0;
}

int CChannelPackage::ValidPackage()
{
	//总是完整的数据包
	return Length();
}

int CChannelPackage::ReadFromChannel(CChannel *pChannel)
{
	int nReadLen = 0;
	if (pChannel->GetType() == CT_STREAM)
	{
		//捕捉未查出的系统的缺陷
		if (m_pPackageBuffer == NULL)
		{
			int nThID = 0;
#ifdef WIN32
			nThID = (int)::GetCurrentThreadId();
#else
			nThID = (int)::pthread_self();
#endif		
			REPORT_EVENT(LOG_CRITICAL, "mouse", "CChannelPackage,CurrThreadID=[%d]", nThID);
			return 0;
		}

		int n = m_pHead - m_pPackageBuffer->Data();
		memmove(m_pPackageBuffer->Data(), m_pHead, Length());
		m_pHead -= n;
		m_pTail -= n;

		int nCount = m_pPackageBuffer->Length() - Length();
		nReadLen = pChannel->Read(nCount, m_pTail);
		if (nReadLen > 0)
		{
			m_pTail += nReadLen;
		}
	}
	else
	{
		m_pTail = m_pHead = m_pPackageBuffer->Data();

		nReadLen = pChannel->Read(m_pPackageBuffer->Length(), m_pTail);
		if (nReadLen > 0)
		{
			m_pTail += nReadLen;
		}
	}
	return nReadLen;
}
