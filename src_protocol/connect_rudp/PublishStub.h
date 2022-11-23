// PublishStub.h: interface for the CPublishStub class.
//
// 20070905 xuzh 增加了CancelPublish方法
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

	/** 获取本接口关联的网络地址
	*@return 本接口关联的网络地址
	*/
	virtual const char *GetAddress();

	/**创建一个消息
	* @return 创建出的消息
	*/	
	virtual CXTPPackage *CreateMessage();

	//-------------------发布订阅接口-------------------------------
	/**注册消息队列
	* @param pFlow 要注册的消息队列
	* @param nSubjectNo 要发布的主题
	*/
	virtual void RegisterFlow(CReadOnlyFlow *pFlow, DWORD nSubjectNo);

	/**取消发布主题
	* @param nSubjectNo 要取消发布的主题
	*/
	virtual void CancelPublish(DWORD nSubjectNo);

	/**根据主题获取一个注册的流
	* @param nSubjectNo 主题
	* @return 获取到的流
	*/
	virtual CReadOnlyFlow *GetFlow(DWORD nSubjectNo);

	/**删除本接口
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
