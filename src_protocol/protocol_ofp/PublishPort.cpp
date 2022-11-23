// PublishPort.cpp: implementation of the CPublishPort class.

//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "PublishPort.h"

const unsigned int SUBSCRIBER_ACK_TIME_OUT = 1000;
/*
void prob(int no,int maxp)
{
	static struct timeval timeout[100];
	static int inter[100];
	static int begin[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//printf("step 1 no=%d ,maxp=%d\n",no , maxp);fflush(stdout);	
	//采集控制
	if (begin[no] !=0 )
		return;
	
	if (no==0 )
	{
		begin[no]=1;
	}
	else
	{
		if (begin[no-1] ==0)
			return;
	}
	
	begin[no]=1;	
		
	struct timeval now;
	gettimeofday(&now, 0);
	timeout[no].tv_sec=now.tv_sec;
	timeout[no].tv_usec=now.tv_usec;

//printf("step 2 no=%d ,maxp=%d\n",no , maxp);fflush(stdout);

	if (no == maxp-1)
	{
		//计算差值
		int total=0;
		for (int k=0;k<maxp;k++)
		{
			if ((k+1)<maxp)
			{
				inter[k]=(timeout[k+1].tv_sec-timeout[k].tv_sec)*1000*1000 + timeout[k+1].tv_usec - timeout[k].tv_usec;
				total=total+inter[k];
	//printf("total=%d inter[%d]=%u timeout[%d].tv_sec=%u timeout[%d].tv_usec=%u\n",total,k,inter[k],k,timeout[k].tv_sec,k,timeout[k].tv_usec);
			}
		}	
				
		if (total > 500)
		{	
				//输出
				char buf[800];
				char bb[100];
				strcpy(buf,"");
				for (int i=0;i<maxp-1;i++)
				{
					sprintf(bb,"[%-d,%-d][%u]\t",i,i+1, inter[i]);
					strcat(buf,bb);
				}
			
				char TimeStr[100];
        time_t ltime;
        struct tm *now;
        time(&ltime);
        now=localtime(&ltime);
        sprintf(TimeStr,"%02d:%02d:%02d",now->tm_hour,now->tm_min,now->tm_sec);
        printf("\n\n%s  %d %s\n",TimeStr,timeout[0].tv_usec/1000,buf);
        fflush(stdout);
		}
		
		//清空
		memset(&begin[0],0,maxp*sizeof(int));
	}	
	
}

*/
//////////////////////////////////////////////////////////////////////
// CPublishCompactPackage
//////////////////////////////////////////////////////////////////////
CPublishCompactPackage::CPublishCompactPackage(int nMaxLength)
{
	ConstructAllocate(nMaxLength, 1000);
	m_pBufferEnd = m_pPackageBuffer->Data()+m_pPackageBuffer->Length();
	Reset();
}

bool CPublishCompactPackage::AppendPackage(CPackage *pPackage)
{
	if (m_pTail+pPackage->Length() > m_pBufferEnd)
	{
		return false;
	}
	memcpy(m_pTail, pPackage->Address(), pPackage->Length());
	m_pTail += pPackage->Length();
	return true;
}

void CPublishCompactPackage::Reset()
{
	AllocateToAdd();
	Truncate(0);
}

CPublishPort::CPublishPort(CProtocol *pSendProtocol, CXTPPackage *pSendPackage, unsigned int nSubjectNo)
{
	m_pSendProtocol = pSendProtocol;
	m_pSendPackage = pSendPackage;
	m_nSubjectNo = nSubjectNo;
	m_nTransRateIncrease = 10;
		
	m_nTransRate = 20000;
	m_nDesiredTransRate = m_nTransRate;
	m_nBandWidth = 0;
	srand(time(NULL));
//	m_dwGroupID = rand();
	
	//xuzh 20200922 扩充了XTP_PACKAGE_MAX_SIZE，这里不能写死
	m_pCompactPackage = new CPublishCompactPackage(2*(PACKAGE_MAX_SIZE+PACKAGE_RESERVE_SIZE));

	m_nRandCostBandWidth=0;
	m_nCurrTimerClockLapse=0;
}

