#ifndef _XTPPROTOCOL_H__
#define _XTPPROTOCOL_H__

#if _MSC_VER > 1000
#pragma once
#endif 


#include "Protocol.h"
#include "XTPPackage.h"
#include "HashMap.h"
#include "XTPPubEndPoint.h"
#include "XTPSubEndPoint.h"

#define MSG_XTPERR_BADPACKAGE		0x3001

/**协议栈容器的第第三层协议，主要对XTP包的处理
*/
class  CXTPProtocol : public CProtocol  
{
public:
	/**构造函数
	* @param pReactor 指向运行引擎的指针
	* @param pPubHelper    下层协议的指针
	* @param nActiveId 对下层协议而言，本层协议的协议Id
	*/
	CXTPProtocol(CReactor *pReactor, CXTPPubHelper *pPubHelper, CXTPSubHelper *pSubHelper);
	
	/**析构函数
	*/
	virtual ~CXTPProtocol();

	/**将包传送到合适的上层协议
	* @param pPackage 需要上传的包的指针
	* @return 0   成功
	* @return <>0 失败
	*/
	virtual int Pop(CPackage *pPackage);
	
	/**处理定时操作，由CReactor调用
	* @param nIDEvent 定时器事件标识   
	*/
	virtual void OnTimer(int nIDEvent);

	/**收到一个错误的数据包时调用
	* @param pPackage 错误的数据包
	* @return 0   成功
	* @return <>0 失败
	*/
	virtual int OnRecvErrorPackage(CPackage *pPackage);

	void OnHeartBeat(unsigned int nSubjectNo, UF_INT8 nSeqNo);
	
	void Publish(CReadFlow *pFlow, unsigned int nSubjectNo, unsigned int dwSubscriberID, UF_INT8 nStartId);
	void UnPublish(unsigned int nSubjectNo);
	void RegisterSubscriber(CSubscriber *pSubscriber, unsigned int dwSubscriberID);
	void UnRegisterSubscriber(CSubscriber *pSubscriber);
	void OnAck(unsigned int dwSubscriberID, unsigned int dwSubjectNo, UF_INT8 nStartId);
	
	/**此处主要功能是由核心线程触发一次发布动作
	* @param pReadId   输出参数，返回读所使用的socket，为0表示不需要读
	* @param pWriteId  输出参数，返回写所使用的socket，为0表示不需要写 
	*/
	virtual void GetIds(int *pReadId, int *pWriteId);
protected:
	
private:
	CXTPSubEndPoint *GetSubEndPoint(unsigned int nSubjectNo);
	CXTPPubEndPoint *GetPubEndPoint(unsigned int nSubjectNo);

private:
	typedef CHashMap<unsigned int, CXTPSubEndPoint *, HashInt> CSubEndPointMap;
	CSubEndPointMap m_mapSubEndPoint;

	typedef CHashMap<unsigned int, CXTPPubEndPoint *, HashInt> CPubEndPointMap;
	CPubEndPointMap m_mapPubEndPoint;

	CXTPPubHelper *m_pPubHelper;
	CXTPSubHelper *m_pSubHelper;
};

#endif
