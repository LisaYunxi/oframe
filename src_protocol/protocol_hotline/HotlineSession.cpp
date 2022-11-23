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
	//这里面临订阅号和流水内部号码的匹配问题
	//备系统需要的流水号居然比主系统大，这不可能
	//备系统需要请求最新的内存的流水，第一为了速度，第二防止过小订阅导致找不到文件core
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
	//任一ChannelProtocol空闲，就可以发送
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

