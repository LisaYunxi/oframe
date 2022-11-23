// XTPPubEndPoint.cpp: implementation of the CXTPPubEndPoint class.
//
// 20061020  xuzh 增加发送状态日志
// 20061107  xuzh 将发送状态日志由事件改为状态
// 20090617  xuzh NT-0083: XTP Nak记入syslog
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "XTPPubEndPoint.h"
#include "monitorIndex.h"

const int MAX_PUB_PORTS			= 8;	//pubport的最大个数
const int FRONT_MERGE_DISTANCE	= 10000;//向前合并距离
const int BACK_MERGE_DISTANCE	= 10000;//向后合并距离
const unsigned int SEND_HEARTBEAT_TIMEOUT	= 500;	//发送心跳时间间隔，单位为毫秒
const unsigned int LOG_TIMEOUT	= 5000;	//写日志时间间隔，单位为毫秒

//static CStringBufferMonitorIndex XTPPubMonitor("XTPPub", 10);



CXTPPubEndPoint::CXTPPubEndPoint(CReadFlow *pFlow, unsigned int nSubjectNo, 
								 CProtocol *pSendProtocol, CXTPPubHelper *pControlHelper)
{
	m_pFlow = pFlow;
	m_nSubjectNo = nSubjectNo;
	m_pSendProtocol = pSendProtocol;
	m_pControlHelper = pControlHelper;
	m_nLastSendClock = 0;
	m_nLastRandClock = 0;
	m_nSendHeartbeatClock = 0;
	m_nLogClock = 0;
	m_SendPackage.ConstructAllocate(PACKAGE_MAX_SIZE,PACKAGE_RESERVE_SIZE);
}

CXTPPubEndPoint::~CXTPPubEndPoint()
{
	for (int i=0; i<m_PublishPorts.size(); i++)
	{
		delete m_PublishPorts[i];
	}
}

int ComparePublishPort(const void *elem1, const void * elem2 )
{
	UF_INT8 nNextId1 = (*((CPublishPort **)elem1))->GetNextId();
	UF_INT8 nNextId2 = (*((CPublishPort **)elem2))->GetNextId();
	if (nNextId1 > nNextId2)
	{
		return 1;
	}
	if (nNextId1 < nNextId2)
	{
		return -1;
	}

	return 0;
}

void CXTPPubEndPoint::Publish(unsigned int nCurrClock,int DriveFlag)
{
	//对随机触发不再记录时间
	unsigned int nClockLapse;
	if (DriveFlag == 0)
	{
		nClockLapse = nCurrClock - m_nLastSendClock;
		m_nLastSendClock = nCurrClock;
	}
	else
	{
		nClockLapse = nCurrClock - m_nLastRandClock;
		m_nLastRandClock = nCurrClock;
	}

	
	int i = 0;
	for (i=0; i<m_PublishPorts.size(); i++)
	{
		m_PublishPorts[i]->Publish(nClockLapse,DriveFlag);
	}

	if (m_PublishPorts.size() < 2)
	{
		return;
	}
	//如果两个PublishPort离的很近，则将其合并，每次最多只合并一对
	qsort(&m_PublishPorts[0], m_PublishPorts.size(), sizeof(CPublishPort *),
		ComparePublishPort);
	for (i=m_PublishPorts.size()-2; i>=0; i--)
	{
		UF_INT8 nDiff = m_PublishPorts[i+1]->GetNextId()-m_PublishPorts[i]->GetNextId();
		if (nDiff<FRONT_MERGE_DISTANCE/2 && nDiff>-BACK_MERGE_DISTANCE/2)
		{
//			NET_STATUS_LOG3("Merge Publish Port[%d], merge from [%d] to [%d],", i,
	//			m_PublishPorts[i+1]->GetNextId(), m_PublishPorts[i]->GetNextId());
			m_PublishPorts[i]->Merge(m_PublishPorts[i+1]);
			RemovePublishPort(m_PublishPorts[i+1]);
	//		NET_STATUS_LOG1("Remain Publish Port Count [%d]\n", m_PublishPorts.size());
			break;
		}
	}
}

