#include "public.h"
#include "HotlineProtocol.h"

CHotlineProtocol::CHotlineProtocol(CReactor *pReactor):CProtocol(pReactor, HOTLINEHLEN, new CHotlinePackage(), NULL)
{
	m_readerPackage.ConstructAllocate(HOTLINE_PACKAGE_MAX_SIZE, 1000);
}

CHotlineProtocol::~CHotlineProtocol()
{
}

int CHotlineProtocol::PublishSend()
{
	if (m_pFlow == NULL) 
	{
		return 0;
	}

	//hotline packages是获取了input流水，在上面增加自己的报文头，所以对原来的报文不能valid
	int nSendCount = 0;
	for (nSendCount=0; nSendCount < 40; nSendCount++)
	{
		int len = CFlowReader::GetNextBase(&m_readerPackage);
		if (len < 0) 
		{
			break;
		}
		m_readerPackage.Truncate(len);
		m_readerPackage.SetTid(HTID_CONFIRM_REQ);
		m_readerPackage.SetSequenceNo(GetId());
		//直接发送，不需要validpackage
		Send(&m_readerPackage);
	}
	return nSendCount;
}
