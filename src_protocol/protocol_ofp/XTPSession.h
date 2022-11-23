#ifndef AFX_XTPSESSION_H__
#define AFX_XTPSESSION_H__

#include "EventHandler.h"
#include "XMPSession.h"
#include "XMPProtocol.h"
#include "XTPProtocol.h"
#include "FlowReader.h"

class CXTPSession;

class CXTPReqCallback
{
public:
	virtual int HandleRequest(CXTPPackage *pXTPPackage, CXTPSession *pSession) = 0;
	virtual ~CXTPReqCallback() {};
};

class CXTPRspCallback
{
public:
	virtual int HandleResponse(CXTPPackage *pXTPPackage, CXTPSession *pSession) = 0;
	virtual ~CXTPRspCallback() {};
};

/**��һ�����Э��ջ���������ǻ���IO�����EventHandler�࣬һ������ListenCtrl
*������������ע�ᵽһ��Reactor�������ĵײ�������һ��TcpChannel�ϣ������ڵ�
*Э��ֻ��ͨ�������Ĳ��ܵõ���д���� */
class CXTPSession : public CProtocolCallback, public CXMPSession, public CXTPPubHelper, public CXTPSubHelper
{
public:
	/**���캯�������Э��ջ�Ĺ���
	* @param pReactor   ��������
	* @param pChannel   һ��TcpChannel
	*/
	CXTPSession(CReactor *pReactor, CChannel *pChannel);

	/**�����������ͷſɿ���Դ
	*/
	virtual ~CXTPSession();

	/**����һ����Ϣ
	* @param nEventID ��ϢID
	* @param dwParam ��һ������
	* @param pParam �ڶ�������
	* @return ������
	*/
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);

	virtual int HandlePackage(CPackage *pPackage, CProtocol *pProtocol);

	void RegisterReqHandler(CXTPReqCallback *pReqHandler);
	void RegisterRspHandler(CXTPRspCallback *pRspHandler);

	void Publish(CReadFlow *pFlow, unsigned int nSubjectNo, unsigned int dwSubscriberID, UF_INT8 nStartId);
	void UnPublish(unsigned int nSubjectNo);
	void RegisterSubscriber(CSubscriber *pSubscriber, unsigned int dwSubscriberID);
	void UnRegisterSubscriber(CSubscriber *pSubscriber);
	CXTPPackage *CreatePackage();
	void SendRequestPackage(CXTPPackage *pPackage);
	void OnHeartBeat(unsigned int nSubjectNo, UF_INT8 nSeqNo);
	void OnAck(unsigned int dwSubscriberID, unsigned int dwSubjectNo, UF_INT8 nStartId);

protected:
	CXTPProtocol *m_pXTPProtocol;	/**< �����ڵĵ�����Э��(���²���ʼ)*/
	CXTPReqCallback *m_pReqHandler;
	CXTPRspCallback *m_pRspHandler;
};

#endif
