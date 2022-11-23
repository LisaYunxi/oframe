#ifndef __XtpClientStub_H__
#define __XtpClientStub_H__

#include "public.h"
#include "SessionFactory.h"
#include "SelectReactor.h"
#include "TcpXtpSession.h"
#include "Flow.h"
#include "PipeEventHandler.h"
#include "XtpClient.h"

#define USER_CHANNEL_DISCONNECTED_ALL 0x1199

class CXtpClientStub:public CSessionFactory,public CTcpXtpSessionCallback,public CXtpClient
{
public:
	CXtpClientStub(CReactor *pReactor, CFlow *pSerailFlow, int nSubjectNo, UF_INT8 nSequenceNo);
	~CXtpClientStub();
	virtual void RegisterReportServer(const char *pszServerAddress);
	virtual void Init();
	virtual void StartConnect();
	virtual void StopConnect();
	virtual int HandlePackage(CXTPPackage *pXTPPackage, CTcpXtpSession *pSession);
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);
	virtual void RegClientCallback(CXtpClientCallback *pClientCallback);

	void InterLogin(CTcpXtpSession *pSession);

	virtual int SendReportPackage(CXTPPackage *pXTPPackage);
	virtual int RequestToDialogFlow(CXTPPackage *pXTPPackage);

protected:
	virtual CSession *CreateSession(CChannel *pChannel, unsigned int dwMark);
	virtual void OnSessionConnected(CSession *pSession);
	virtual void OnSessionDisconnected(CSession *pSession, int nReason);
	void CreateDialogFlow();
	void RemoveDialogFlow();

	CXTPPackage m_XtpPackage;
	char *m_pzLogPath;
	bool m_bConnected;
	CXtpClientCallback *m_pClientCallBack;

public:
	
	CFlow *m_pSerialFlow;
	//��¼�յ��˶��ٸ���,���ڿ���m_pSerialFlow���ж��ͬʱд����ʱ��Ҫ�Լ���¼���ܵı���
	//��ֻ����һ�����ļ���ʱ��m_nSubCount������
	//����ϲ��ʼ��m_nSubCount==-1��ʱ��,ʹ������С��ʼ������Ϊֻ��һ�����ļ�
	UF_INT8 m_nSubCount;
	//���ĵ������
	int m_nSubjectNo;
	UF_INT8 m_nSequenceNo;
	CFlow *m_pDialogReqFlow;
	CPipeEventHandler *m_pPipeEventHandler;
};
#endif

