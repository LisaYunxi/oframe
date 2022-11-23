// SubscribeStub.h: interface for the CSubscribeStub class.
//
// 20070905 xuzh 增加了UnSubscribe方法
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBSCRIBESTUB_H__CE014FB6_D01E_4127_B9BA_FFFFB746E6F1__INCLUDED_)
#define AFX_SUBSCRIBESTUB_H__CE014FB6_D01E_4127_B9BA_FFFFB746E6F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FibSubscribeApi.h"
#include "XTPPubSession.h"

class CSubscribeStub : public CFibSubscribeApi  
{
public:
	CSubscribeStub(CReactor *pReactor, DWORD nNodeID, char *pszAddress);
	
	/** 获取本接口关联的网络地址
	*@return 本接口关联的网络地址
	*/
	virtual const char *GetAddress();

	/*订阅一个主题
	* @param pSubscriber 订阅者
	* @param dwSubscriberID 订阅者代码
	*/
	virtual void Subscribe(CSubscriber *pSubscriber, DWORD dwSubscriberID);

	/*订阅一个主题
	* @param pSubscriber 订阅者
	*/
	virtual void UnSubscribe(CSubscriber *pSubscriber);

	/**设置所管理的订阅者的CPN
	* @param nCommPhaseNo 要设置的新CPN
	*/
	virtual void SetSubCommPhaseNo(WORD nCommPhaseNo);

	/**删除本接口
	*/
	virtual void Release();

protected:
	virtual ~CSubscribeStub();

private:
	CXTPPubSession *m_pSession;
	char m_szAddress[100];
};

#endif // !defined(AFX_SUBSCRIBESTUB_H__CE014FB6_D01E_4127_B9BA_FFFFB746E6F1__INCLUDED_)
