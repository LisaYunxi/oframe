#ifndef AFX_COMPFLOWFACTORY_H__
#define AFX_COMPFLOWFACTORY_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "Flow.h"
#include "CachedFlow.h"
#include "IndexFlow.h"
#include "HashMap.h"

/**CCompFlowFactory管理排队服务所有的发布和订阅的流。
* 流共分为2类：
*		1.确认流(Series)，已经排好队并持久化的请求序列
*		2.未确认流(PreSeries)，建立在确认流之上，比确认流多的包未确认
*/
class CCompFlowFactory 
{
public:
	CCompFlowFactory(CCachedFlow *pPreSeries);

	virtual ~CCompFlowFactory();

	//取未确认流
	inline CCachedFlow *GetPreSeries();

	//获取日志流
	//inline CFlow *GetSeries();

	//将未确认的报文从所有的流中删除
	//void Rollback();

private:
//	CFlow *m_pSeries;							/**< 确认流 */
	CCachedFlow *m_pPreSeries;					/**< 未确认流 */
};

inline CCachedFlow *CCompFlowFactory::GetPreSeries()
{
	return m_pPreSeries;
}

//inline CFlow *CCompFlowFactory::GetSeries()
//{
//	return m_pSeries;
//}

#endif
