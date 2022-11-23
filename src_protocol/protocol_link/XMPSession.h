#ifndef AFX_XMPSESSION_H__
#define AFX_XMPSESSION_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Session.h"
#include "XMPProtocol.h"

class CXMPSession : public CSession  
{
public:
	CXMPSession(CReactor *pReactor,CChannel *pChannel);
	virtual ~CXMPSession();
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);
	void SetHeartbeatTimeout(unsigned int dwReadTimeout);
	void SetHeartbeatMode(bool bSlaveMode);
protected:
	CXMPProtocol *m_pXMPProtocol;	// 容器内的第二层协议(从下部开始)
};

#endif
