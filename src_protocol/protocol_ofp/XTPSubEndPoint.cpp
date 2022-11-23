#include "public.h"
#include "XTPSubEndPoint.h"
#include "XTPPackage.h"

const unsigned int NAK_TIMEOUT	= 40;				//毫秒. 第一次NAK间隔时间,以后超时加倍
const unsigned int RECV_HEARTBEAT_TIMEOUT	= 1500;	//毫秒. 接收心跳超时时间
//const unsigned int ACK_TIMEOUT = 200;				//毫秒. ACK间隔时间
const unsigned int ACK_TIMEOUT = 200;				//毫秒. ACK间隔时间

CXTPSubEndPoint::CXTPSubEndPoint(CSubscriber *pSubscriber, unsigned int dwSubscriberID, CXTPSubHelper *pControlHelper)
:m_orderingQ(4000)
{
	m_pSubscriber = pSubscriber;
	m_nDisorderClock = 0;
	m_nDeliveryClock = 0;
	m_nNakInterval = NAK_TIMEOUT;
	m_pControlHelper = pControlHelper;
	m_dwSubscriberID = dwSubscriberID;
	m_dwLastAckClock = 0;
//	m_dwGroupID = 0;
	m_orderingQ.SetStartIndex(m_pSubscriber->GetReceivedCount() + 1);
	m_available=1;
}

CXTPSubEndPoint::~CXTPSubEndPoint()
{
}

int CXTPSubEndPoint::Delivery(CXTPPackage *pXTPPackage)
{
	//判断m_pSubscriber是否为空，因为ResetSubscriber接口会置空该指针
	if (m_available ==0)
	{
		return 0;
	}
	//对于一个订阅者注册在多个SubEndPoint的情况，对于同一序号的报文，
	//会多次触发订阅者，这就需要对报文进行过滤。
	//即：如果报文的序号小于等于订阅者已收到的报文数，则丢弃该报文
	if (pXTPPackage->GetXTPHeader()->SequenceNo <= m_pSubscriber->GetReceivedCount())
		return 0;

	//对订阅者的操作进行同步
	m_pSubscriber->EnterCritical();
	if (pXTPPackage->GetXTPHeader()->SequenceNo == m_pSubscriber->GetReceivedCount()+1)
	{
		m_pSubscriber->HandleMessage(pXTPPackage);
	}
	//}
	m_pSubscriber->LeaveCritical();

	return 0;
}

int CXTPSubEndPoint::HandlePackage(CXTPPackage *pXTPPackage, unsigned int nCurrClock)
{
	//DEBUG_LOG2("[%d] [%d]\n",pXTPPackage->GetXTPHeader()->SequenceNo ,(unsigned int)m_orderingQ.GetStartIndex());
	if (pXTPPackage->GetXTPHeader()->SequenceNo < m_orderingQ.GetStartIndex())
	{
		return 0;
	}

	if (pXTPPackage->GetXTPHeader()->SequenceNo != m_orderingQ.GetStartIndex())
	{
		//if (m_nDisorderClock == 0 && m_dwGroupID == pXTPPackage->GetXTPHeader()->GroupID)
		if (m_nDisorderClock == 0)
		{
			m_nDisorderClock = nCurrClock;
			m_nNakInterval = NAK_TIMEOUT;
			m_pControlHelper->SendNak(GetSubjectNo(), m_dwSubscriberID,m_orderingQ.GetStartIndex()-1);
		}
		pXTPPackage->Push(XTPHLEN);
		m_orderingQ.EnQueue(pXTPPackage->GetXTPHeader()->SequenceNo,pXTPPackage->Address(), pXTPPackage->Length());
		pXTPPackage->Pop(XTPHLEN);
		return 0;
	}

//	m_dwGroupID = pXTPPackage->GetXTPHeader()->GroupID;
	Delivery(pXTPPackage);
	m_orderingQ.DeQueue();
	int nPackageLen = 0;
	void *pPackageData = NULL;
	while ((pPackageData = m_orderingQ.QueueFront(nPackageLen)) != NULL)
	{
		//构造包
		CPackageBuffer packageBuffer((char *)pPackageData, nPackageLen);
		pXTPPackage->AttachBuffer(&packageBuffer);
		pXTPPackage->ValidPackage();

		//向上传送
		Delivery(pXTPPackage);

		//出队列
		m_orderingQ.DeQueue();
		pXTPPackage->BufRelease();
	}

//	if (m_orderingQ.IsEmpty())
	{
		m_nDeliveryClock = nCurrClock;
		m_nDisorderClock = 0;
	}
	return 0;
}

void CXTPSubEndPoint::OnTimer(unsigned int nCurrClock)
{	
	//长时间未收到心跳或正常的消息，准备发出NAK
	if (nCurrClock - m_nDeliveryClock > RECV_HEARTBEAT_TIMEOUT)
	{
		if (m_nDisorderClock == 0)
		{
			m_nDisorderClock = nCurrClock;	
			m_nNakInterval = NAK_TIMEOUT;
		}
	}
	if (m_nDisorderClock != 0)
	{
		unsigned int nClock = nCurrClock - m_nDisorderClock;
		if (nClock > m_nNakInterval)
		{
//			DEBUG_LOG3("Send Nak %d %u %u\n", GetSubjectNo(),m_orderingQ.GetStartIndex(),
//				nClock);
			m_pControlHelper->SendNak(GetSubjectNo(), m_dwSubscriberID, m_orderingQ.GetStartIndex() - 1);
			m_nDisorderClock = nCurrClock;	
			if (m_nNakInterval < 16 * NAK_TIMEOUT)
			{
				m_nNakInterval *= 2;
			}
		}
	}
	//定期发出ACK
	if (nCurrClock-m_dwLastAckClock > ACK_TIMEOUT)
	{
		m_pControlHelper->SendAck(GetSubjectNo(), m_dwSubscriberID, m_orderingQ.GetStartIndex() - 1);
		m_dwLastAckClock = nCurrClock;
	}
}

void CXTPSubEndPoint::OnHeartBeat(UF_INT8 nSeqNo, unsigned int nCurrClock)
{
	if (nSeqNo == m_orderingQ.GetStartIndex()-1)
	{
		//已经收全了所有的消息
		m_nDeliveryClock = nCurrClock;
		m_nDisorderClock = 0;
	}
}