CPublishPort::~CPublishPort()
{
	delete m_pCompactPackage;
}

int CPublishPort::AttachFlow(CReadFlow *pFlow, UF_INT8 nStartId)
{
	return m_flowReader.AttachFlow(pFlow, nStartId);
}

static int inter=0;
static int jk=0;
void CPublishPort::MoveTo(UF_INT8 nOffset, CFlowReader::SeekFlag nSeekFlag)
{
	if (nSeekFlag==CFlowReader::FROM_HEAD && m_flowReader.GetId()==nOffset 
		|| nSeekFlag==CFlowReader::FROM_HERE && nOffset==0)
	{
		//如果原地未动，则直接返回
		return;
	}
	
	m_nDesiredTransRate = m_nTransRate;	
	
//	NET_STATUS_LOG3("Publish Port Move SubjectNo=[%d] [%s] Move [%s]\n",m_nSubjectNo, PRINT64(m_flowReader.GetId()), PRINT64(nOffset-m_flowReader.GetId()));
	if (m_nTransRate > 100 && (nSeekFlag ==CFlowReader::FROM_HEAD && m_flowReader.GetId()>nOffset
			|| nSeekFlag!=CFlowReader::FROM_HERE && nOffset<0))
	{
		m_nTransRate *= 95;
		m_nTransRate /= 100;
	}
	
	//将重传时的速度和降速的平均值做为理想速度
	m_nDesiredTransRate = (m_nDesiredTransRate+m_nTransRate)/2;
	m_flowReader.SetId(nOffset, nSeekFlag);
	
	//NET_STATUS_LOG1("[%d]\n", m_flowReader.GetId());
	printf("m_nDesiredTransRate=%d 	m_nTransRate=%d  inter_clock=%d m_nRandTotal=%d jk=%d\n\n\n",m_nDesiredTransRate,m_nTransRate,inter,m_nRandCostBandWidth,jk);
}

CPackage *CPublishPort::GetNextDataPackage()
{
	if(m_flowReader.GetNext(m_pSendPackage))
	{
		TXTPHeader  *pHeader = m_pSendPackage->GetXTPHeader();
		pHeader->SequenceNo = m_flowReader.GetId();
		pHeader->SubjectNo = m_nSubjectNo;
//		pHeader->GroupID = (unsigned short)m_dwGroupID;		//临时使用
		return m_pSendPackage;
	}
	return NULL;
}

bool CPublishPort::Flush()
{
	int nSendRet = m_pSendProtocol->Send(m_pCompactPackage);
	if (nSendRet != 0)
	{
		MoveTo(-1, CFlowReader::FROM_HERE);
	}
	m_pCompactPackage->Reset();
	return (nSendRet == 0);
}

