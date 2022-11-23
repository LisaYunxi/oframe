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
/**�����߻Ự�೧�����ڴ����͹����Ŷӷ�������һ�Ŷӷ���ĻỰ
*
*/
class CHotlineSessionFactory : public CAggrSessionFactory
{
public:
	/**���캯��
	* @param pReactor ������Ӧ�������������Ժ󴴽����ĻỰ
	* @param pCompServer �Ŷӷ���
	*/
	CHotlineSessionFactory(CReactor *pReactor, CCompServer *pCompServer, unsigned int dwDefaultAggrID);

	/**�����Ự����
	* @param dwAggregationID ����������ͨ�������������Ӵ�����
	* @return �������������߻Ự����
	*/
	virtual CSessionAggregation *CreateSessionAggregation(unsigned int dwAggregationID);
	
	/**�Ự���Ͻ��������
	* @param pSessionAggr �����ĻỰ����
	* @remark �ñ�����֪ͨ�Ŷӷ��������߻Ự�ѽ���
	*/
	virtual void OnSessionAggrLogin(CSessionAggregation *pSessionAggr);

	/**�Ự���϶Ͽ������
	* @param pSessionAggr �Ͽ��ĻỰ����
	* @param nReason �Ͽ���ԭ��
	* @remark �ñ�����֪ͨ�Ŷӷ��������߻Ự�Ͽ�
	*/
	virtual void OnSessionAggrDisconnected(CSessionAggregation *pSessionAggr, int nReason);

	virtual void OnSessionConnected(CSession *pSession);

private:
	CCompServer *m_pCompServer;			/**< �Ŷӷ��� */
};

/**�Ŷӷ�����Ϊ����Ӧ�õĵ������ġ�
* ��������һ��SessionFactory���������ٲõ����ӣ�ͬʱҪ���������߻Ự���ٲûỰ�ĸ����¼���
* �ٲ÷�������������״̬��ͬ�����ڸ����¼��Ĵ���Ҳ��ͬ������ʹ��״̬��ģʽ����������¼���
* �ٲ÷���ͬʱ��Ҫ�����׳�ʼ���������л�����������
*/
class CCompServer : public CAggrSessionFactory, public CArbPackageHandler, public CHotlinePackageHandler
{
public:
	/**���캯��
	* @param pReactor ������Ӧ��
	* @param pFlowFactory �����Ŷӷ������еķ����Ͷ��ĵ���������
	* @param dwTSeriesSubject ���������������
	* @param nTimeDifference ����ʱ�������ʱ��Ĳ�
	*/
	CCompServer(CReactor *pReactor, CCompFlowFactory *pFlowFactory,	unsigned int dwTSeriesSubject, unsigned int dwSubscriberID);
	virtual ~CCompServer();

	virtual CSessionAggregation *CreateSessionAggregation(unsigned int dwAggregationID);

	virtual void OnSessionAggrLogin(CSessionAggregation *pSessionAggr);

	virtual void OnSessionAggrDisconnected(CSessionAggregation *pSessionAggr, int nReason);

	/**����ʱ���¼�
	* @param nIDEvnet ��ʱ���¼����
	* @remark ��ʱ����ʱ�Ӱ�
	*/
	virtual void OnTimer(int nIDEvent);

