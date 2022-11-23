// FIBSession.h: interface for the CFIBSession class .
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBSESSION_H__88022E6A_66E9_46E0_B66F_8F73B23CDEFA__INCLUDED_)
#define AFX_FIBSESSION_H__88022E6A_66E9_46E0_B66F_8F73B23CDEFA__INCLUDED_

#include "EventHandler.h"
#include "XMPSession.h"
#include "XMPProtocol.h"
#include "CompressProtocol.h"
#include "FIBProtocol.h"
#include "FlowReader.h"

class CFIBSession;

class CFIBSessionCallback
{
public:
	virtual int HandlePackage(CXTPPackage *pFIBPackage, CFIBSession *pSession) = 0;
};

/**是一个存放协议栈的容器，是基于IO处理的EventHandler类，一般是由ListenCtrl
*创建，它必须注册到一个Reactor。容器的底部捆绑在一个TcpChannel上，容器内的
*协议只有通过容器的才能得到读写操作 */
class CFIBSession : public CProtocolCallback, public CXMPSession
{
public:
	/**构造函数，完成协议栈的构造
	* @param pReactor   运行引擎
	* @param pChannel   一个TcpChannel
	*/ 	
	CFIBSession(CReactor *pReactor,CChannel *pChannel);
	
	/**析构函数，释放可控资源
	*/
	virtual ~CFIBSession();
	
	/**处理一条消息
	* @param nEventID 消息ID
	* @param dwParam 第一个参数
	* @param pParam 第二个参数
	* @return 处理结果
	*/
	virtual int HandleEvent(int nEventID, DWORD dwParam, void *pParam);

	virtual int HandlePackage(CPackage *pPackage,CProtocol *pProtocol);

	/**获取读、写socket，供select使用
	* @param pReadId   输出参数，返回读所使用的socket，为0表示不需要读
	* @param pWriteId  输出参数，返回写所使用的socket，为0表示不需要写
	* @remark 在这里将发布的主题的数据包传递到最下层协议
	*/
	virtual void GetIds(int *pReadId, int *pWriteId);
	
	void RegisterPackageHandler(CFIBSessionCallback *pPackageHandler);

	void Publish(CReadOnlyFlow *pFlow, WORD wSequenceSeries, int nStartId);
	void UnPublish(WORD wSequenceSeries);
	void RegisterSubscriber(CFIBSubscriber *pSubscriber);
	void UnRegisterSubscriber(CFIBSubscriber *pSubscriber);
	
	CXTPPackage *CreatePackage();
	
	void SendRequestPackage(CXTPPackage *pPackage);
	
	void SetCommPhaseNo(DWORD wCommPhaseNo);	

	virtual void Disconnect(int nErrorCode);

	/**设置针对FIB的压缩算法
	*@param chCompressMethod 压缩算法代码，0表示不压缩
	*/
	void SetCompressMethod(BYTE chCompressMethod);
	
	/**设置本会话的版本
	*@param	version	版本号
	*/
	void SetVersion(BYTE version);
	
	/**获取本会话的版本
	*@return	本会话的版本
	*/
	BYTE GetVersion(void);
	
protected:
	CFIBProtocol *m_pFIBProtocol;		/**< FIB协议对象*/
	CCompressProtocol *m_pCRPProtocol;	/**< CRP协议对象*/
	CFIBSessionCallback *m_pPackageHandler;
	///-----
	///	xuzh，20060223：增加了对FTD协议的版本设置
	///-----
	BYTE m_version;
};

#endif // !defined(AFX_FIBSESSION_H__88022E6A_66E9_46E0_B66F_8F73B23CDEFA__INCLUDED_)
