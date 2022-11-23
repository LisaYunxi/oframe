#ifndef _FTDCSUBENDPOINT_H__
#define _FTDCSUBENDPOINT_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Protocol.h"
#include "FTDCSubscriber.h"

class CFTDCSubEndPoint
{
public:
	CFTDCSubEndPoint(CFTDCSubscriber *pSubscriber);
	virtual ~CFTDCSubEndPoint();

	int HandlePackage(CFTDCPackage *pFTDCPackage);
	inline unsigned short GetSequenceSeries();
private:
	CFTDCSubscriber *m_pSubscriber;
};

inline unsigned short CFTDCSubEndPoint::GetSequenceSeries()
{
	return m_pSubscriber->GetSequenceSeries();
}


#endif
