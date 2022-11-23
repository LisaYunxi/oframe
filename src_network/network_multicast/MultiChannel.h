// MultiChannel.h: interface for the CMultiChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MultiCHANNEL_H__6DE6311B_C987_4246_A61D_53ADA1F3D619__INCLUDED_)
#define AFX_MultiCHANNEL_H__6DE6311B_C987_4246_A61D_53ADA1F3D619__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Channel.h"

struct sockaddr_in;

class CMultiChannel : public CChannel
{
public:
	CMultiChannel(int id, char *pTargetAddress, unsigned short wTargetPort);
	virtual ~CMultiChannel();
			
	void SetMultiCastIP(char *pMultiCastIP,char *pHostIP);
	bool LeaveMultiGroup();

	//���ضಥ��ַ
	virtual char *GetRemoteName(void);

protected:
	/**����TCP�Ķ�ȡʵ�ֺ���
	* @param number	buffer�������ĳ���
	* @param buffer	��Ż�����
	* @return >0 ʵ�ʶ�ȡ�ó���
	* @return 0  ��ȡʱ���жϷ���
	* @return -1 ����Ͽ�
	*/
	virtual int ReadImp(int number, char *buffer);


	/**����TCP��д��ʵ�ֺ���
	* @param number	׼�����͵ĳ���
	* @param buffer	��Ż�����
	* @return >0 ʵ�ʷ��͵ó���
	* @return 0  ��ȡʱ���жϷ���
	* @return -1 ����ʧ��
	*/
	virtual int WriteImp(int number, char *buffer);

	
	/**����TCP���жϿɶ���ʵ�ֺ���
	* @return true  �ɶ�
	* @return false ���ɶ�
	*/
	virtual bool AvailableImp(void);
	
	/**����TCP�ĶϿ�����ʵ�ֺ���
	* @return true  �Ͽ��ɹ�
	* @return false �Ͽ�ʧ��
	*/
	virtual bool DisconnectImp(void);
	
	/**����TCP�ļ��������ʵ�ֺ���
	* @return true  ��������
	* @return false ���ӶϿ�
	*/
	virtual bool CheckConnectionImp(void);


private:
	sockaddr_in *m_pTargetAddr; //��������ַ�ṹ
	char *m_MultiCastIP;	/*��Ŷಥ��ַ*/
	char *m_sHostIP;
	char *m_sMultiCastName;
	unsigned short m_nPort;
};

inline char *CMultiChannel::GetRemoteName()
{
	return m_sMultiCastName;
}

#endif // !defined(AFX_MultiCHANNEL_H__6DE6311B_C987_4246_A61D_53ADA1F3D619__INCLUDED_)
