#ifndef _THREAD_H__
#define _THREAD_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "platform.h"
#include "public.h"

#ifdef WIN32
//#include <windows.h>
typedef HANDLE THREAD_HANDLE;
#else
//#include <pthread.h>
typedef pthread_t THREAD_HANDLE;

void BindThd2CPU(pthread_t thdid, int ncpuid);
#endif


class  CThread
{
public:

	CThread();
	virtual ~CThread();

	//����һ���߳�
	// @return true:�����ɹ� false:����ʧ��
	virtual bool Create();

	//��ñ��̶߳���洢���߳̾��
	// @return ���̶߳���洢���߳̾���߳̾��
	inline THREAD_HANDLE GetHandle();

	//�߳�˯��seconds��
	// @param seconds ˯������
	void OSSleep(int nSeconds);

	void SleepMs(int nMilliseconds);

	bool Join();

	bool IsCurrentThread();

	void ExitThread();
#ifdef WIN32
	unsigned long m_IDThread;
#else
	int m_IDThread;
#endif
private:
#ifdef WIN32
	static DWORD WINAPI _ThreadEntry(LPVOID pParam);
#else
	static void * _ThreadEntry(void *pParam);
#endif

	//�麯�����������һЩʵ��������
	// @return true:�����ɹ� false:����ʧ��
	virtual bool InitInstance();

	//�麯�����������ʵ��
	virtual void ExitInstance();

	//�߳̿�ʼ���У����麯�����������̳�ʵ��
	virtual void Run() = 0;

private:
	THREAD_HANDLE m_hThread;	// �߳̾��/
};

inline THREAD_HANDLE CThread::GetHandle()
{
	return m_hThread;
}

#endif
