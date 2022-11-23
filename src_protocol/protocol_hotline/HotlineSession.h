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

	/**获取读、写socket，供select使用
	* @param pReadId   输出参数，返回读所使用的socket，为0表示不需要读
	* @param pWriteId  输出参数，返回写所使用的socket，为0表示不需要写
	* @remark 在这里将发布的主题的数据包传递到最下层协议
	*/
	virtual void GetIds(int *pReadId, int *pWriteId);
private:
	CHotlineProtocol *m_pHotlineProtocol;
	CHotlinePackage m_prePackage;
	CHotlinePackageHandler *m_pHotlinePackageHandler;
};

#endif