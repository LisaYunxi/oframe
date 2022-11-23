// ShmServer.h: interface for the CShmServer class .
//
//  
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ShmSERVER_H__FE5CED67_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_ShmSERVER_H__FE5CED67_6EC3_11D7_BB64_444553540000__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Server.h"
#include "ShmChannel.h"

/**CShmServer��һ������Shm�������������
*/
class  CShmServer : public CServerBase
{
public:
	/**���캯������ָ���ķ��������������������
	* @param  pName	ָ���ķ�������
	*/
	CShmServer(CServiceName *pName);
	
	/**�����������ͷ�ռ�õ���Դ
	*/
	virtual ~CShmServer();
	
	/** ����Shm�Ļ�ȡselect�ļ��ŷ���
	* @return selectʱ���ļ���
	*/
	virtual int GetId(void);
	
	void p();
	void v();
protected:
	/**����Shm�ĵȴ��ͻ������ӷ���
	* @param  wait	��ʾ��Ҫ�ȴ������Ӳŷ��أ�0��ʾ���ȴ�
	* @return NULL   ��Channelδ���ɹ�
	* @return ��NULL �µ�Channel
	*/
	virtual CChannel *Accept(int wait);

	virtual CChannel *CreateChannel(ConnectStruct  *pStruct);
private:
	int m_nID;	/**< �洢���������˵��ļ��� */
	int m_iShmid;
	void *m_pBaseShm;
	int m_iSemid;
	int m_iCount;
};

#endif // !defined(AFX_ShmSERVER_H__FE5CED67_6EC3_11D7_BB64_444553540000__INCLUDED_)
