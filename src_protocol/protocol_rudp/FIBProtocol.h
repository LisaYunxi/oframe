// FIBProtocl.h: interface for the CFIBProtocl class .
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBPROTOCOL_H__C5C04921_6D0B_11D7_BB64_444553540000__INCLUDED_)
#define AFX_FIBPROTOCOL_H__C5C04921_6D0B_11D7_BB64_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Protocol.h"
#include "XTPPackage.h"
#include "HashMap.h"
#include "FIBPubEndPoint.h"
#include "FIBSubEndPoint.h"

#define MSG_FIBERR_BADPACKAGE		0xA001

/**协议栈容器的第第三层协议，主要对FIB包的处理
*/
class  CFIBProtocol : public CProtocol  
{
public:
	/**构造函数
	* @param pReactor 指向运行引擎的指针
	* @param pBelow    下层协议的指针
	* @param nActiveId 对下层协议而言，本层协议的协议Id
	*/
	CFIBProtocol(CReactor *pReactor);
	
	/**析构函数
	*/
	virtual ~CFIBProtocol();

	virtual int Pop(CPackage *pPackage);
	
	/**将各主题的报文发送出去
	*/
	void PublishSend();

	/**收到一个错误的数据包时调用
	* @param pPackage 错误的数据包
	* @return 0   成功
	* @return <>0 失败
	*/
	virtual int OnRecvErrorPackage(CPackage *pPackage);

	void Publish(CReadOnlyFlow *pFlow, WORD nSequenceSeries, int nStartId);
	void UnPublish(WORD nSequenceSeries);
	void RegisterSubscriber(CFIBSubscriber *pSubscriber);
	void UnRegisterSubscriber(CFIBSubscriber *pSubscriber);
	void SetCommPhaseNo(DWORD wCommPhaseNo);

	void Clear();
	
private:
	CFIBSubEndPoint *GetSubEndPoint(WORD nSequenceSeries);
	CFIBPubEndPoint *GetPubEndPoint(WORD nSequenceSeries);

private:
	typedef CHashMap<WORD, CFIBSubEndPoint *, HashInt> CSubEndPointMap;
	CSubEndPointMap m_mapSubEndPoint;

	typedef CHashMap<WORD, CFIBPubEndPoint *, HashInt> CPubEndPointMap;
	CPubEndPointMap m_mapPubEndPoint;
};

#endif
