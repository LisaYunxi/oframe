#include "public.h"
#include "Protocol.h"

CProtocol::CProtocol(CReactor *pReactor, int nHdrReserveLen, CPackage *pDemuxPackage, CPackage *pSendPackage)
	:CEventHandler(pReactor), m_pErrorHandler(NULL)
{
	m_nActiveID = 0;
	m_pUpper = NULL;
	m_pBrother = NULL;
	m_pDemuxPackage = pDemuxPackage;
	m_pSendPackage = pSendPackage;
	m_nHdrReserveLen = nHdrReserveLen;
	m_pUpperHandler = NULL;
}

CProtocol::~CProtocol()
{
	while (m_lowers.size() > 0)
	{
		DetachLower(m_lowers[0]);
	}

	if (m_pDemuxPackage != NULL)
	{
		m_pDemuxPackage->Release();
	}

	if (m_pSendPackage != NULL)
	{
		m_pSendPackage->Release();
	}
}

void CProtocol::AttachLower(CProtocol *pLower, unsigned int nActiveID)
{
	m_nActiveID = nActiveID;
	m_nHdrReserveLen += pLower->GetHdrReserveLen();
	CProtocolArray::iterator i = find(m_lowers.begin(), m_lowers.end(), pLower);
	if (i == m_lowers.end())
	{
		m_lowers.push_back(pLower);
		pLower->AddUpper(this);
	}
}

void CProtocol::DetachLower(CProtocol *pLower)
{
	CProtocolArray::iterator i = find(m_lowers.begin(), m_lowers.end(), pLower);
	if (i != m_lowers.end())
	{
		m_lowers.erase(i);
		pLower->RemoveUpper(GetActiveID());
	}
}

int CProtocol::HandlePackage(CPackage *pPackage, CProtocol *pProtocol)
{
	int ret = 0;
	while (pPackage->Length() > 0)
	{
		m_pDemuxPackage->BufAddRef(pPackage);
		int len = m_pDemuxPackage->ValidPackage();

		if (len >= 0)
		{
			//完整数据包				
			ret = Pop(m_pDemuxPackage);
			if (ret < 0)
			{
				//有错误		
				break;
			}
			pPackage->Pop(len);
		}
		else if (len == -1)
		{
			//部分数据包
			break;
		}
		else
		{
			//错误数据包
			OnRecvErrorPackage(pPackage);
			ret = len;
			break;
		}
		m_pDemuxPackage->BufRelease();
	}
	return ret;
}

int CProtocol::Pop(CPackage *pPackage)
{
	unsigned int nActiveID = pPackage->GetActiveID();
	CProtocolCallback *pUpper = GetUpper(nActiveID);

	if (pUpper != NULL)
	{
		return pUpper->HandlePackage(pPackage, this);
	}

	if (m_pUpperHandler != NULL)
	{
		return m_pUpperHandler->HandlePackage(pPackage, this);
	}

	return 0;
}

void CProtocol::AddUpper(CProtocol *pUpper)
{
	//	pUpper->m_pBrother = m_pUpper;
	//	m_pUpper = pUpper;
	if (m_pUpper == NULL)
	{
		m_pUpper = pUpper;
	}
	else
	{
		CProtocol *pCurr = m_pUpper;
		for (; pCurr != NULL; pCurr = pCurr->m_pBrother)
		{
			if (pCurr->m_pBrother == NULL)
			{
				pCurr->m_pBrother = pUpper;
				break;
			}
		}
	}
}

CProtocol *CProtocol::RemoveUpper(unsigned int nActiveID)
{
	CProtocol *pPrev = NULL;
	CProtocol *pCurr = m_pUpper;
	while (pCurr != NULL)
	{
		if (pCurr->GetActiveID() == nActiveID)
		{
			if (pPrev == NULL)
			{
				m_pUpper = pCurr->m_pBrother;
			}
			else
			{
				pPrev->m_pBrother = pCurr->m_pBrother;
			}
			break;
		}
		pPrev = pCurr;
		pCurr = pCurr->m_pBrother;
	}
	return pCurr;
}

int CProtocol::OnRecvErrorPackage(CPackage *pPackage)
{
	return 0;
}

int CProtocol::MakePackage(CPackage *pPackage, CProtocol *pUpper)
{
	return pPackage->MakePackage();
}

int CProtocol::Send(CPackage *pPackage, CProtocol *pUpper)
{
	int ret = MakePackage(pPackage, pUpper);
	if (ret < 0)
	{
		return ret;
	}
	CProtocolArray::iterator i = m_lowers.begin();
	for (; i != m_lowers.end(); i++)
	{
		(*i)->Push(pPackage, this);
	}
	return 0;
}

int CProtocol::Push(CPackage *pPackage, CProtocol *pUpper)
{
	m_pSendPackage->BufAddRef(pPackage);
	int ret = Send(m_pSendPackage, pUpper);
	m_pSendPackage->BufRelease();
	return ret;
}

