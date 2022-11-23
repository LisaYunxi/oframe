// ShmChannel.h: interface for the CShmChannel class .
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ShmCHANNEL_H__FE5CED66_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_ShmCHANNEL_H__FE5CED66_6EC3_11D7_BB64_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Channel.h"

struct ConnectStruct{
		int iUsedFlag;
		int iPos;
		int iBuffSize;
		void *pBaseAddr;//
		int iType;//0 ��ʾserver�ˣ�1��ʾclient��
		int iUpWriteCount;//�ͻ����������д����
		int iUpReadCount;	
		int iDownWriteCount;//��������ͻ���д����
		int iDownReadCount;
	};

/**CShmChannel��һ������Shm���������Ӷ���
*/
class  CShmChannel : public CChannel
{
public:
	/**���캯����ʹ��ָ����socket�������˶���
	* @param id	ָ����socket
	*/
	CShmChannel(ConnectStruct  *pStruct);
	
	/**��������
	*/
	virtual ~CShmChannel(void);
	
	/**��ȡ�Է��ĵ�ַ��������IP��ַ��
	* @return	�Է��ĵ�ַ
	*/
	virtual char *GetRemoteName(void);

    /** ��ȡ���صĵ�ַIP
    * @return   ���ص�ַ
    * lidp 20140716
    */
    virtual char *GetLocalName(void);
protected:

	/**����Shm�Ķ�ȡʵ�ֺ���
	* @param number	buffer�������ĳ���
	* @param buffer	��Ż�����
	* @return >0 ʵ�ʶ�ȡ�ó���
	* @return 0  ��ȡʱ���жϷ���
	* @return -1 ����Ͽ�
	*/
	virtual int ReadImp(int number, char *buffer);


	/**����Shm��д��ʵ�ֺ���
	* @param number	׼�����͵ĳ���
	* @param buffer	��Ż�����
	* @return >0 ʵ�ʷ��͵ó���
	* @return 0  ��ȡʱ���жϷ���
	* @return -1 ����ʧ��
	*/
	virtual int WriteImp(int number, char *buffer);

	
	/**����Shm���жϿɶ���ʵ�ֺ���
	* @return true  �ɶ�
	* @return false ���ɶ�
	*/
	virtual bool AvailableImp(void);
	
	/**����Shm�ĶϿ�����ʵ�ֺ���
	* @return true  �Ͽ��ɹ�
	* @return false �Ͽ�ʧ��
	*/
	virtual bool DisconnectImp(void);
	
	/**����Shm�ļ��������ʵ�ֺ���
	* @return true  ��������
	* @return false ���ӶϿ�
	*/
	virtual bool CheckConnectionImp(void);

protected:
	DWORD m_dwInAddr; // �Զ�IP
  DWORD m_dwLocalAddr; // ����IP
  ConnectStruct m_stConnected;
  
};

#endif // !defined(AFX_ShmCHANNEL_H__FE5CED66_6EC3_11D7_BB64_444553540000__INCLUDED_)
