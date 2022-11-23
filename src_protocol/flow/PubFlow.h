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

	//�Ŷӻ�ȷ����Ϣ�Ѿ����
	//@param nKey Ҫȷ�ϵ��Ŷӻ������к�
	void ConfirmQueued(UF_INT8 nKey);

	//���ȷ���������������������������������
	//@param iSeriesId �Ŷӻ�series����Ϣ���
	//@param iResultId ��Ͻ��tresult����Ϣ���
	//@return ��ȡ��δȷ�Ͻ����
	bool SetSIDAndRID(UF_INT8 iSeriesId,UF_INT8 iResultId);

	virtual int GetFlowType();

private:
	volatile UF_INT8 m_iAckSeriesId;
	volatile UF_INT8 m_iAckResultId;
	volatile UF_INT8 m_iLastNoAckSeriesId;
	volatile UF_INT8 m_iLastNoAckResultId;
};

#endif