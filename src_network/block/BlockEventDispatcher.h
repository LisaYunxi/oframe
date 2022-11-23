// BlockEventDispatcher.h: interface for the CBlockEventDispatcher class.
// 20170711 yangyong �������ļ�
//////////////////////////////////////////////////////////////////////

#if !defined(__BLOCKEVENTDISPATCHER_H__)
#define __BLOCKEVENTDISPATCHER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"
#include "BlockEventHandler.h"
#include "BlockEventQueue.hpp"
#include "BlockTimerQueue.h"

enum SPEED_MODE
{
	SM_HIGHT = 0,	// ����
	SM_MEDIUM,		// ����
	SM_LOW,			// ����
};

class CBlockEventDispatcher : public CThread  
{
public:
	CBlockEventDispatcher(SPEED_MODE sm);
	virtual ~CBlockEventDispatcher();

	/**ע��һ����ʱ������ж���
	* @param pEventHandler ��ʱ����ָ�� 
	* @param nIDEvent ��ʱ���ı�ʾ 
	* @param nElapse ��ʱ����ʱ���� ����
	*/
	void RegisterTimer(CBlockEventHandler *pEventHandler, int nIDEvent, int nElapse);

	/**�Ӷ�ʱ����������ȥһ����ʱ������
	* @param pEventHandler ��ʱ�������ָ�� 
	* @param nIDEvent ��ʱ����CReatctor�ı�ʾ��=0 ��ȥ�ö�ʱ������ע��
	*        >0 ��ȥ�ö�ʱ��ָ��ע��
	*/
	void RemoveTimer(CBlockEventHandler *pEventHandler, int nIDEvent);
	
	/*��EventHandler�����첽�¼�
	* @param pEventHander Ŀ��EventHandler
	* @param nEventID �¼�ID
	* @param dwParam ˫���Ͳ���
	* @param pParam �޷���ָ���Ͳ���
	*/
	bool PostEvent(CBlockEventHandler *pEventHandler, int nEventID, DWORD dwParam, void *pParam, release_func pReleaseFunc);

	/*��EventHandler����ͬ���¼�
	* @param pEventHander Ŀ��EventHandler
	* @param nEventID �¼�ID
	* @param dwParam ˫���Ͳ���
	* @param pParam �޷���ָ���Ͳ���
	*/
	int SendEvent(CBlockEventHandler *pEventHandler, int nEventID, DWORD dwParam, void *pParam);

	/**û���¼�������������¼�ʱ����Reactor�����������¼�,
	* @param nEventID �¼�ID
	* @param dwParam ��һ������
	* @param pParam �ڶ�������
	* @return ������
	*/
	virtual int HandleEvent(int nEventID, DWORD dwParam, void *pParam, release_func pReleaseFunc);
	
	/**���һ��ʧЧ��EventHandler���������е��¼�
	* @param pEventHander ʧЧ��EventHandler
	*/
	//inline void ClearEvent(CEventHandler *pEventHandler);
	/**CReactor��ʼ����
	*/
	virtual void Run();
	
	/**��ȡ��ǰϵͳʱ��
	* @return ��ǰϵͳʱ��
	*/
	inline DWORD Time();

	/**��ȡ��ǰϵͳʱ��ĺ��벿��
	* @return ��ǰϵͳʱ��ĺ��벿��
	*/
	inline DWORD GetMilTime();
	
	/**��ȡ��ǰϵͳ������
	*@return  ��ǰϵͳ���������Ժ����)
	*/
	inline DWORD GetClock();
	
	/**��ֹ������ѭ��
	*/
	void Stop(DWORD nStopCode = 0);
	
	inline DWORD GetStopCode();
protected:
	/**��鶨ʱ�������Ƿ��е����¼���������ִ�ж�ʱ��������OnTimer()����
	*/
	void CheckTimer();
	
	/**��ȡ��ǰϵͳʱ��
	*/
	inline void SyncTime();

	inline DWORD SyncMilTime(); // ��ȡ��ǰ�ĺ���

	/**���¼�������ȡ���¼������ɸ���Ӧ���¼�������
	*/
	void DispatchEvents();

protected:
	bool m_bShouldRun;				/**< ѭ�����б�־ */
	DWORD m_nCurrMilTime;			/**< ��ǰϵͳʱ�����*/
	DWORD m_nCurrTime;				/**< ��ǰϵͳʱ����*/
	DWORD m_nCurrClock;				/**< ��ǰϵͳ������*/

	ConcurrentQueue<CBlockEvent> m_queueEvent;		/**< �¼�����*/
	CBlockTimerQueue *m_pTimerQueue;

	DWORD m_nStopCode;

	CMutex m_lockActive;

	DWORD m_nUpdateTime;  // ���¸��µ�ʱ��

	SPEED_MODE m_speedMode;
};

inline DWORD CBlockEventDispatcher::Time()
{
	return m_nCurrTime;
}

inline DWORD CBlockEventDispatcher::GetMilTime()
{
	return m_nCurrMilTime;
}

inline DWORD CBlockEventDispatcher::GetClock()
{
	return m_nCurrClock;
}

inline void CBlockEventDispatcher::SyncTime()
{
#ifdef WIN32	
   struct _timeb timebuffer;
   _ftime( &timebuffer );
	m_nCurrTime = (DWORD)timebuffer.time;	
	m_nCurrMilTime = timebuffer.millitm;
	m_nCurrClock = m_nCurrTime*1000 + timebuffer.millitm;
#else
	struct timeval timeout;
  gettimeofday(&timeout, 0);
	m_nCurrTime = timeout.tv_sec;	
	m_nCurrMilTime = timeout.tv_usec/1000;
	m_nCurrClock = m_nCurrTime*1000 + timeout.tv_usec/1000;
#endif
}

inline DWORD CBlockEventDispatcher::SyncMilTime()
{
	DWORD dwRet = 0;
#ifdef WIN32	
   struct _timeb timebuffer;
   _ftime( &timebuffer );
	dwRet = timebuffer.millitm;
#else
	struct timeval timeout;
	gettimeofday(&timeout, 0);
	dwRet = timeout.tv_usec/1000;
#endif
	return dwRet;
}


inline DWORD CBlockEventDispatcher::GetStopCode()
{
	return m_nStopCode;
}

#endif // !defined(__BLOCKEVENTDISPATCHER_H__)
