// PublishStub.h: interface for the CPublishStub class.
//
// 20070905 xuzh ������CancelPublish����
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBLISHSTUB_H__BD24DD44_B6E3_4E21_B423_D800FC93AC9E__INCLUDED_)
#define AFX_PUBLISHSTUB_H__BD24DD44_B6E3_4E21_B423_D800FC93AC9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FibPublishApi.h"
#include "XTPPubSession.h"
#include "HashMap.h"

class CPublishStub : public CFibPublishApi, public CXTPFlowManager   
{
public:
	CPublishStub(CReactor *pReactor, DWORD nNodeID, char *pszAddress);

	/** ��ȡ���ӿڹ����������ַ
	*@return ���ӿڹ����������ַ
	*/
	virtual const char *GetAddress();

	/**����һ����Ϣ
	* @return ����������Ϣ
	*/	
	virtual CXTPPackage *CreateMessage();

	//-------------------�������Ľӿ�-------------------------------
	/**ע����Ϣ����
	* @param pFlow Ҫע�����Ϣ����
	* @param nSubjectNo Ҫ����������
	*/
	virtual void RegisterFlow(CReadOnlyFlow *pFlow, DWORD nSubjectNo);

	/**ȡ����������
	* @param nSubjectNo Ҫȡ������������
	*/
	virtual void CancelPublish(DWORD nSubjectNo);

	/**���������ȡһ��ע�����
	* @param nSubjectNo ����
	* @return ��ȡ������
	*/
	virtual CReadOnlyFlow *GetFlow(DWORD nSubjectNo);

	/**ɾ�����ӿ�
	*/
	virtual void Release();

private:
	virtual ~CPublishStub();
	inline DWORD GetNodeID();
private:
	typedef CHashMap<DWORD, CReadOnlyFlow *, HashInt> CFlowMap;
	CFlowMap m_mapFlow;
	CXTPPubSession *m_pSession;
	DWORD m_nNodeID;
	char m_szAddress[100];
};

inline DWORD CPublishStub::GetNodeID()
{
	return m_nNodeID;
}

#endif // !defined(AFX_PUBLISHSTUB_H__BD24DD44_B6E3_4E21_B423_D800FC93AC9E__INCLUDED_)
