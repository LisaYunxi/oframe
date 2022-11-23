#ifndef AFX_SESSION_H__
#define AFX_SESSION_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "EventHandler.h"
#include "ChannelProtocol.h"

class CSession;

class CSessionCallback
{
public:
	virtual void OnSessionConnected(CSession *pSession) = 0;
	virtual void OnSessionDisconnected(CSession *pSession, int nReason) = 0;
	virtual void OnSessionWarning(CSession *pSession, int nReason, int nParam) = 0;
	virtual ~CSessionCallback(){};
};


/**��һ�����Э��ջ�����������������²�Э����CSession��
���಻��ʵ������һ����Ϊ����TCP���ӵ�Э��ջ�Ļ��� */
class  CSession : public CEventHandler  
{
public:
	CSession(CReactor *pReactor,CChannel *pChannel,int MaxPackageSize);
	virtual ~CSession();
	
	inline unsigned int GetSessionID();
//	inline long GetSessionID();
	//Ϊ��ֹ��ͬ���߳���SessionID��ͬ
	//@param nID �ı�ID�ĸ�λ
	void ReSetSessionID(int nThreadID);

	//�������пɶ����ݰ�ʱ������
	//@return 0   �ɹ�
	//@return <>0 ʧ��
	virtual int HandleInput();
	
	//��Reactor������Ҫ�����緢�����ݰ�ʱ������
	//@return >=0   �ɹ�
	//@return <0    ʧ��
	virtual int HandleOutput();
	
	//����һ����Ϣ
	//@param nEventID ��ϢID
	//@param dwParam ��һ������
	//@param pParam �ڶ�������
	//@return ������
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);

	//ȡ���²�Э��󶨵�Channel
	//@return ���²�Э��󶨵�Channel
	inline CChannel *GetChannel();

	//�Ͽ���������
	//@param ErrorCode  ��·��ԭ��
	virtual void Disconnect(int nErrorCode);

	//��ȡ����дsocket����selectʹ��
	//@param pReadId   ������������ض���ʹ�õ�socket��Ϊ0��ʾ����Ҫ��
	//@param pWriteId  �������������д��ʹ�õ�socket��Ϊ0��ʾ����Ҫд 
	virtual void GetIds(int *pReadId, int *pWriteId);

	inline void RegisterSessionCallback(CSessionCallback *pSessionCallback);

	//����/�ָ�������
	//param bSuspend true ���������    false�ָ������� 
	void SuspendRead(bool bSuspend);

	//���Channel�Ƿ���Ҫд��������
	//@return true ��Ҫд��������, false û��Ҫд��������
	bool IsChannelWriteBusy();

protected:	
	//�����·��Ϣ�������ÿ�����������·ʱ���ô˺���
	//@param ErrorCode  ��·��ԭ��
	virtual void OnChannelLost(int nErrorCode);
protected:
	unsigned int m_nSessionID;

	CChannel *m_pChannel;			//���������Channel
	CChannelProtocol *m_pChannelProtocol;	//�����ڵĵײ�Э��
	CSessionCallback *m_pSessionCallback;

	bool m_bSuspendRead;
};

inline CChannel *CSession::GetChannel()
{
	return m_pChannel;
}

inline void CSession::RegisterSessionCallback(CSessionCallback *pSessionCallback)
{
	m_pSessionCallback = pSessionCallback;
}

inline unsigned int CSession::GetSessionID()
//inline long CSession::GetSessionID()
{
	return m_nSessionID;
}

#endif
