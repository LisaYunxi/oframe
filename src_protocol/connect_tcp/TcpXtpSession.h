#ifndef _CTcpXtpSession_H_
#define _CTcpXtpSession_H_

#include "EventHandler.h"
#include "XMPSession.h"
#include "XMPProtocol.h"
#include "TcpXtpProtocol.h"
#include "FlowReader.h"
#include "CompressProtocol.h"

class CTcpXtpSession;
class CTcpXtpSessionCallback
{
public:
	virtual int HandlePackage(CXTPPackage *pXTPPackage, CTcpXtpSession *pSession) = 0;
	virtual ~CTcpXtpSessionCallback() {};
};

class CTcpXtpSession : public CXMPSession, public CProtocolCallback
{
public:
	CTcpXtpSession(CReactor *pReactor, CChannel *pChannel, CTcpXtpSessionCallback *pSeesionCallback, unsigned int nSubjectNo = 0);
	virtual ~CTcpXtpSession();
	virtual int HandlePackage(CPackage *pPackage, CProtocol *pProtocol);
	void SetCompressMethod(unsigned char chCompressMethod);
	int SendReportPackage(CXTPPackage *pPackage);
	void GetIds(int *pReadId, int *pWriteId);
	void PublishFlow(CReadFlow *pFlow, UF_INT8 nStartId);
private:
	CTcpXtpSessionCallback	 *m_pCallback;
	CTcpXtpProtocol			 *m_pProtocol;
	CCompressProtocol		 *m_pCRPProtocol;
	CXTPPackage				 m_prePackage;
	unsigned int			 m_nSubjectNo;
};
#endif
