#include "public.h"
#include "XTPSubEndPoint.h"
#include "XTPPackage.h"

const unsigned int NAK_TIMEOUT	= 40;				//����. ��һ��NAK���ʱ��,�Ժ�ʱ�ӱ�
const unsigned int RECV_HEARTBEAT_TIMEOUT	= 1500;	//����. ����������ʱʱ��
//const unsigned int ACK_TIMEOUT = 200;				//����. ACK���ʱ��
const unsigned int ACK_TIMEOUT = 200;				//����. ACK���ʱ��

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
	//�ж�m_pSubscriber�Ƿ�Ϊ�գ���ΪResetSubscriber�ӿڻ��ÿո�ָ��
	if (m_available ==0)
	{
		return 0;
	}
	//����һ��������ע���ڶ��SubEndPoint�����������ͬһ��ŵı��ģ�
	//���δ��������ߣ������Ҫ�Ա��Ľ��й��ˡ�
	//����������ĵ����С�ڵ��ڶ��������յ��ı������������ñ���
	if (pXTPPackage->GetXTPHeader()->SequenceNo <= m_pSubscriber->GetReceivedCount())
		return 0;

	//�Զ����ߵĲ�������ͬ��
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
		//�����
		CPackageBuffer packageBuffer((char *)pPackageData, nPackageLen);
		pXTPPackage->AttachBuffer(&packageBuffer);
		pXTPPackage->ValidPackage();

		//���ϴ���
		Delivery(pXTPPackage);

		//������
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
	//��ʱ��δ�յ���������������Ϣ��׼������NAK
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
	//���ڷ���ACK
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
		//�Ѿ���ȫ�����е���Ϣ
		m_nDeliveryClock = nCurrClock;
		m_nDisorderClock = 0;
	}
}