	/**����һ���¼�
	* @param nEventID �¼�ID
	* @param dwParam ��һ������
	* @param pParam �ڶ�������
	* @return ������
	*/
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);

	/**ע�������ߵļ�����ַ
	* @param pszLocation �����ߵļ�����ַ
	*/
	void RegisterHotlineListen(const char *pszLocation);

	/**ע����һ�Ŷӷ��������ߵļ�����ַ
	* @param pszLocation ��һ�Ŷӷ��������ߵļ�����ַ
	*/
	void RegisterHotlineLocation(const char *pszLocation);

	/**������һ�Ŷӷ��񷢳��İ�
	* @param pPackage ��һ�Ŷӷ��񷢳��İ�
	* @param pSession ��һ�Ŷӷ���ĻỰ
	* @remark ������ǰ״̬������
	*/
	virtual void HandleHotlinePackage(CHotlinePackage *pPackage, CHotlineSession *pSession);

	/**�����ٲ÷��񷢳��İ�
	* @param pPackage �ٲ÷��񷢳��İ�
	* @param pSession �ٲ÷���ĻỰ
	* @remark ������ǰ״̬������
	*/
	virtual void HandleGuardPackage(CGuardPackage *pPackage, CGuardSession *pSession);
	
	//*���������������¼�
	//* @param pSession �����ߵĻỰ
	//* @remark ������ǰ״̬������
	void OnHotlineConnected(CHotlineSession *pSession);

	//*���������߶Ͽ��¼�
	//* @param pSession �����ߵĻỰ
	//* @param ErrorCode �Ͽ�ԭ��
	//* @remark ������ǰ״̬������
	void OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode);

	//�������ٲ÷��������¼�
	//@param pSession ���ٲ÷���ĻỰ
	//@remark ������ǰ״̬������
	void OnArbConnected(CGuardSession *pSession);

	//�������ٲ÷���Ͽ��¼�
	//@param pSession ���ٲ÷���ĻỰ
	//@param ErrorCode �Ͽ�ԭ��
	//@remark ������ǰ״̬������
	void OnGuardChannelLost(CGuardSession *pSession, int ErrorCode);
	
	/**�ı䵱ǰ״̬
	* @param newState ��״̬����
	*/
	void ChangeState(TCompStateID newState);

	/**֪ͨ�ٲ÷������ѵ�ǰ��״̬
	*/
	void ReportState();

	/**�������߻Ự��������
	* @param pSession �����ߵĻỰ
	* @param nStartId ��ʼλ�ã������￪ʼ��
	*/
	void HotlineAttachFlow(UF_INT8 nStartId);
	
	/**���ٲ÷��񱨸淢�����¼�
	* @nEvent �¼�����
	*/
	void NotifyArb(unsigned int nEvent);

	/**ʹ�������ߵ�ͨѶ����
	* @bEnable �Ƿ�������ߵ�ͨѶ����
	* @remark �������򵥻�״̬��Ҫ�򿪼���
	*/
	void EnableHotlineListen(bool bEnable);
	
	/**ʹ�������ߵ�ͨѶ����
	* @bEnable �Ƿ�������ߵ�ͨѶ����
	* @remark �ڴӻ���׼����״̬��Ҫ������
	*/
	void EnableHotlineConn(bool bEnable);


	/**ȡ�Ŷӻ���ǰ״̬
	*@return �Ŷӻ���ǰ״̬
	*/
	int GetCurrStateID();

	/**������guard���ӵĻỰ��������ʱʱ��
	*@param  dwArbHeartbeatTimeout ��guard���ӵĻỰ��������ʱʱ��
	*/
	void SetArbHeartbeatTimeout(unsigned int dwArbHeartbeatTimeout);

	/**��������һ�Ŷӻ����ӵĻỰ��������ʱʱ��
	*@param  dwArbHeartbeatTimeout ����һ�Ŷӻ����ӵĻỰ��������ʱʱ��
	*/
	void SetHotlineHeartbeatTimeout(unsigned int dwHotlineHeartbeatTimeout);

	//*�ӻ�ȷ�����źöӵİ���������״̬�±�����
	//* @param pPackage ȷ�ϰ�
	//* @remark ִ����÷����󣬰�����������������
	void ConfirmQueued(CHotlinePackage *pPackage);

	/**����ȷ�����źöӵİ����ڵ���״̬�±�����
	* @param nKey ȷ�ϵ��źöӵİ������
	* @remark ִ����÷����󣬰�����������������
	*/
	void ConfirmQueued(UF_INT8 nKey);
	
	/**ȷ��������δ�õ�ȷ�ϵİ�
	* @remark ִ����÷����󣬰�����������������
	*/
	void ConfirmAllQueued();

	/**������λָ�׼����״̬�ָ�����ʼֵ
	* @param pszResetReason ˵����λ��ԭ��
	*/
	void PostResetEvent(const char *pszResetReason);

	//��״̬�ָ�����ʼֵ
	void Reset();
	
	//��ȡ�����߳���,�������¼�֪ͨ
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

	CCompState *m_pCurrState;							/**< ��ǰ״̬���� */
	CCompFlowFactory *m_pFlowFactory;					/**< �����Ŷӷ������еķ����Ͷ��ĵ��������� */
	unsigned int m_dwTSeriesSubject;
	CHotlineSessionFactory *m_pHotlineSessionFactory;	/**< ����һ�Ŷӷ���ĻỰ������ */
	CGuardSession *m_pArbSession;							/**< ���ٲ÷���ĻỰ */
	CHotlineSession *m_pHotlineSession;					/**< ����һ�Ŷӷ���ĻỰ */


	unsigned int m_dwArbHeartbeatTimeout;						/**< ��guard���ӵĻỰ��������ʱʱ�� */
	unsigned int m_dwHotlineHeartbeatTimeout;					/**< ����һ�Ŷӻ����ӵĻỰ��������ʱʱ�� */

	friend class CCompInitState;
	friend class CCompSingleState;
	friend class CCompPrimaryState;
	friend class CCompSecondaryState;
	friend class CCompPrepareState;
	friend class CHotlineSessionFactory;

	CStatusCtrlInterfaces *m_pStatusCtrlEng;
};

#endif
