// FIBSubEndPoint.cpp: implementation of the CFIBSubEndPoint class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "FIBSubEndPoint.h"
#include "XTPPackage.h"
#include "BaseXtpData.h"

const DWORD NAK_TIMEOUT	= 100;			//ÿһ��NAK��NAK_TIMEOUT����󷢳����Ժ�ʱ�ӱ�
const DWORD RECV_HEARTBEAT_TIMEOUT	= 2000;	//����������ʱʱ�䣬��λΪ����

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFIBSubEndPoint::CFIBSubEndPoint(CFIBSubscriber *pSubscriber)
{
	m_pSubscriber = pSubscriber;
}

CFIBSubEndPoint::~CFIBSubEndPoint()
{
}

int CFIBSubEndPoint::HandlePackage(CXTPPackage *pFIBPackage)
{
	if (TID_NtfCommPhaseChange == pFIBPackage->GetTid())
	{
		CCommPhaseField CommPhaseField;
		XTP_GET_SINGLE_FIELD(pFIBPackage,&CommPhaseField);
		SetCommPhaseNo(CommPhaseField.CommPhaseNo);
		m_pSubscriber->HandleMessage(pFIBPackage);
		return 0;
	}

	if (pFIBPackage->GetXTPHeader()->SequenceNo != m_pSubscriber->GetReceivedCount()+1)
	{	
		return -1;
	}
	m_pSubscriber->HandleMessage(pFIBPackage);
	return 0;
}
