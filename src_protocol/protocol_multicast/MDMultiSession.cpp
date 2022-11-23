// MDMultiSession.cpp: implementation of the CMDMultiSession class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "MDMultiSession.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//多播心跳超时时间
#define MULTI_HEART_TIMEOUT   10000
#define FTD_TID_MULTI_HEARTBEAT 0x00FF0000


CMDMultiSession::CMDMultiSession(CReactor *pReactor,CChannel *pChannel,DWORD nTopicID,bool bIsServer)
:CSession(pReactor, pChannel, 4096+256)
{
	m_pFTDCProtocol = new CFTDCProtocol(m_pReactor);
	m_pFTDCProtocol->AttachLower(m_pChannelProtocol, 0);
	m_pFTDCProtocol->RegisterErrorHandler(this);
	m_pFTDCProtocol->RegisterUpperHandler(this);
	m_pPackageHandler = NULL;
	m_nTopicID=nTopicID;
	m_bContinue=true;

	//add by zbz 20150304
	if (bIsServer)
	{	
		SetTimer(1, MULTI_HEART_TIMEOUT);
	}
	m_bIsServer=bIsServer;
	m_SendPackage.ConstructAllocate(FTDC_PACKAGE_MAX_SIZE+FTDCHLEN, 1000);
	strcpy(m_MultiCastIP,pChannel->GetRemoteName());
}

CMDMultiSession::~CMDMultiSession()
{
}

void CMDMultiSession::RegisterPackageHandler(CMDMultiSessionCallback *pPackageHandler)
{
	m_pPackageHandler = pPackageHandler;
}


void CMDMultiSession::OnTimer(int nIDEvent)
{
	if (m_bIsServer)
	{
		//发送心跳
		m_SendPackage.PreparePackage(FTD_TID_MULTI_HEARTBEAT, FTDC_CHAIN_LAST, 0);
		m_pFTDCProtocol->Send(&m_SendPackage);		
	}
	
}


int CMDMultiSession::HandlePackage(CPackage *pPackage,CProtocol *pProtocol)
{

	CFTDCPackage *pFTDCPackage = (CFTDCPackage *)pPackage;

	if (!m_bIsServer)
	{
		if (pFTDCPackage->GetTID() == FTD_TID_MULTI_HEARTBEAT)
		{
			time_t dwCurrTime = (time_t)m_pReactor->Time();
			strftime(m_timeBuffer,9,"%H:%M:%S",localtime(&dwCurrTime));	
			m_pPackageHandler->OnMultiHeartbeat(m_timeBuffer,m_MultiCastIP);
		}		
	}

	if (!m_bContinue)
		return 0;
	
	if (pProtocol == m_pFTDCProtocol)
	{
		if (m_pPackageHandler == NULL)
		{
			return 0;
		}
		return m_pPackageHandler->HandlePackage(pFTDCPackage, this,m_nTopicID);
	}
	return 0;
}

void CMDMultiSession::Send(CFTDCPackage *pPackage)
{
	m_pFTDCProtocol->Send(pPackage);
}

void CMDMultiSession::Stop()
{
	m_bContinue=false;
}
void CMDMultiSession::Continue()
{
	m_bContinue=true;
}

DWORD CMDMultiSession::GetTopicID()
{
	return m_nTopicID;
}
