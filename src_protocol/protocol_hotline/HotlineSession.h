#ifndef AFX_HOTLINESESSION_H__
#define AFX_HOTLINESESSION_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "SessionAggregation.h"
#include "HotlineProtocol.h"

class CHotlineSession;

class CHotlinePackageHandler
{
public:
	virtual void HandleHotlinePackage(CHotlinePackage *pPackage, CHotlineSession *pSession) = 0;
};

class CHotlineSession : public CSessionAggregation 
{
public:
	CHotlineSession(CReactor *pReactor, unsigned int dwAggregationID,CHotlinePackageHandler *pHotlinePackageHandler);
	virtual ~CHotlineSession();
	void AttachFlow(CReadFlow *pFlow, UF_INT8 nStartId);
	void SendLoginReq(UF_INT8 nSequenceNo);
	void SendConfirmRsp(UF_INT8 nSequenceNo);
	virtual int HandlePackage(CPackage *pPackage, CProtocol *pProtocol);

	/**��ȡ����дsocket����selectʹ��
	* @param pReadId   ������������ض���ʹ�õ�socket��Ϊ0��ʾ����Ҫ��
	* @param pWriteId  �������������д��ʹ�õ�socket��Ϊ0��ʾ����Ҫд
	* @remark �����ｫ��������������ݰ����ݵ����²�Э��
	*/
	virtual void GetIds(int *pReadId, int *pWriteId);
private:
	CHotlineProtocol *m_pHotlineProtocol;
	CHotlinePackage m_prePackage;
	CHotlinePackageHandler *m_pHotlinePackageHandler;
};

#endif