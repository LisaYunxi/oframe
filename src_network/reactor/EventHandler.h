#ifndef AFX_EVENTHANDLER_H__
#define AFX_EVENTHANDLER_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "platform.h"

class  CReactor;
class  CEventHandler;

struct CEvent
{
	CEventHandler *pEventHandler;
	int nEventID;
	unsigned int dwParam;
	void *pParam;
	void *pAdd;
};

//CEventHandler��һ������Ŀ������еĶ������Ǹ���Э�顢����������ʱ���ĳ�����ࡣ
//�������¼��ɴ������У�һ��ΪIO��������һ��Ϊ��ʱ������һ��CEventHandler����ֻ��
//ע�ᵽCReactor���ܻ�ú�������ĵ���

class  CEventHandler  
{
public:
	CEventHandler(CReactor *pReactor);
	
	virtual ~CEventHandler();
	
	//����IO�����Ķ���������CReactor����
	//@return ����
	virtual int HandleInput(){return 0;};
	
	//����IO������д��������CReactor����
	//@return ����
	virtual int HandleOutput(){return 0;};
 	
	//����ʱ��������CReactor����
	//@param nIDEvent ��ʱ����ʶ   
	virtual void OnTimer(int nIDEvent){};
	
	//��CEventHandler�������첽�¼�
	//@param nEventID �¼�ID
	//@param dwParam ˫���Ͳ���
	//@param pParam �޷���ָ���Ͳ���
	bool PostEvent(int nEventID, unsigned int dwParam, void *pParam);

	//��CEventHandler������ͬ���¼�
	//@param nEventID �¼�ID
	//@param dwParam ˫���Ͳ���
	//@param pParam �޷���ָ���Ͳ���
	int SendEvent(int nEventID, unsigned int dwParam, void *pParam);

	//����һ���¼�
	//param nEventID �¼�ID
	//param dwParam ��һ������
	//param pParam �ڶ�������
	//return ������
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam){return 0;};

	//��ȡ����дsocket����selectʹ��
	//param pReadId   ������������ض���ʹ�õ�socket��Ϊ0��ʾ����Ҫ��
	//param pWriteId  �������������д��ʹ�õ�socket��Ϊ0��ʾ����Ҫд 
	virtual void GetIds(int *pReadId, int *pWriteId){};

	CReactor* getReactor() { return m_pReactor; };

protected:
	//���ö�ʱ��
	//param nIDEvent ��ʱ����ʶ(����Ϊ0)
	//param nElapse ��ʱ����(��)
	void SetTimer(int nIDEvent, int nElapse);

	//ȡ����ʱ��
	//param nIDEvent ��ʱ����ʶ(0��ʾȡ���ö�������ж�ʱ��)
	void KillTimer(int nIDEvent);

protected:
	CReactor *m_pReactor;    //��ǰ������������ָ��
};

#endif
