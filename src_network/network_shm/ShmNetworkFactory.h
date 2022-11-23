// ShmNetworkFactory.h: interface for the CShmNetworkFactory class.
//
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ShmNETWORKFACTORY_H__9C863AA5_2C99_445B_B6FD_C1459913BB4F__INCLUDED_)
#define AFX_ShmNETWORKFACTORY_H__9C863AA5_2C99_445B_B6FD_C1459913BB4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetworkFactory.h"

class CShmNetworkFactory : public CNetworkFactory  
{
public:
	CShmNetworkFactory();
	virtual ~CShmNetworkFactory();

	/**����һ��Shm�������ˡ����Service Name��ƥ����������������೧��������
	* @param pName	��������
	* @return  �õ��ķ�������
	*/
	virtual CServerBase *CreateServer(CServiceName *pName);
	
	/**����һ��Shm�ͻ��ˡ����Service Name��ƥ����������������೧��������
	* @param	pName	Ҫ���ӵķ�������
	* @return   �õ��Ŀͻ���
	*/
	virtual CClientBase *CreateClient(CServiceName *pName);

private:
	static CShmNetworkFactory m_instance;
};

CShmNetworkFactory CShmNetworkFactory::m_instance;

#endif // !defined(AFX_ShmNETWORKFACTORY_H__9C863AA5_2C99_445B_B6FD_C1459913BB4F__INCLUDED_)
