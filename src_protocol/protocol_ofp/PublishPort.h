// PublishPort.h: interface for the CPublishPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBLISHPORT_H__88F9C025_0AE8_4FBE_AAFD_56BAE65FECFD__INCLUDED_)
#define AFX_PUBLISHPORT_H__88F9C025_0AE8_4FBE_AAFD_56BAE65FECFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "FlowReader.h"
#include "Protocol.h"
#include "XTPPackage.h"

/**记录一个订阅者的信息
*/
struct TSubscirberInfo
{
	unsigned int dwSubscriberID;				/**< 订阅者代码 */
	unsigned int dwLastActiveTime;				/**< 上次发送ACK时间 */
};

/**CPublishCompactPackage是可以容纳多个包的包，用于发布消息时，将多个包
*  组装成一个包，再向网络上发送
*/
class CPublishCompactPackage : public CPackage
{
public:
	/**构造函数
	*@param nMaxLength 包缓冲区大小，决定了可以容纳的包的累计长度
	*@remark 构造函数中给包分配了真正的缓冲区，不要再与其它包挂接。
	*/
	CPublishCompactPackage(int nMaxLength);

	/**向包中添加一个包的内数据
	*@param pPackage 要添加的包
	*@return true 添加成功，false 添加失败，包内的空闲空间不足
	*@remark
	*/
	bool AppendPackage(CPackage *pPackage);

	/**清空包中的数据
	*/
	void Reset();
private:
	char *m_pBufferEnd;			/**< 空闲缓冲区指针 */
};

/**CPublishPort是消息队列的发布端口。一个CPublishPort对象负责向一组订阅者
*  发送消息。
*/
class CPublishPort  
{
public:
	/**构造函数
	*@param pSendProtocol 发布消息使用的协议对象
	*@param pSendPackage 发布消息使用的包
	*@param nSubjectNo 发布消息的主题
	*/
	CPublishPort(CProtocol *pSendProtocol, CXTPPackage *pSendPackage, unsigned int nSubjectNo);

	/**析构函数
	*/
	virtual ~CPublishPort();

	/**挂接一个流，准备将流中的包发送出去
	*@param pFlow 要发布的流
	*@param nStartId 当前要发送的消息的序号
	*@return 0 挂接成功， 其它 挂接失败
	*/
	int AttachFlow(CReadFlow *pFlow, UF_INT8 nStartId);

	/**改变当前发送消息的序号
	*@param nOffset 消息序号的偏移量
	*@param nSeekFlag 起始位置：
	*       FROM_HEAD 从流头部计算；FROM_HERE 从当前位置计算；FROM_END 从尾部计算
	*@remark 如果序号发生了回退，则发送速率会下降
	*/
	void MoveTo(UF_INT8 nOffset, CFlowReader::SeekFlag nSeekFlag = CFlowReader::FROM_HEAD);

	/**获取下一个要发送的包
	*@return NULL 未取到要发送的包；!=NULL 取到了要发送的包
	*/
	CPackage *GetNextDataPackage();
	
	/**发送一组消息
	*@param nClockLapse 时间片长度
	*@return 实际发送的消息的个数
	*@remark 根据时间片的长度和发送速率，从当前序号开始，将流中的消息发送出去
	*/
	int Publish(unsigned int nClockLapse,int DriveFlag=0);

	/**接管另一个发布端口所管理的订阅者
	*@param pPublishPort 被接管的发布端口
	*/
	void Merge(CPublishPort *pPublishPort);

	/**检查一个订阅者是否由本CPublishPort对象提供发送服务
	*@param dwSubscriberID 订阅者ID
	*@return true 提供，false 不提供
	*/
	inline bool IsServeFor(unsigned int dwSubscriberID);

	/**获取当前发送消息的序号
	*@return 当前发送消息的序号
	*/
	inline UF_INT8 GetNextId();

	/**获取提供服务的订阅者的个数
	*@return 订阅者的个数
	*/
	inline int GetSubscirberCount();

	/**增加对一个订阅者的发布服务
	*@param dwSubscriberID 订阅者ID
	*@param dwLastActiveTime 提供服务的起始时间
	*/
	inline void AddSubscriber(unsigned int dwSubscriberID, unsigned int dwLastActiveTime);

	/**取消对一个订阅者的发布服务
	*@param dwSubscriberID 订阅者ID
	*@param dwLastActiveTime 提供服务的起始时间
	*/
	inline void DelSubscriber(unsigned int dwSubscriberID);

