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

/**是一个存放协议栈的容器，是基于IO处理的EventHandler类，一般是由ListenCtrl
*创建，它必须注册到一个Reactor。容器的底部捆绑在一个TcpChannel上，容器内的
*协议只有通过容器的才能得到读写操作 */
class CXTPSession : public CProtocolCallback, public CXMPSession, public CXTPPubHelper, public CXTPSubHelper
{
public:
	/**构造函数，完成协议栈的构造
	* @param pReactor   运行引擎
	* @param pChannel   一个TcpChannel
	*/
	CXTPSession(CReactor *pReactor, CChannel *pChannel);

	/**析构函数，释放可控资源
	*/
	virtual ~CXTPSession();

	/**处理一条消息
	* @param nEventID 消息ID
	* @param dwParam 第一个参数
	* @param pParam 第二个参数
	* @return 处理结果
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
	CXTPProtocol *m_pXTPProtocol;	/**< 容器内的第三层协议(从下部开始)*/
	CXTPReqCallback *m_pReqHandler;
	CXTPRspCallback *m_pRspHandler;
};

#endif
