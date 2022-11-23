#ifndef AFX_XMPPROTOCOL_H__
#define AFX_XMPPROTOCOL_H__

#include "Protocol.h"
#include "XMPPackage.h"

#define MSG_XMPERR_RECVHEARTBEAT		0x2001
#define MSG_XMPERR_SENDHEARTBEAT		0x2002
#define MSG_XMPERR_BADPACKAGE			0x2003

#define MSG_XMPWARNING_RECVHEARTBEAT	0x2101

#define XMPTypeNone				0x00
#define XMPTypeBinary			0x01
#define XMPTypeCompressed		0x02
#define XMPTypeSCP				0x03
#define XMPTypeXTP				0x04

const unsigned int XMPHBT_SLAVE_MODE	= 0x00000001;

/**协议栈容器的第二层协议，主要负责链路管理
*/
class  CXMPProtocol : public CProtocol  
{
public:
	/**构造函数
	* @param pReactor 指向运行引擎的指针
	* @param pBelow 下层协议的指针
	*/
	CXMPProtocol(CReactor *pReactor);
	/**析构函数
	*/
	virtual ~CXMPProtocol();

	/**加入XMP报头并传至下层协议
	* @param pPackage 上层层协议下传的包
	* @return 0   成功
	* @return <>0 失败
	*/
	virtual int Push(CPackage *pPackage, CProtocol *pUpper);
	
	void EnableHeartbeat(bool bEnable);

	void SetHeartbeatTimeout(unsigned int dwReadTimeout);

	void SetHeartbeatMode(unsigned int dwHeartbeatMode);
protected:	
	/**记录当前收到包的时间，判断是否XMP包，并上传
	* @param pPackage 下层协议上传的包
	* @return 0   成功
	* @return <>0 失败
	*/
	virtual int Pop(CPackage *pPackage);
		
	/**处理定时操作，由CReactor调用。检查如果读超时，则通知上层协议。
	* 检查如果写超时，则发送心跳报文。
	* @param nIDEvent 当自身为定时器时，在定时器链表定时器标识   
	*/
	virtual void OnTimer(int nIDEvent);
		
	/**生成心跳报文，并发送心跳
	* @return >0   成功
	* @return <=0  失败 
	*/
	int SendHeartbeat();

	/**生成并发送设置心跳超时报文
	* @return >0   成功
	* @return <=0  失败 
	*/
	int SendWriteTimeout(unsigned int dwWriteTimeout);

	/**根据设置心跳超时报文，设置发送心跳超时时间
	* @param pPackage 设置心跳超时报文
	*/
	void SetWriteTimeout(CXMPPackage *pPackage);

	/**收到一个错误的数据包时调用
	* @param pPackage 错误的数据包
	* @return 0   成功
	* @return <>0 失败
	*/
	virtual int OnRecvErrorPackage(CPackage *pPackage);

protected:
	unsigned int m_LastWriteTime;		/**< 最后发包时间　*/
	unsigned int m_LastReadTime;		/**< 最后收包时间　*/
	unsigned int m_LastWarnTime;		/**< 最后发出警告时间　*/
	bool m_bHeartEnable;		/**< 是否接收/发送心跳 */
	unsigned int m_dwWriteTimeout;		/**< 发出心跳的时间间隔 */
	unsigned int m_dwReadTimeout;		/**< 接收心跳的超时时间 */
	unsigned int m_dwWarningTimeout;	/**< 接收心跳的报警时间 */
	unsigned int m_dwHeartbeatMode;	/**< 心跳时间模式*/
};

#endif