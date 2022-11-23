#include "public.h"
#include "FlowManager.h"
#include "FileFlow.h"
#include "CachedFlow.h"

#define INI_FILE_NAME "omq.ini"

CFlowManager::CFlowManager()
{
	m_strFlowPath = GetConfigString(INI_FILE_NAME, "FlowPath");
	m_nFlowCacheSize = 0x100000;
	int nFlowCacheSize = GetConfigInt(INI_FILE_NAME, "FlowCacheSize");
	if (nFlowCacheSize > 0)
	{
		m_nFlowCacheSize = nFlowCacheSize;
	}
}

CFlowManager::~CFlowManager()
{
	CFlowInfoMap::iterator itor = m_mapFlowInfo.Begin();
	while (!itor.IsEnd())
	{
		CIndexFlowMap::iterator indexitor = (*itor).m_mapIndexFlow.begin();
		while (indexitor != (*itor).m_mapIndexFlow.end())
		{
			delete indexitor->second;
			indexitor++;
		}
		(*itor).m_mapUserFlow.clear();

		CIndexFlowMap::iterator useritor = (*itor).m_mapUserFlow.begin();
		while (useritor != (*itor).m_mapUserFlow.end())
		{
			delete useritor->second;
			useritor++;
		}
		(*itor).m_mapUserFlow.clear();
		itor++;
	}
}

CFlow *CFlowManager::GetFlow(const int nSubjectNo)
{
	TFlowInfo *pFind = m_mapFlowInfo.Find(nSubjectNo);
	if (pFind == NULL)
	{
		if (nSubjectNo > 0)
			return RegisterTopic(nSubjectNo);
		else
			return NULL;
	}
	return pFind->pFlow;
}

TFlowInfo* CFlowManager::GetFlowInfo(const int nSubjectNo)
{
	TFlowInfo *pFind = m_mapFlowInfo.Find(nSubjectNo);
	if (pFind == NULL)
		return NULL;
	return pFind;
}

CFlow* CFlowManager::RegisterTopic(int nSubjectNo, const char* pFlowName, CFlow* pFlow)
{
	TFlowInfo *pFind = m_mapFlowInfo.Find(nSubjectNo);
	if (pFind != NULL)
	{
		PRINT_TO_STD("CFlowManager RegisterTopic Error: %d is already exsit!", nSubjectNo);
		return NULL;
	}

	char flowName[256];
	if (pFlowName == NULL)
		sprintf(flowName, "output_%d", nSubjectNo);
	else
		sprintf(flowName, "output_%d_%s", nSubjectNo, pFlowName);

	CFlow *pThisFlow = NULL;

	TFlowInfo FlowInfo;
	if (pFlow == NULL)
		pThisFlow = new CCachedFlow(true, flowName, m_nFlowCacheSize, 0x20000);
	else
		pThisFlow = pFlow;

	if (GetConfigString(INI_FILE_NAME, "PersistOtherFlows") == "yes")
	{
		CFlow* pFileFlow = new CFileFlow(flowName, m_strFlowPath.c_str(), false, GetConfigInt(INI_FILE_NAME, "PersistGap"));
		pThisFlow->AttachUnderFlow(pFileFlow);
	}

	FlowInfo.pFlow = pThisFlow;
	FlowInfo.TopicID = nSubjectNo;
	m_mapFlowInfo.Insert(nSubjectNo, FlowInfo);
	return FlowInfo.pFlow;
}

bool CFlowManager::RemoveTopic(int nSubjectNo)
{
	//Î´ÊµÏÖ
	return false;
}

UF_INT8 CFlowManager::PulishToTopic(CPackage *pPackage, int nSubjectNo)
{
	CFlow *pFlow = GetFlow(nSubjectNo);
	if (pFlow == NULL)
	{
		return -1;
	}
	pPackage->MakePackage();
	return pFlow->Append(pPackage->Address(), pPackage->Length());
}

CIndexFlow * CFlowManager::GetIndexFlow(int nSubjectNo, char* SubIndexID)
{
	CIndexFlow* pIndexFlow = NULL;
	TFlowInfo* pFlowInfo = GetFlowInfo(nSubjectNo);
	if (pFlowInfo == NULL)
		return NULL;

	CIndexFlowMap::iterator iter = pFlowInfo->m_mapIndexFlow.find(SubIndexID);
	if (iter == pFlowInfo->m_mapIndexFlow.end())
	{
		PRINT_TO_STD("AddIndexFlow: SubjectNo:%d, IndexID:%s, IndexFlowSize:%d", nSubjectNo, SubIndexID, pFlowInfo->m_mapIndexFlow.size());
		//pIndexFlow = new CIndexFlow(m_nFlowCacheSize);
		char IndexName[128];
		sprintf(IndexName, "Index_%d_%s", nSubjectNo, SubIndexID);
		pIndexFlow = new CIndexFlow(IndexName);
		pIndexFlow->AttachUnderFlow(pFlowInfo->pFlow);
		pFlowInfo->m_mapIndexFlow.insert(CIndexFlowMap::value_type(SubIndexID, pIndexFlow));
	}
	else
	{
		pIndexFlow = iter->second;
	}
	return pIndexFlow;
}

//CIndexFlow * CFlowManager::GetUserFlow(int nSubjectNo, char* UserID)
//{
//	CIndexFlow* pIndexFlow = NULL;
//	TFlowInfo* pFlowInfo = GetFlowInfo(nSubjectNo);
//	if (pFlowInfo == NULL)
//		return NULL;
//
//	CIndexFlowMap::iterator iter = pFlowInfo->m_mapUserFlow.find(UserID);
//	if (iter == pFlowInfo->m_mapUserFlow.end())
//	{
//		PRINT_TO_STD("AddXUserFlow: SubjectNo:%d, XUserID:%s, UserFlowSize:%d", nSubjectNo, UserID, pFlowInfo->m_mapUserFlow.size());
//		//pIndexFlow = new CIndexFlow(m_nFlowCacheSize);
//		char IndexName[128];
//		sprintf(IndexName, "User_%d_%s", nSubjectNo, UserID);
//		pIndexFlow = new CIndexFlow(IndexName);
//		pIndexFlow->AttachUnderFlow(pFlowInfo->pFlow);
//		pFlowInfo->m_mapUserFlow.insert(CUserFlowMap::value_type(UserID, pIndexFlow));
//	}
//	else
//	{
//		pIndexFlow = iter->second;
//	}
//	return pIndexFlow;
//}