int CPublishPort::Publish(unsigned int nClockLapse,int DriveFlag)
{
	if (DriveFlag == 0)
		inter=nClockLapse;
	
	if (nClockLapse > 20)
	{
		nClockLapse = 20;
	}
	
	//else if (nClockLapse <=0)
	//{
	//	nClockLapse = 1;
	//}

	//定时驱动,去除随机驱动消耗的带宽
	if (DriveFlag==0)
	{
		//未用完的带宽额度放弃	
		//if (m_nBandWidth > 0)
		//{
		//	m_nBandWidth = 0;
		//}
		
		//m_nBandWidth += m_nTransRate * nClockLapse;
		m_nBandWidth = m_nTransRate * nClockLapse;

		//调整带宽限额
		//m_nBandWidth-=m_nRandCostBandWidth;
		//m_nRandCostBandWidth=0;
		m_nCurrTimerClockLapse=nClockLapse;
		jk=0;
	}
	else
	{
		//如果本转发周期随机转发量大于单位时间转发最大值,则随机转发停止
		//如果上个周期发送的超额，则本次随机发送终止
		//if ((m_nRandCostBandWidth > (m_nTransRate * m_nCurrTimerClockLapse)) || m_nBandWidth<0)
		if (m_nBandWidth<0)
		{
			//printf("本周期带宽已用完 m_nRandCostBandWidth=%d m_nPlanWidth=%d  TimerClockLapse=%d\n",m_nRandCostBandWidth,m_nTransRate * m_nCurrTimerClockLapse,m_nCurrTimerClockLapse);
			return 0;
		}
		m_nRandBandWidth =  m_nBandWidth;	
		//分配本次发送额度
		/*
		if ( nClockLapse==0)
			m_nRandBandWidth = 1;	//只发送一个包
		else
			m_nRandBandWidth = m_nTransRate * nClockLapse;     //nClockLapse个毫秒带宽
			*/
		jk++;
	}		
		
	int nSendCount = 0;
	int nThisLimit=0;
	bool bSendSuccess = true;
	bool bRandFullPackFlag = false;
	
	//本次发送额度
	if (DriveFlag==0)
	{
		nThisLimit=m_nBandWidth;
		m_nBandWidth=0;
	}
	else
	{
		nThisLimit=m_nRandBandWidth;
		//预冻结本次额度
		m_nBandWidth-=nThisLimit;
	}

	while (nThisLimit > 0)
	{
		CPackage *pPackage = GetNextDataPackage();
		if (pPackage == NULL)
		{
			//没有发送任务
			break;
		}
		
		pPackage->MakePackage();
		
		//计算本次消耗带宽
		nThisLimit-=pPackage->Length();

		while (!m_pCompactPackage->AppendPackage(pPackage))
		{
			//如果满包则发送
			bSendSuccess = Flush();
			
			//事件触发组成一个完整报文
			if (DriveFlag==1)
			{
				bRandFullPackFlag=true;
			}	
		}
		if (!bSendSuccess)
		{
			break;
		}

		nSendCount++;		
		//add by zbz 事件驱动导致消耗的带宽
		//if (DriveFlag==1)
		//{
		//	m_nRandCostBandWidth+=pPackage->Length();
		//}
	}

	//清算本次额度
	m_nBandWidth+=nThisLimit;
		
	if (bSendSuccess && m_pCompactPackage->Length()>0)
	{
		Flush();
	}
		
	//只有定时驱动，才可以加速
	if (m_nBandWidth <= 0 && DriveFlag==0)
	{
		if (m_nTransRate < m_nDesiredTransRate)
		{
			m_nTransRate += m_nTransRateIncrease;
		}
		else
		{
			//超过理想速度时，增速减半
			m_nTransRate += m_nTransRateIncrease/2;
		}
	}	
	return nSendCount;
}

void CPublishPort::Merge(CPublishPort *pPublishPort)
{
	for(int i=0; i<pPublishPort->m_SubscirberInfos.size(); i++)
	{
		m_SubscirberInfos.push_back(pPublishPort->m_SubscirberInfos[i]);
	}
}

bool CPublishPort::RemoveTimeoutSubscriber(unsigned int dwCurrTime)
{
	vector<TSubscirberInfo>::iterator i= m_SubscirberInfos.begin();
	for (; i != m_SubscirberInfos.end(); i++)
	{
		if ((dwCurrTime-(*i).dwLastActiveTime) > SUBSCRIBER_ACK_TIME_OUT)
		{
//			NET_STATUS_LOG3("RemoveTimeoutSubscriber: SubscriberID=[%x] CurrentTime=[%d] LastActiveTime=[%d]\n", (*i).dwSubscriberID, dwCurrTime, (*i).dwLastActiveTime);
			m_SubscirberInfos.erase(i);
			return true;
		}
	}
	return false;
}


