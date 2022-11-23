// EventHandler.h: interface for the CPipeEventHandler class .
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPipeEventHandler_H__D0DAF147_AD16_4C5C_9393_78E357E0A93E__INCLUDED_)
#define AFX_CPipeEventHandler_H__D0DAF147_AD16_4C5C_9393_78E357E0A93E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "EventHandler.h"
//#include "CachedFlow.h"
#include "public.h"

class CSelectReactor;

/**
* CPipeEventHandler��һ��֪ͨ���������Ҫ��������CSelectReactor,���������˯�ߣ�
* �Ӷ������䲻�ܼ�ʱ�������Զ��е�����
*/
class CPipeEventHandler: public CEventHandler,public CFlowCallback
{
public:
	/**���캯��
	* @param pReactor ��ǰ���ж������������ 
	*/
	CPipeEventHandler(CReactor *pReactor);
	
	/**��������
	*/
	virtual ~CPipeEventHandler();
	
	/**����IO�����Ķ���������CReactor����
	* @return ����
	*/
	virtual int HandleInput();
	
	/**��ȡ����дsocket����selectʹ��
	* @param pReadId   ������������ض���ʹ�õ�socket��Ϊ0��ʾ����Ҫ��
	* @param pWriteId  �������������д��ʹ�õ�socket��Ϊ0��ʾ����Ҫд 
	*/
	virtual void GetIds(int *pReadId, int *pWriteId);
	
	/**����֪ͨ
	*/
	virtual void Notify();
	
	/*��ȡ����ʱ��
	*/
	unsigned int GettMilSec();

protected:
	void CreatePipe(int &p0, int &p1);

	int CreateTmpServer(int &port);
	int CreateTmpClient(int port, int &p1);

protected:
	CReactor *m_pDestReactor;
	int             m_nReadPipe;
	int             m_nWritePipe;
	unsigned int 		m_nPreClock;
	unsigned int 		m_nPerCnt;
	bool			m_SendFlag;
	char 			m_buffer[10];
	
	volatile unsigned int     m_nWriteCnt;
	volatile unsigned int     m_nReadCnt;

//	int	m_nLocalPivatePort;  //WIN32ר��
};

#endif !defined(AFX_EVENTHANDLER_H__D0DAF147_AD16_4C5C_9393_78E357E0A93E__INCLUDED_)
