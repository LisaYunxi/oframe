#ifndef AFX_HOTLINEPROTOCOL_H__
#define AFX_HOTLINEPROTOCOL_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Protocol.h"
#include "FlowReader.h"
#include "HotlinePackage.h"
#include "XTPPackage.h"

class CHotlineProtocol : public CProtocol, public CFlowReader 
{
public:
	CHotlineProtocol(CReactor *pReactor);
	virtual ~CHotlineProtocol();
	int PublishSend();
private:
	CHotlinePackage m_readerPackage;
};

#endif
