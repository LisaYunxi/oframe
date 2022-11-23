#ifndef AFX_FLOWMANAGER_H__
#define AFX_FLOWMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "HashMap.h"
#include "CachedFlow.h"
#include "Package.h"
#include "Config.h"
#include "IndexFlow.h"

typedef map<string, CIndexFlow *> CIndexFlowMap;
typedef map<string, CIndexFlow *> CUserFlowMap;

struct TFlowInfo
{
	CFlow *pFlow;
	unsigned int TopicID;
	CIndexFlowMap m_mapIndexFlow;
	CUserFlowMap m_mapUserFlow;
};

class CFlowManager
{
public:
	CFlowManager();
	virtual ~CFlowManager();

	CFlow *GetFlow(const int nSubjectNo);
	TFlowInfo* GetFlowInfo(const int nSubjectNo);
	virtual bool RemoveTopic(int nSubjectNo);
	virtual UF_INT8 PulishToTopic(CPackage *pPackage, int nSubjectNo);
	CIndexFlow* GetIndexFlow(int nSubjectNo, char* SubIndexID);
//	CIndexFlow* GetUserFlow(int nSubjectNo, char* UserID);
	virtual CFlow* RegisterTopic(int nSubjectNo, const char* pFlowName = NULL, CFlow* pFlow = NULL);

private:
	typedef CHashMap<int, TFlowInfo, HashInt> CFlowInfoMap;
	CFlowInfoMap m_mapFlowInfo;

	int m_nFlowCacheSize;		//流水内存的大小
	string m_strFlowPath;
};

#endif
