// XTPPubSession.cpp: implementation of the CXTPPubSession class.
//
// 20080620 xuzh CProtocol不再自动删除上层协议，m_pControlProtocol要
//                 自行删除
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "XTPPubSession.h"
#include "SCPPackageDesc.h"


CXTPPubSession::CXTPPubSession(CReactor *pReactor, CChannel *pChannel,
							   CXTPFlowManager *pFlowManager)
:CXTPSession(pReactor, pChannel)
{
	m_pFlowManager = pFlowManager;
 	m_pControlProtocol = new CSCPProtocol(pReactor);
	m_pControlProtocol->AttachLower(m_pXMPProtocol, XMPTypeSCP);
	m_controlPackage.ConstructAllocate(SCP_PACKAGE_MAX_SIZE, m_pControlProtocol->GetHdrReserveLen());
	m_pControlProtocol->RegisterUpperHandler(this);
	m_pXMPProtocol->EnableHeartbeat(false);
}

CXTPPubSession::~CXTPPubSession()
{
	delete m_pControlProtocol;
}

int CXTPPubSession::HandlePackage(CPackage *pPackage, CProtocol *pProtocol)
{
	if (m_pControlProtocol == pProtocol)
	{
		CSCPPackage *pSCPPackage = (CSCPPackage *)pPackage;
//		DEBUG_LOG1("STID = %04x\n", pSCPPackage->GetTid());
		switch(pSCPPackage->GetTid()) 
		{
		case STID_SUBSCRIBE:
			OnSubscribeReq(pSCPPackage);
			break;
		case STID_UN_SUBSCRIBE:
			OnUnSubscribeReq(pSCPPackage);
			break;
		case STID_PUBLISH:
			OnHeartBeat(pSCPPackage);
			break;
		case STID_ACK:
			OnAck(pSCPPackage);
			break;
		default:
			break;
		}

		return 0;
	}
	
	return CXTPSession::HandlePackage(pPackage, pProtocol);
}

void CXTPPubSession::OnSubscribeReq(CSCPPackage *pPackage)
{
	CSCPSubscribeField subscribeField;
	CNamedFieldIterator i = pPackage->GetNamedFieldIterator(&subscribeField.m_Describe);

	while (!i.IsEnd()) 
	{
		i.Retrieve(&subscribeField);

		if (m_pFlowManager == NULL)
		{
			break;
		}
		CReadFlow *pFlow = m_pFlowManager->GetFlow(subscribeField.SubjectNo);
		if (pFlow != NULL) 
		{
//			NET_STATUS_LOG3("publish : reqCommPase[%d] SubscriberID[%x] startid[%s]\n", 
//				(const unsigned short)subscribeField.SubscriberID,
//				PRINT64(subscribeField.StartID));
			Publish(pFlow, subscribeField.SubjectNo,subscribeField.SubscriberID, subscribeField.StartID);		
		}
		i.Next();
	}
}

void CXTPPubSession::OnUnSubscribeReq(CSCPPackage *pPackage)
{
	CSCPSubscribeField subscribeField;
	CNamedFieldIterator i = pPackage->GetNamedFieldIterator(&subscribeField.m_Describe);

	while (!i.IsEnd()) 
	{
		i.Retrieve(&subscribeField);

		UnPublish(subscribeField.SubjectNo);

		i.Next();
	}	
}

void CXTPPubSession::OnHeartBeat(CSCPPackage *pPackage)
{
	CSCPPublishField publishField;
	CNamedFieldIterator i = pPackage->GetNamedFieldIterator(&publishField.m_Describe);

	while (!i.IsEnd()) 
	{
		i.Retrieve(&publishField);

		CXTPSession::OnHeartBeat(publishField.SubjectNo, publishField.SeqNo);

		i.Next();
	}
}

void CXTPPubSession::OnAck(CSCPPackage *pPackage)
{
	CSCPSubscribeField subscribeField;
	CNamedFieldIterator i = pPackage->GetNamedFieldIterator(&subscribeField.m_Describe);

	while (!i.IsEnd()) 
	{
		i.Retrieve(&subscribeField);

		CXTPSession::OnAck(subscribeField.SubscriberID, subscribeField.SubjectNo, subscribeField.StartID);

		i.Next();
	}
}

void CXTPPubSession::RegisterSubscriber(CSubscriber *pSubscriber, unsigned int dwSubscriberID)
{
	CXTPSession::RegisterSubscriber(pSubscriber, dwSubscriberID);
	SendSubscribeReq(pSubscriber->GetSubjectNo(), dwSubscriberID, pSubscriber->GetReceivedCount());
}

void CXTPPubSession::UnRegisterSubscriber( CSubscriber *pSubscriber)
{
	CXTPSession::UnRegisterSubscriber(pSubscriber);
	SendUnSubscribeReq(pSubscriber->GetSubjectNo());
}


CSCPPackage *CXTPPubSession::PrepareControlPackage(unsigned short stid)
{
	m_controlPackage.AllocateToAdd();
	m_controlPackage.Truncate(0);
	m_controlPackage.SetTid(stid);
	return &m_controlPackage;
}

void CXTPPubSession::SendControlPackage(CSCPPackage *pPackage)
{
	m_pControlProtocol->Send(pPackage);
}

void CXTPPubSession::SendSubscribeReq(unsigned int nSubjectNo, unsigned int dwSubscriberID, UF_INT8 nStartID)
{
//	DEBUG_LOG3("SendSubscribeReq SubjectNo:[%x], StartID:[%0.f]\n", nSubjectNo, nStartID);

	CSCPSubscribeField subscribeField;
	subscribeField.SubjectNo = nSubjectNo;
	subscribeField.StartID = nStartID;
	subscribeField.SubscriberID = dwSubscriberID;

	CSCPPackage *pPackage = PrepareControlPackage(STID_SUBSCRIBE);
	SCP_ADD_FIELD(pPackage, &subscribeField);

	SendControlPackage(pPackage);
}

void CXTPPubSession::SendUnSubscribeReq(unsigned int nSubjectNo)
{
	CSCPSubscribeField subscribeField;
	subscribeField.SubjectNo = nSubjectNo;
	subscribeField.StartID = 0.0;

	CSCPPackage *pPackage = PrepareControlPackage(STID_UN_SUBSCRIBE);
	SCP_ADD_FIELD(pPackage, &subscribeField);

	SendControlPackage(pPackage);

}

void CXTPPubSession::SendNak(unsigned int nSubjectNo, unsigned int dwSubscriberID, UF_INT8 nSeqNo)
{
	SendSubscribeReq(nSubjectNo, dwSubscriberID, nSeqNo);
}

void CXTPPubSession::SendAck(unsigned int nSubjectNo, unsigned int dwSubscriberID, UF_INT8 nSeqNo)
{
	CSCPSubscribeField subscribeField;
	subscribeField.SubjectNo = nSubjectNo;
	subscribeField.StartID = nSeqNo;
	subscribeField.SubscriberID = dwSubscriberID;

	CSCPPackage *pPackage = PrepareControlPackage(STID_ACK);
	SCP_ADD_FIELD(pPackage, &subscribeField);

	SendControlPackage(pPackage);

}

void CXTPPubSession::SendHeartBeat(unsigned int nSubjectNo, UF_INT8 nSeqNo)
{
	CSCPPublishField publishField;
	publishField.SubjectNo = nSubjectNo;
	publishField.SeqNo = nSeqNo;

	CSCPPackage *pPackage = PrepareControlPackage(STID_PUBLISH);
	SCP_ADD_FIELD(pPackage, &publishField);

	SendControlPackage(pPackage);
}