void CXTPPubEndPoint::OnNak(unsigned int dwSubscriberID, UF_INT8 nStartID, unsigned int dwNakTime)
{
	REPORT_EVENT(LOG_INFO, "XTP", "Receive Nak: Subject=[0x%04x] SubscriberID=[%x] StartID=[%0.f]\n",m_nSubjectNo, dwSubscriberID, nStartID);
	int i = 0;
	for (i=0; i<m_PublishPorts.size(); i++)
	{
		CPublishPort *pCurrPort = m_PublishPorts[i];
		if (pCurrPort->IsServeFor(dwSubscriberID))
		{
			UF_INT8 nDistance = pCurrPort->GetNextId() - nStartID;
			if(pCurrPort->GetSubscirberCount() <= 1	|| nDistance>-BACK_MERGE_DISTANCE && nDistance<FRONT_MERGE_DISTANCE)
			{
				pCurrPort->MoveTo(nStartID);
				return ;
			}
			pCurrPort->DelSubscriber(dwSubscriberID);
			if (pCurrPort->GetSubscirberCount() <= 0)
			{
				RemovePublishPort(pCurrPort);
			}
			break;
		}
	}

	//找出离nStrartID最近的前后两个PublishPort
	CPublishPort *pFrontPort = NULL;
	CPublishPort *pBackPort = NULL;
	int nBackDistance = 0x80000000;
	int nFrontDistance = 0x7FFFFFFF;

	for (i=0; i<m_PublishPorts.size(); i++)
	{
		int nDistance = m_PublishPorts[i]->GetNextId() - nStartID;
		if (nDistance < 0)
		{
			if (nBackDistance < nDistance)
			{
				nBackDistance = nDistance;
				pBackPort = m_PublishPorts[i];
			}
		}
		else
		{
			if (nFrontDistance > nDistance)
			{
				nFrontDistance = nDistance;
				pFrontPort = m_PublishPorts[i];
			}
		}
	}

	if (pBackPort != NULL && (-nBackDistance) <= BACK_MERGE_DISTANCE)
	{
		//后面的pubport已经很近了，不需要移动指针
		pBackPort->AddSubscriber(dwSubscriberID, dwNakTime);
		return;
	}

	if (pFrontPort != NULL && nFrontDistance <= FRONT_MERGE_DISTANCE)
	{
		//前面的pubport在可用范围内，指针回退
		pFrontPort->AddSubscriber(dwSubscriberID, dwNakTime);
		pFrontPort->MoveTo(nStartID);
		return;
	}

	//如果pubport已经达到最大数量，则：
	//	如果后面有pubport，则什么也不做，等待这个pubport提供服务
	//  如果后面已经没有pubport，则将前面的pubport回退
	if(m_PublishPorts.size() >= MAX_PUB_PORTS)
	{
		if (pBackPort == NULL)
		{
			if (pFrontPort == NULL)
			{
				pFrontPort = m_PublishPorts[0];
			}
			pFrontPort->MoveTo(nStartID);
			pFrontPort->AddSubscriber(dwSubscriberID, dwNakTime);
		}
		else
		{
			pBackPort->AddSubscriber(dwSubscriberID, dwNakTime);
		}
		return;
	}

	CreatePublishPort(dwSubscriberID, nStartID, dwNakTime);
}

void CXTPPubEndPoint::OnAck(unsigned int dwSubscriberID, UF_INT8 nStartID, unsigned int dwAckTime)
{
	for (int i=0; i<m_PublishPorts.size(); i++)
	{
		CPublishPort *pCurrPort = m_PublishPorts[i];
		if (pCurrPort->IsServeFor(dwSubscriberID))
		{
			if (nStartID > pCurrPort->GetNextId())
			{
				CPublishPort *pNextPort = NULL;
				if (i != m_PublishPorts.size()-1)
				{
					pNextPort = m_PublishPorts[i+1];
				}

				if (pNextPort != NULL && nStartID-pNextPort->GetNextId() > FRONT_MERGE_DISTANCE/2)
				{
					pCurrPort->DelSubscriber(dwSubscriberID);
					pNextPort->AddSubscriber(dwSubscriberID, dwAckTime);
					pCurrPort = pNextPort;
				}
				else
				{
					if (pCurrPort->GetSubscirberCount() <= 1)
					{
						//？？？这里不应减速
//printf("\n$$$$****RECV ack nStartID=%d  CurrPortID=%d\n",nStartID,pCurrPort->GetNextId());
//fflush(stdout);
						pCurrPort->MoveTo(nStartID);
					}

				}
			}
			pCurrPort->UpdSubscriber(dwSubscriberID, dwAckTime);
			break;
		}
	}
}

