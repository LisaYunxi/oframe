#include "public.h"
#include "HotlineSession.h"
#include "HotlinePackage.h"

CHotlineSession::CHotlineSession(CReactor *pReactor, unsigned int dwAggregationID, CHotlinePackageHandler *pHotlinePackageHandler)
	:CSessionAggregation(pReactor, dwAggregationID)
{
	m_pHotlinePackageHandler = pHotlinePackageHandler;

	m_pHotlineProtocol = new CHotlineProtocol(pReactor);
	m_pHotlineProtocol->AttachLower(m_pAggrProtocol, AGGR_TYPE_CARRY);
	m_pHotlineProtocol->RegisterUpperHandler(this);
	m_pHotlineProtocol->RegisterErrorHandler(this);

	m_prePackage.ConstructAllocate(HOTLINE_PACKAGE_MAX_SIZE, 256);
}

CHotlineSession::~CHotlineSession()
{
	delete m_pHotlineProtocol;
}

void CHotlineSession::AttachFlow(CReadFlow *pFlow, UF_INT8 nStartId)
{
	//�������ٶ��ĺź���ˮ�ڲ������ƥ������
	//��ϵͳ��Ҫ����ˮ�ž�Ȼ����ϵͳ���ⲻ����
	//��ϵͳ��Ҫ�������µ��ڴ����ˮ����һΪ���ٶȣ��ڶ���ֹ��С���ĵ����Ҳ����ļ�core
	if (nStartId >= pFlow->GetFirstID() && nStartId <= pFlow->GetCount())
	{
		m_pHotlineProtocol->AttachFlow(pFlow, nStartId);
	}
	else
	{
		m_prePackage.AllocateToAdd();
		m_prePackage.Truncate(0);
		m_prePackage.SetTid(HTID_LOGIN_ERROR);
		m_pHotlineProtocol->Send(&m_prePackage);
	}
}

void CHotlineSession::SendLoginReq(UF_INT8 nSequenceNo)
{
	m_prePackage.AllocateToAdd();
	m_prePackage.Truncate(0);
	m_prePackage.SetTid(HTID_LOGIN_REQ);
	m_prePackage.SetSequenceNo(nSequenceNo);
	m_pHotlineProtocol->Send(&m_prePackage);
}

void CHotlineSession::SendConfirmRsp(UF_INT8 nSequenceNo)
{
	m_prePackage.AllocateToAdd();
	m_prePackage.Truncate(0);
	m_prePackage.SetTid(HTID_CONFIRM_RSP);
	m_prePackage.SetSequenceNo(nSequenceNo);

	m_pHotlineProtocol->Send(&m_prePackage);
}

int CHotlineSession::HandlePackage(CPackage *pPackage, CProtocol *pProtocol)
{
	m_pHotlinePackageHandler->HandleHotlinePackage((CHotlinePackage *)pPackage, this);
	return 0;
}

void CHotlineSession::GetIds(int *pReadId, int *pWriteId)
{
	//��һChannelProtocol���У��Ϳ��Է���
	pReadId = 0;
	*pWriteId = 0;

	CAggrSessionMap::iterator i = m_mapSession.begin();
	for (; i != m_mapSession.end(); i++)
	{
		if (!(*i).second->IsChannelWriteBusy())
		{
			break;
		}
	}
	if (i != m_mapSession.end())
	{
		m_pHotlineProtocol->PublishSend();
	}
}

