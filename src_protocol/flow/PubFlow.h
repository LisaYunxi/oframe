#ifndef _ResultPubFlow_H__
#define _ResultPubFlow_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Flow.h"

class CPubFlow : public CFlow
{
public:
	CPubFlow(CFlow *pFlow);
	virtual ~CPubFlow();

	virtual UF_INT8 GetCount();
	virtual int Get(UF_INT8 id, void *pObject, int length);

	//排队机确认消息已经落地
	//@param nKey 要确认的排队机流序列号
	void ConfirmQueued(UF_INT8 nKey);

	//撮合确认输入流序号与该输入产生的最大结果流序号
	//@param iSeriesId 排队机series流消息序号
	//@param iResultId 撮合结合tresult流消息序号
	//@return 获取到未确认结果流
	bool SetSIDAndRID(UF_INT8 iSeriesId,UF_INT8 iResultId);

	virtual int GetFlowType();

private:
	volatile UF_INT8 m_iAckSeriesId;
	volatile UF_INT8 m_iAckResultId;
	volatile UF_INT8 m_iLastNoAckSeriesId;
	volatile UF_INT8 m_iLastNoAckResultId;
};

#endif