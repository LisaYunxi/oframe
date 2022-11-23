// EventHandler.h: interface for the CEventHandler class .
// 20170111 yangyong �������ļ�
//////////////////////////////////////////////////////////////////////

#if !defined(__BLOCKEVENTHANDLER_H__)
#define __BLOCKEVENTHANDLER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "platform.h"

class  CBlockReactor;
class  CBlockEventHandler;

inline void default_release(void *pParam)
{
	free(pParam);
}

typedef void (*release_func)(void* pParam);

struct CBlockEvent
{
	CBlockEventHandler *pEventHandler;
	int nEventID;
	DWORD dwParam;
	void *pParam;
	void *pSync;
	release_func pReleaseFunc;
};

/**
* CEventHandler��һ������Ŀ������еĶ������Ǹ���Э�顢����������ʱ���ĳ�����ࡣ
* �������¼��ɴ������У�һ��ΪIO��������һ��Ϊ��ʱ������һ��CEventHandler����ֻ��
* ע�ᵽCReactor���ܻ�ú�������ĵ���
*/
class  CBlockEventHandler  
{
public:
	/**���캯��
	* @param pReactor ��ǰ���ж������������ 
	*/
	CBlockEventHandler(CBlockReactor *pReactor);
	
	/**��������
	*/
	virtual ~CBlockEventHandler();
	
	/**����IO�����Ķ���������CReactor����
	* @return ����
	*/
	virtual int HandleInput(){return 0;};
	
	/**����IO������д��������CReactor����
	* @return ���� 
	*/
	virtual int HandleOutput(){return 0;};
 	
	/**����ʱ��������CReactor����
	* @param nIDEvent ��ʱ����ʶ   
	*/
	virtual void OnTimer(int nIDEvent){};
	
	/*��CEventHandler�������첽�¼�
	* @param nEventID �¼�ID
	* @param dwParam ˫���Ͳ���
	* @param pParam �޷���ָ���Ͳ���
	*/
	bool PostEvent(int nEventID, DWORD dwParam, void *pParam, release_func pReleaseFunc);

	/*��CEventHandler������ͬ���¼�
	* @param nEventID �¼�ID
	* @param dwParam ˫���Ͳ���
	* @param pParam �޷���ָ���Ͳ���
	*/
	int SendEvent(int nEventID, DWORD dwParam, void *pParam);

	/**����һ���¼�
	* @param nEventID �¼�ID
	* @param dwParam ��һ������
	* @param pParam �ڶ�������
	* @return ������
	*/
	virtual int HandleEvent(int nEventID, DWORD dwParam, void *pParam, release_func pReleaseFunc){return 0;};

	/**��ȡ����дsocket����selectʹ��
	* @param pReadId   ������������ض���ʹ�õ�socket��Ϊ0��ʾ����Ҫ��
	* @param pWriteId  �������������д��ʹ�õ�socket��Ϊ0��ʾ����Ҫд 
	*/
	virtual void GetIds(int *pReadId, int *pWriteId){};

protected:

	/**���ö�ʱ��
	* @param nIDEvent ��ʱ����ʶ(����Ϊ0)
	* @param nElapse ��ʱ����(��)
	* ��ʱ��ֻ����ͬһ���߳������� ��û�м���
	*/
	void SetTimer(int nIDEvent, int nElapse);

	/**ȡ����ʱ��
	* @param nIDEvent ��ʱ����ʶ(0��ʾȡ���ö�������ж�ʱ��)
	*/
	void KillTimer(int nIDEvent);

protected:
	CBlockReactor *m_pReactor;    /**<��ǰ������������ָ��*/
};

#endif // !defined(__BLOCKEVENTHANDLER_H__)
