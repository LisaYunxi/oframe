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

	//hotline packages�ǻ�ȡ��input��ˮ�������������Լ��ı���ͷ�����Զ�ԭ���ı��Ĳ���valid
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
		//ֱ�ӷ��ͣ�����Ҫvalidpackage
		Send(&m_readerPackage);
	}
	return nSendCount;
}
