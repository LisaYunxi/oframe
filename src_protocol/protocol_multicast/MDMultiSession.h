// MDMultiSession.h: interface for the CMDMultiSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDMultiSession_H__A1BCFDA2_51EF_4C57_9D8A_2E96155CEA88__INCLUDED_)
#define AFX_MDMultiSession_H__A1BCFDA2_51EF_4C57_9D8A_2E96155CEA88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Session.h"
#include "FTDCProtocol.h"

class CMDMultiSession;

class CMDMultiSessionCallback
{
public:
	virtual int HandlePackage(CFTDCPackage *pPackage, CMDMultiSession *pSession,DWORD nTopicID) = 0;
	virtual void  OnMultiHeartbeat(char *CurrTime,char *MultiCastIP) = 0;
};


class CMDMultiSession : public CSession, public CProtocolCallback  
{
public:
	CMDMultiSession(CReactor *pReactor,CChannel *pChannel,DWORD nTopicID,bool bIsServer);
	virtual ~CMDMultiSession();
	virtual int HandlePackage(CPackage *pPackage,CProtocol *pProtocol);
	void RegisterPackageHandler(CMDMultiSessionCallback *pPackageHandler);
	void Send(CFTDCPackage *pPackage);
	void Stop();
	void Continue();
	DWORD GetTopicID();
	virtual void OnTimer(int nIDEvent);
	
protected:
	CFTDCProtocol *m_pFTDCProtocol;
	CMDMultiSessionCallback *m_pPackageHandler;
	DWORD m_nTopicID;
	bool  m_bContinue;

	//新增发送心跳add by zbz 20150304
	bool  m_bIsServer;
	CFTDCPackage m_SendPackage;
	char m_timeBuffer[20];
	char m_MultiCastIP[100];
};

#endif
