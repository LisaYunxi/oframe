#ifndef _FTDCSESSION_H__
#define _FTDCSESSION_H__

#include "EventHandler.h"
#include "XMPSession.h"
#include "XMPProtocol.h"
#include "CompressProtocol.h"
#include "FTDCProtocol.h"
#include "FlowReader.h"

class CFTDCSession;

class CFTDCSessionCallback
{
public:
	virtual int HandlePackage(CFTDCPackage *pFTDCPackage, CFTDCSession *pSession) = 0;
};

//是一个存放协议栈的容器，是基于IO处理的EventHandler类，一般是由ListenCtrl
//创建，它必须注册到一个Reactor。容器的底部捆绑在一个TcpChannel上，容器内的
//协议只有通过容器的才能得到读写操作 
class CFTDCSession : public CProtocolCallback, public CXMPSession
{
public:
	CFTDCSession(CReactor *pReactor, CChannel *pChannel);
	virtual ~CFTDCSession();
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);
	virtual int HandlePackage(CPackage *pPackage, CProtocol *pProtocol);

	//获取读、写socket，供select使用
	// @param pReadId   输出参数，返回读所使用的socket，为0表示不需要读
	// @param pWriteId  输出参数，返回写所使用的socket，为0表示不需要写
	// @remark 在这里将发布的主题的数据包传递到最下层协议
	virtual void GetIds(int *pReadId, int *pWriteId);

	void RegisterPackageHandler(CFTDCSessionCallback *pPackageHandler);

	void Publish(CReadFlow *pFlow, unsigned short wSequenceSeries, UF_INT8 nStartId, UF_INT8 nBusinessNo = UF_INT8_MAX, int nReaderTimes = 1);
	void UnPublish(unsigned short wSequenceSeries);
	void UnPublishAll();

	void RegisterSubscriber(CFTDCSubscriber *pSubscriber);
	void UnRegisterSubscriber(CFTDCSubscriber *pSubscriber);

	CFTDCPackage *CreatePackage();

	void SendRequestPackage(CFTDCPackage *pPackage);

	virtual void Disconnect(int nErrorCode);

	//设置针对FTDC的压缩算法
	//@param chCompressMethod 压缩算法代码，0表示不压缩
	void SetCompressMethod(unsigned char chCompressMethod);

	void SetVersion(unsigned char version);
	unsigned char GetVersion(void);

protected:
	CFTDCProtocol *m_pFTDCProtocol;
	CCompressProtocol *m_pCRPProtocol;
	CFTDCSessionCallback *m_pPackageHandler;
	unsigned char m_version;
};

#endif