void CXTPPubEndPoint::CreatePublishPort(unsigned int dwSubscriberID, UF_INT8 nStartID, unsigned int dwNakTime)
{
	//创建新的pubport
	CPublishPort *pPublishPort = new CPublishPort(m_pSendProtocol, &m_SendPackage, m_nSubjectNo);
	pPublishPort->AttachFlow(m_pFlow,nStartID);
	
	//将新创建的pubport插入到数组的i位置
	m_PublishPorts.push_back(pPublishPort);
	pPublishPort->AddSubscriber(dwSubscriberID, dwNakTime);
	
	//每增加一个pubport,原有port要按权重减速
	int nTransRateSum = 0;
	for (int i=0; i<m_PublishPorts.size(); i++)
	{
		nTransRateSum += m_PublishPorts[i]->GetTransRate();
	}

	for (int k=0; k<m_PublishPorts.size(); k++)
	{
		m_PublishPorts[k]->ChangeTransRate(-pPublishPort->GetTransRate()/nTransRateSum);
//		NET_STATUS_LOG3("Old Publish Ports: m_PublishPorts[%d] = [%p], startid=[%s]\n", k, m_PublishPorts[k], PRINT64(m_PublishPorts[k]->GetNextId()));
	}

//	NET_STATUS_LOG2("New Publish Port SubscriberID=[%x] StartID=[%0.f]\n",dwSubscriberID, nStartID);
}

void CXTPPubEndPoint::RemovePublishPort(CPublishPort *pPublishPort)
{
	m_PublishPorts.erase(find(m_PublishPorts.begin(),
		m_PublishPorts.end(), pPublishPort));

	delete pPublishPort;
}

void CXTPPubEndPoint::OnTimer(unsigned int nCurrClock,int DriveFlag)
{

	Publish(nCurrClock,DriveFlag);

	// 对于随机驱动不再出触发其他事件	
	if (DriveFlag)
		return;
	
	//定期发出心跳
	if (m_pFlow != NULL && nCurrClock-m_nSendHeartbeatClock > SEND_HEARTBEAT_TIMEOUT)
	{
		m_pControlHelper->SendHeartBeat(GetSubjectNo(), m_pFlow->GetCount());
		m_nSendHeartbeatClock = nCurrClock;
	}

	vector<CPublishPort *>::iterator i = m_PublishPorts.begin();
	for (; i!= m_PublishPorts.end(); i++)
	{
		if ((*i)->RemoveTimeoutSubscriber(nCurrClock))
		{
			if ((*i)->GetSubscirberCount() <= 0)
			{
				// 2013-07-24 longqy erase 函数删除一个对象后，迭代器无效了。先用临时指针保存它。容易出错！
				CPublishPort* pDelPublishPort = (*i);
				m_PublishPorts.erase(i);
				// 2013-02-26 longqy 此对象没有任何指针指向，删除
				// delete (*i);
				delete pDelPublishPort;
				break;
			}
		}
	}

	//定期写日志	
	if (m_pFlow != NULL && nCurrClock-m_nLogClock > LOG_TIMEOUT)
	{
		char buf[1000];
		sprintf(buf, "SubjectNo=0X%X, Flow Size=%.0f, PublishPort:", GetSubjectNo(), m_pFlow->GetCount());
		i = m_PublishPorts.begin();
		for (; i!= m_PublishPorts.end(); i++)
		{
			sprintf(buf+strlen(buf), " %.0f", (*i)->GetNextId());
		}
//		strcat(buf, "\n");

//		XTPPubMonitor.setValue(buf);

		m_nLogClock = nCurrClock;
	}
}
