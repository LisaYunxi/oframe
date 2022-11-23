// FIBSubEndPoint.h: interface for the CFIBSubEndPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBSUBENDPOINT_H__27554866_860A_4566_B4C9_674C9F0FBF02__INCLUDED_)
#define AFX_FIBSUBENDPOINT_H__27554866_860A_4566_B4C9_674C9F0FBF02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Protocol.h"
#include "FIBSubscriber.h"

class CFIBSubEndPoint
{
public:
	CFIBSubEndPoint(CFIBSubscriber *pSubscriber);
	virtual ~CFIBSubEndPoint();

	int HandlePackage(CXTPPackage *pFIBPackage);
	inline WORD GetSequenceSeries();
	inline void SetCommPhaseNo(DWORD wCommPhaseNo);
private:
	CFIBSubscriber *m_pSubscriber;
};

inline WORD CFIBSubEndPoint::GetSequenceSeries()
{
	return m_pSubscriber->GetSequenceSeries();
}

inline void CFIBSubEndPoint::SetCommPhaseNo(DWORD wCommPhaseNo)
{
	m_pSubscriber->SetCommPhaseNo(wCommPhaseNo);
}

#endif
