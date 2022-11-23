#ifndef _COMPSERVER_H__
#define _COMPSERVER_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "AggrSessionFactory.h"
#include "CompFlowFactory.h"
#include "ArbPackage.h"
#include "HotlineSession.h"
#include "ArbSession.h"
#include "CDate.h"
#include "monitorIndex.h"


class CCompState;
class CCompServer;

class CStatusCtrlInterfaces
{
public:
	virtual bool EnterSlaver() = 0;
	virtual bool EnterSingle() = 0;
	virtual bool EnterMaster() = 0;
	virtual void ConfirmQueued(UF_INT8 nKey) = 0;
};
/**心跳线会话类厂，用于创建和管理排队服务与另一排队服务的会话
*
*/
class CHotlineSessionFactory : public CAggrSessionFactory
{
public:
	/**构造函数
	* @param pReactor 驱动反应器，用于驱动以后创建出的会话
	* @param pCompServer 排队服务
	*/
	CHotlineSessionFactory(CReactor *pReactor, CCompServer *pCompServer, unsigned int dwDefaultAggrID);

	/**创建会话集合
	* @param dwAggregationID 网络连接是通过监听还是连接创建的
	* @return 创建出的心跳线会话集合
	*/
	virtual CSessionAggregation *CreateSessionAggregation(unsigned int dwAggregationID);
	
	/**会话集合建立后调用
	* @param pSessionAggr 建立的会话集合
	* @remark 用本方法通知排队服务心跳线会话已建立
	*/
	virtual void OnSessionAggrLogin(CSessionAggregation *pSessionAggr);

	/**会话集合断开后调用
	* @param pSessionAggr 断开的会话集合
	* @param nReason 断开的原因
	* @remark 用本方法通知排队服务心跳线会话断开
	*/
	virtual void OnSessionAggrDisconnected(CSessionAggregation *pSessionAggr, int nReason);

	virtual void OnSessionConnected(CSession *pSession);

private:
	CCompServer *m_pCompServer;			/**< 排队服务 */
};

/**排队服务，做为整个应用的调度中心。
* 它首先是一个SessionFactory，管理与仲裁的连接，同时要处理心跳线会话、仲裁会话的各种事件。
* 仲裁服务由于所处的状态不同，对于各种事件的处理也不同，这里使用状态机模式来处理各种事件。
* 仲裁服务同时还要处理交易初始化发出的切换交易日请求。
*/
class CCompServer : public CAggrSessionFactory, public CArbPackageHandler, public CHotlinePackageHandler
{
public:
	/**构造函数
	* @param pReactor 驱动反应器
	* @param pFlowFactory 管理排队服务所有的发布和订阅的流管理器
	* @param dwTSeriesSubject 交易序列主题代码
	* @param nTimeDifference 交易时间与机器时间的差
	*/
	CCompServer(CReactor *pReactor, CCompFlowFactory *pFlowFactory,	unsigned int dwTSeriesSubject, unsigned int dwSubscriberID);
	virtual ~CCompServer();

	virtual CSessionAggregation *CreateSessionAggregation(unsigned int dwAggregationID);

	virtual void OnSessionAggrLogin(CSessionAggregation *pSessionAggr);

	virtual void OnSessionAggrDisconnected(CSessionAggregation *pSessionAggr, int nReason);

	/**处理定时器事件
	* @param nIDEvnet 定时器事件编号
	* @remark 定时发出时钟包
	*/
	virtual void OnTimer(int nIDEvent);

