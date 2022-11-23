// ShmClient.h: interface for the CShmClient class .
//
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ShmCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_ShmCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ServiceName.h"
#include "ShmCommon.h"

/**CShmClient��һ������Shm������ͻ���
*/


class  CShmClient
{
public:
	/**���캯������ɳ�ʼ��
	*/
	CShmClient();
	
	/**��������
	*/
	virtual ~CShmClient();

	/**����Shm������ʵ�ֺ���
	*/
	void *ShmConnect(CServiceName *pName,int number);
	
	void p();
	void v();
protected:
	//CChannel *CreateChannel(ConnectStruct  *pStruct);
	int shmsize;
private:

	int m_iShmid;
	//�����ڴ��ַ
	void *m_pShm;
	int m_iSemid;
	ConnectStruct m_stConnected;

#ifdef WIN32
    HANDLE hMapFile;
#endif
};


#endif // !defined(AFX_ShmCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_)
