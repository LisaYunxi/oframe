#if !defined(AFX_MultiNETWORKFACTORY_H__4EB0F1E7_5C35_43B0_B0B2_8160B72651D0__INCLUDED_)
#define AFX_MultiNETWORKFACTORY_H__4EB0F1E7_5C35_43B0_B0B2_8160B72651D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "NetworkFactory.h"

class CMultiNetworkFactory : public CNetworkFactory  
{
public:
	CMultiNetworkFactory();
	virtual ~CMultiNetworkFactory();
	
	/**����һ��TCP�������ˡ����Service Name��ƥ����������������೧��������
	* @param pName	��������
	* @return  �õ��ķ�������
	*/
	virtual CServerBase *CreateServer(CServiceName *pName);
	
	/**����һ��TCP�ͻ��ˡ����Service Name��ƥ����������������೧��������
	* @param	pName	Ҫ���ӵķ�������
	* @return   �õ��Ŀͻ���
	*/
	virtual CClientBase *CreateClient(CServiceName *pName);

//private:
	static CMultiNetworkFactory m_instance;
};

#endif // !defined(AFX_MultiNETWORKFACTORY_H__4EB0F1E7_5C35_43B0_B0B2_8160B72651D0__INCLUDED_)
