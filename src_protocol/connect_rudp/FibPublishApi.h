// FibPublishApi.h: interface for the CFibPublishApi class.
//
// 20070905 xuzh ������CancelPublish����
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBPUBLISHAPI_H__CA980195_08A4_4448_BD75_54E61FEE2B30__INCLUDED_)
#define AFX_FIBPUBLISHAPI_H__CA980195_08A4_4448_BD75_54E61FEE2B30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Reactor.h"
#include "XTPPackage.h"
#include "Subscriber.h"
#include "ReadOnlyFlow.h"

class CFibPublishApi
{
public:
	/** ��ȡ���ӿڹ����������ַ
	*@return ���ӿڹ����������ַ
	*/
	virtual const char *GetAddress() = 0;

	/**����һ����Ϣ
	* @return ����������Ϣ
	*/	
	virtual CXTPPackage *CreateMessage() = 0;

	//-------------------�������Ľӿ�-------------------------------
	/**ע����Ϣ����
	* @param pFlow Ҫע�����Ϣ����
	* @param nSubjectNo Ҫ����������
	*/
	virtual void RegisterFlow(CReadOnlyFlow *pFlow, DWORD nSubjectNo) = 0;

	/**ȡ����������
	* @param nSubjectNo Ҫȡ������������
	*/
	virtual void CancelPublish(DWORD nSubjectNo) = 0;

	/**ɾ�����ӿ�
	*/
	virtual void Release() = 0;

	/**�����ӿ�
	* @param pReactor ������Ӧ��
	* @param nNodeID  �ڵ��ʶ
	* @param pszAddress �����������ַ����ʽΪ"protocol://address:port"
	* @return �������Ľӿ�
	*/
	static CFibPublishApi *CreatePublishApi(CReactor *pReactor, DWORD nNodeID, char *pszAddress);

protected:
	virtual ~CFibPublishApi(){};
};

#endif // !defined(AFX_FIBPUBLISHAPI_H__CA980195_08A4_4448_BD75_54E61FEE2B30__INCLUDED_)
