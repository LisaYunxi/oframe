#ifndef _FTDCPROTOCOL_H__
#define _FTDCPROTOCOL_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Protocol.h"
#include "FTDCPackage.h"
#include "HashMap.h"
#include "FTDCPubEndPoint.h"
#include "FTDCSubEndPoint.h"

#define MSG_FTDCERR_BADPACKAGE		0xA001

//协议栈容器的第三层协议，主要对FTD包的处理
class  CFTDCProtocol : public CProtocol
{
public:
	CFTDCProtocol(CReactor *pReactor);
	virtual ~CFTDCProtocol();

	virtual int Pop(CPackage *pPackage);
	void PublishSend();

	virtual int OnRecvErrorPackage(CPackage *pPackage);

	void Publish(CReadFlow *pFlow, unsigned short nSequenceSeries, UF_INT8 nStartId, UF_INT8 nBusinessNo = UF_INT8_MAX, int nReaderTimes = 1);
	void UnPublish(unsigned short nSequenceSeries);
	void UnPublishAll();
	void RegisterSubscriber(CFTDCSubscriber *pSubscriber);
	void UnRegisterSubscriber(CFTDCSubscriber *pSubscriber);

	void Clear();

private:
	CFTDCSubEndPoint *GetSubEndPoint(unsigned short nSequenceSeries);
	CFTDCPubEndPoint *GetPubEndPoint(unsigned short nSequenceSeries);

private:
	typedef CHashMap<unsigned short, CFTDCSubEndPoint *, HashInt> CSubEndPointMap;
	CSubEndPointMap m_mapSubEndPoint;

	typedef CHashMap<unsigned short, CFTDCPubEndPoint *, HashInt> CPubEndPointMap;
	CPubEndPointMap m_mapPubEndPoint;
};

#endif