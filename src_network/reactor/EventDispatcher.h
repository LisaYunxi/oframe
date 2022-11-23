#ifndef AFX_EVENTDISPATCHER_H__
#define AFX_EVENTDISPATCHER_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Thread.h"
#include "EventHandler.h"
#include "EventQueue.h"
#include "TimerQueue.h"

const int SM_USER = 0x1000;


class CEventDispatcher : public CThread
{
public:
	CEventDispatcher();
	virtual ~CEventDispatcher();

	//ע��һ����ʱ������ж���
	//@param pEventHandler ��ʱ����ָ��
	//@param nIDEvent ��ʱ���ı�ʾ
	//@param nElapse ��ʱ����ʱ���� ����
	void RegisterTimer(CEventHandler *pEventHandler, int nIDEvent, int nElapse);

	//�Ӷ�ʱ����������ȥһ����ʱ������
	//@param pEventHandler ��ʱ�������ָ��
	//@param nIDEvent ��ʱ����CReatctor�ı�ʾ��=0 ��ȥ�ö�ʱ������ע�� >0 ��ȥ�ö�ʱ��ָ��ע��
	void RemoveTimer(CEventHandler *pEventHandler, int nIDEvent);

	//��EventHandler�����첽�¼�
	//@param pEventHander Ŀ��EventHandler
	//@param nEventID �¼�ID
	//@param dwParam ˫���Ͳ���
	//@param pParam �޷���ָ���Ͳ���
	bool PostEvent(CEventHandler *pEventHandler, int nEventID, unsigned int dwParam, void *pParam);

	//��EventHandler����ͬ���¼�
	//@param pEventHander Ŀ��EventHandler
	//@param nEventID �¼�ID
	//@param dwParam ˫���Ͳ���
	//@param pParam �޷���ָ���Ͳ���
	int SendEvent(CEventHandler *pEventHandler, int nEventID, unsigned int dwParam, void *pParam);

	//û���¼�������������¼�ʱ����Reactor�����������¼�,
	//@param nEventID �¼�ID
	//@param dwParam ��һ������
	//@param pParam �ڶ�������
	//@return ������
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);

	//���һ��ʧЧ��EventHandler���������е��¼�
	//@param pEventHander ʧЧ��EventHandler
	inline void ClearEvent(CEventHandler *pEventHandler);

	//CReactor��ʼ����
	virtual void Run();

	//��ȡ��ǰϵͳʱ��
	//@return ��ǰϵͳʱ��
	inline unsigned int Time();

	//��ȡ��ǰϵͳʱ��ĺ��벿��
	//@return ��ǰϵͳʱ��ĺ��벿��
	inline unsigned int GetMilTime();

	//��ȡ��ǰϵͳ������
	//return  ��ǰϵͳ���������Ժ����)
	inline time_t GetClock();

	//��ֹ������ѭ��
	void Stop(unsigned int nStopCode = 0);

	inline unsigned int GetStopCode();

protected:
	//��鶨ʱ�������Ƿ��е����¼���������ִ�ж�ʱ��������OnTimer()����
	void CheckTimer();

	//��ȡ��ǰϵͳʱ��
	inline void SyncTime();

	//���¼�������ȡ���¼������ɸ���Ӧ���¼�������
	void DispatchEvents();
private:
	virtual void DispatchIOs() = 0;

protected:
	bool m_bShouldRun;				//ѭ�����б�־
	unsigned int m_nCurrMilTime;	//��ǰϵͳʱ�����
	unsigned int m_nCurrTime;		//��ǰϵͳʱ����
	time_t m_nCurrClock;			//��ǰϵͳ������

	CEventQueue m_queueEvent;		//�¼�����
	CTimerQueue *m_pTimerQueue;

	unsigned int m_nStopCode;
	CMutex m_lockActive;
};

inline unsigned int CEventDispatcher::Time()
{
	return m_nCurrTime;
}

inline unsigned int CEventDispatcher::GetMilTime()
{
	return m_nCurrMilTime;
}

inline time_t CEventDispatcher::GetClock()
{
	return m_nCurrClock;
}

inline void CEventDispatcher::SyncTime()
{
#ifdef WIN32	
	struct _timeb timebuffer;
	_ftime(&timebuffer);
	m_nCurrTime = (unsigned int)timebuffer.time;
	m_nCurrMilTime = timebuffer.millitm;
	m_nCurrClock = m_nCurrTime * 1000 + timebuffer.millitm;
#else
	struct timeval timeout;
	gettimeofday(&timeout, 0);
	m_nCurrTime = timeout.tv_sec;
	m_nCurrMilTime = timeout.tv_usec / 1000;
	m_nCurrClock = m_nCurrTime * 1000 + timeout.tv_usec / 1000;
#endif
}

inline void CEventDispatcher::ClearEvent(CEventHandler *pEventHandler)
{
	m_queueEvent.ClearEvent(pEventHandler);
}

inline unsigned int CEventDispatcher::GetStopCode()
{
	return m_nStopCode;
}

#endif
