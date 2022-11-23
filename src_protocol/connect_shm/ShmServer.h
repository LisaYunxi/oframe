// ShmServer.h: interface for the CShmServer class .
//
//  
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ShmSERVER_H__FE5CED67_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_ShmSERVER_H__FE5CED67_6EC3_11D7_BB64_444553540000__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ServiceName.h"
#include "ShmCommon.h"
/**CShmServer��һ������Shm�������������
*/


class  CShmServer
{
public:
	/**���캯������ָ���ķ��������������������
	* @param  pName	ָ���ķ�������
	*/
	CShmServer(CServiceName *pName,int number,int shmsize);

	/**�����������ͷ�ռ�õ���Դ
	*/
	virtual ~CShmServer();

	void p();
	void v();
protected:
	/**����Shm�ĵȴ��ͻ������ӷ���
	* @param  wait	��ʾ��Ҫ�ȴ������Ӳŷ��أ�0��ʾ���ȴ�
	* @return NULL   ��Channelδ���ɹ�
	* @return ��NULL �µ�Channel
	*/
	void *ShmSet();

private:

	int m_iShmid;
	//�����ڴ��ַ
	void *m_pBaseShm;
	int m_iSemid;
	int m_iCount;
	int m_size;
#ifdef WIN32
    HANDLE hMapFile;
#endif

};

#endif // !defined(AFX_ShmSERVER_H__FE5CED67_6EC3_11D7_BB64_444553540000__INCLUDED_)