	/**更新一个订阅者的ACK时间
	*@param dwSubscriberID 订阅者ID
	*@param dwLastActiveTime 收到ACK的时间
	*@return
	*@remark
	*/
	inline void UpdSubscriber(unsigned int dwSubscriberID, unsigned int dwLastActiveTime);

	/**将超时的订阅者从服务列表中删除
	*@param dwCurrTime　当前时间
	*@return true 有订阅者超时，false 没有订阅者超时
	*@remark 当长时间未收到某个订阅者的ACK时，将该订阅者从服务列表中删除
	*/
	bool RemoveTimeoutSubscriber(unsigned int dwCurrTime);

	/**改变发送速率
	*@param nDeltaTransRate 发送速率的变化量，>0增速，<0减速
	*/
	inline void ChangeTransRate(int nDeltaTransRate);

	/**获取当前发送速率
	*@return 当前发送速率
	*/
	inline int GetTransRate();

private:
	/**将从流中读出并合并的包发送到网络上
	*@return true 发送成功，false 发送失败
	*/
	bool Flush();

private:
	unsigned int m_nSubjectNo;							/**< 发布主题ID */
	CFlowReader m_flowReader;					/**< 读取要发布的流的Reader */
	CXTPPackage *m_pSendPackage;				/**< 读取要发布的流的包 */
	
	CPublishCompactPackage *m_pCompactPackage;	/**< 合并包的包，供int m_nRandCostBandWidth;  //随机驱动消费带宽累计发送用 */

	int m_nTransRate;							/**< 发送速率(byte/ms) */
	int m_nBandWidth;							/**< 发送带宽(速率×时间片) */
	CProtocol *m_pSendProtocol;					/**< 发布消息使用的协议对象 */
	int m_nTransRateIncrease;					/**< 速率增长速度 */
	int m_nDesiredTransRate;					/**< 估算出的理想发送速率 */

	vector<TSubscirberInfo> m_SubscirberInfos;	/**< 提供服务的订阅者列表 */
//	unsigned int m_dwGroupID;							/**< PublishPort的标识 */
	
	int m_nRandCostBandWidth;  //随机驱动消费带宽累计
	int m_nRandBandWidth;
	int m_nCurrTimerClockLapse; //当前定时驱动的时间间隔
};

inline UF_INT8 CPublishPort::GetNextId()
{
	return m_flowReader.GetId();
}

inline bool CPublishPort::IsServeFor(unsigned int dwSubscriberID)
{
	for (int i=0; i<m_SubscirberInfos.size(); i++)
	{
		if (m_SubscirberInfos[i].dwSubscriberID == dwSubscriberID)
		{
			return true;
		}
	}
	return false;
}

inline int CPublishPort::GetSubscirberCount()
{
	return m_SubscirberInfos.size();
}

inline void CPublishPort::AddSubscriber(unsigned int dwSubscriberID, unsigned int dwLastActiveTime)
{
	TSubscirberInfo SubscirberInfo;
	SubscirberInfo.dwSubscriberID = dwSubscriberID;
	SubscirberInfo.dwLastActiveTime = dwLastActiveTime;
	m_SubscirberInfos.push_back(SubscirberInfo);
}

inline void CPublishPort::UpdSubscriber(unsigned int dwSubscriberID, unsigned int dwLastActiveTime)
{
	vector<TSubscirberInfo>::iterator i= m_SubscirberInfos.begin();
	for (; i != m_SubscirberInfos.end(); i++)
	{
		if ((*i).dwSubscriberID == dwSubscriberID)
		{
			(*i).dwLastActiveTime = dwLastActiveTime;
			break;
		}
	}
}

inline void CPublishPort::DelSubscriber(unsigned int dwSubscriberID)
{
	vector<TSubscirberInfo>::iterator i= m_SubscirberInfos.begin();
	for (; i != m_SubscirberInfos.end(); i++)
	{
		if ((*i).dwSubscriberID == dwSubscriberID)
		{
			m_SubscirberInfos.erase(i);
			break;
		}
	}
}

inline void CPublishPort::ChangeTransRate(int nDeltaTransRate)
{
	m_nTransRate += nDeltaTransRate;
}

inline int CPublishPort::GetTransRate()
{
	return m_nTransRate;
}

#endif // !defined(AFX_PUBLISHPORT_H__88F9C025_0AE8_4FBE_AAFD_56BAE65FECFD__INCLUDED_)