	/**处理一条事件
	* @param nEventID 事件ID
	* @param dwParam 第一个参数
	* @param pParam 第二个参数
	* @return 处理结果
	*/
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);

	/**注册心跳线的监听地址
	* @param pszLocation 心跳线的监听地址
	*/
	void RegisterHotlineListen(const char *pszLocation);

	/**注册另一排队服务心跳线的监听地址
	* @param pszLocation 另一排队服务心跳线的监听地址
	*/
	void RegisterHotlineLocation(const char *pszLocation);

	/**处理另一排队服务发出的包
	* @param pPackage 另一排队服务发出的包
	* @param pSession 另一排队服务的会话
	* @remark 交给当前状态对象处理
	*/
	virtual void HandleHotlinePackage(CHotlinePackage *pPackage, CHotlineSession *pSession);

	/**处理仲裁服务发出的包
	* @param pPackage 仲裁服务发出的包
	* @param pSession 仲裁服务的会话
	* @remark 交给当前状态对象处理
	*/
	virtual void HandleGuardPackage(CGuardPackage *pPackage, CGuardSession *pSession);
	
	//*处理心跳线连接事件
	//* @param pSession 心跳线的会话
	//* @remark 交给当前状态对象处理
	void OnHotlineConnected(CHotlineSession *pSession);

	//*处理心跳线断开事件
	//* @param pSession 心跳线的会话
	//* @param ErrorCode 断开原因
	//* @remark 交给当前状态对象处理
	void OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode);

	//处理与仲裁服务连接事件
	//@param pSession 与仲裁服务的会话
	//@remark 交给当前状态对象处理
	void OnArbConnected(CGuardSession *pSession);

	//处理与仲裁服务断开事件
	//@param pSession 与仲裁服务的会话
	//@param ErrorCode 断开原因
	//@remark 交给当前状态对象处理
	void OnGuardChannelLost(CGuardSession *pSession, int ErrorCode);
	
	/**改变当前状态
	* @param newState 新状态代码
	*/
	void ChangeState(TCompStateID newState);

	/**通知仲裁服务自已当前的状态
	*/
	void ReportState();

	/**将心跳线会话绑定在流上
	* @param pSession 心跳线的会话
	* @param nStartId 起始位置，从这里开始序传
	*/
	void HotlineAttachFlow(UF_INT8 nStartId);
	
	/**向仲裁服务报告发生的事件
	* @nEvent 事件代码
	*/
	void NotifyArb(unsigned int nEvent);

	/**使能心跳线的通讯监听
	* @bEnable 是否打开心跳线的通讯监听
	* @remark 在主机或单机状态需要打开监听
	*/
	void EnableHotlineListen(bool bEnable);
	
	/**使能心跳线的通讯连接
	* @bEnable 是否打开心跳线的通讯连接
	* @remark 在从机或准备机状态需要打开连接
	*/
	void EnableHotlineConn(bool bEnable);


	/**取排队机当前状态
	*@return 排队机当前状态
	*/
	int GetCurrStateID();

	/**设置与guard连接的会话的心跳超时时间
	*@param  dwArbHeartbeatTimeout 与guard连接的会话的心跳超时时间
	*/
	void SetArbHeartbeatTimeout(unsigned int dwArbHeartbeatTimeout);

	/**设置与另一排队机连接的会话的心跳超时时间
	*@param  dwArbHeartbeatTimeout 与另一排队机连接的会话的心跳超时时间
	*/
	void SetHotlineHeartbeatTimeout(unsigned int dwHotlineHeartbeatTimeout);

	//*从机确认了排好队的包，在主机状态下被调用
	//* @param pPackage 确认包
	//* @remark 执行完该方法后，包将发布到交易序列
	void ConfirmQueued(CHotlinePackage *pPackage);

	/**本地确认了排好队的包，在单机状态下被调用
	* @param nKey 确认的排好队的包的序号
	* @remark 执行完该方法后，包将发布到交易序列
	*/
	void ConfirmQueued(UF_INT8 nKey);
	
	/**确认所有沿未得到确认的包
	* @remark 执行完该方法后，包将发布到交易序列
	*/
	void ConfirmAllQueued();

	/**发出复位指令，准备将状态恢复到初始值
	* @param pszResetReason 说明复位的原因
	*/
	void PostResetEvent(const char *pszResetReason);

	//将状态恢复到初始值
	void Reset();
	
	//获取驱动线程类,产生新事件通知
	CReactor *GetReactor();
		
	enum{
		COMP_TIMER_HOTLINE_ID = 0x000001001
	};

	enum{
		COMP_TIMER_HOTLINE_INTERVAL	= 500
	};

	void SetStatusCtrlEng(CStatusCtrlInterfaces *pStatusCtrlEng);

private:
	unsigned int m_dwSubscriberID;

	CCompState *m_pCurrState;							/**< 当前状态对象 */
	CCompFlowFactory *m_pFlowFactory;					/**< 管理排队服务所有的发布和订阅的流管理器 */
	unsigned int m_dwTSeriesSubject;
	CHotlineSessionFactory *m_pHotlineSessionFactory;	/**< 与另一排队服务的会话管理器 */
	CGuardSession *m_pArbSession;							/**< 与仲裁服务的会话 */
	CHotlineSession *m_pHotlineSession;					/**< 与另一排队服务的会话 */


	unsigned int m_dwArbHeartbeatTimeout;						/**< 与guard连接的会话的心跳超时时间 */
	unsigned int m_dwHotlineHeartbeatTimeout;					/**< 与另一排队机连接的会话的心跳超时时间 */

	friend class CCompInitState;
	friend class CCompSingleState;
	friend class CCompPrimaryState;
	friend class CCompSecondaryState;
	friend class CCompPrepareState;
	friend class CHotlineSessionFactory;

	CStatusCtrlInterfaces *m_pStatusCtrlEng;
};

#endif
