#include "public.h"
#include "FTDCSubEndPoint.h"
#include "FTDCPackage.h"

const unsigned int NAK_TIMEOUT = 100;				//每一次NAK在NAK_TIMEOUT毫秒后发出，以后超时加倍
const unsigned int RECV_HEARTBEAT_TIMEOUT = 2000;	//接收心跳超时时间，单位为毫秒

CFTDCSubEndPoint::CFTDCSubEndPoint(CFTDCSubscriber *pSubscriber)
{
	m_pSubscriber = pSubscriber;
}

CFTDCSubEndPoint::~CFTDCSubEndPoint()
{
}

int CFTDCSubEndPoint::HandlePackage(CFTDCPackage *pFTDCPackage)
{
	if (pFTDCPackage->GetFTDCHeader()->SequenceNo != m_pSubscriber->GetReceivedCount() + 1)
		return -1;

	m_pSubscriber->HandleMessage(pFTDCPackage);
	return 0;
}
