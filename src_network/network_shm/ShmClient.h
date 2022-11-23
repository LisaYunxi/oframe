// ShmClient.h: interface for the CShmClient class .
//
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ShmCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_ShmCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Client.h"
#include "ShmChannel.h"
#include <sys/sem.h>
/**CShmClient��һ������Shm������ͻ���
*/
class  CShmClient : public CClientBase
{
public:
	/**���캯������ɳ�ʼ��
	*/
	CShmClient();
	
	/**��������
	*/
	virtual ~CShmClient();

	/**����Shm������ʵ�ֺ���
	* @param pName ��������������Ϣ
	* @return ��NULL �ɹ����ѽ�����Channel
	* @return NULL ʧ��
	*/
	virtual CChannel *Connect(CServiceName *pName);
	void p();
	void v();
protected:
	CChannel *CreateChannel(ConnectStruct  *pStruct);
private:
	int m_iShmid;
	void *m_pShm;
	int m_iSemid;
	ConnectStruct m_stConnected;
};


#endif // !defined(AFX_ShmCLIENT_H__FE5CED65_6EC3_11D7_BB64_444553540000__INCLUDED_)
