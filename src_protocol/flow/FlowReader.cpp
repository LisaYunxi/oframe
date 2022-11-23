#include "public.h"
#include "FlowReader.h"

CFlowReader::CFlowReader()
{
	m_pFlow = NULL;
	m_pFlow = 0;
	m_nNextId = UF_INT8_ZERO;
	m_nBusinessNo = UF_INT8_MAX;
}

CFlowReader::~CFlowReader()
{
	DetachFlow();
}

int CFlowReader::AttachFlow(CReadFlow *pFlow, UF_INT8 nStartId, UF_INT8 nBusinessNo, int nTimes)
{
	m_pFlow = pFlow;
	SetId(nStartId);
	m_nTimes = nTimes;
	m_nBusinessNo = nBusinessNo;
	return 0;
}

CReadFlow *CFlowReader::DetachFlow()
{
	CReadFlow *pFlow = m_pFlow;
	m_pFlow = NULL;
	return pFlow;
}

bool CFlowReader::GetNext(CPackage *pPackage)
{
	int len = GetNextBase(pPackage);
	if (len < 0)
		return false;
	pPackage->ValidPackage();
	return true;
}

int CFlowReader::GetNextBase(CPackage *pPackage)
{
	if (m_pFlow == NULL)
		return -1;

	int len = -1;
	if (m_pFlow->GetFlowType() == FLOW_TYPE_ONETIMEFLOW)
	{
		void *pData = GetNext(len);
		if (pData != NULL)
			pPackage->SetData((char *)pData, len);
		else
			return -1;
	}
	else
	{
		pPackage->AllocateToRead();
		len = GetNext(pPackage->Address(), pPackage->Length());
		if (len >= 0)
			pPackage->Truncate(len);
		else
			return -1;
	}
	return len;
}

int CFlowReader::GetNext(void *pObject, int length)
{
	if (m_nNextId >= m_pFlow->GetCount())
		return -1;

	int len = m_pFlow->Get(m_nNextId, pObject, length);

	//如果m_nNextId小于流大小却无法获取数据，说明指定的nextId太小，可以增加
	//不然有可能永远卡在这里
	//if (len >= 0)
	//{
	//SetId(1, FROM_HERE);
	//}
	SetId(m_nTimes, FROM_HERE);
	return len;
}

void* CFlowReader::GetNext(int& length)
{
	if (m_nNextId >= m_pFlow->GetCount())
	{
		length = -1;
		return NULL;
	}
	void *pData = m_pFlow->GetPointer(m_nNextId, length);
	SetId(m_nTimes, FROM_HERE);
	return pData;
}

void CFlowReader::SetId(UF_INT8 id, SeekFlag from)
{
	switch (from)
	{
	case FROM_HEAD:
	{
		m_nNextId = id;
		break;
	}
	case FROM_HERE:
	{
		m_nNextId += id;
		break;
	}
	case FROM_END:
	{
		m_nNextId = m_pFlow->GetCount() + id;
		break;
	}
	default:
	{
		EMERGENCY_EXIT("Invalid from parameter while calling setId of CFlowReader");
	}
	}
}
