#include "public.h"
#include "FileFlow.h"
#include "monitorIndex.h"
#include "BaseFunction.h"

//#define BLOCK_SIZE	100
struct TFlowId
{
	unsigned int dwSize;                       //���󳤶�
	void ChangeEndian()
	{
		CHANGE_ENDIAN(dwSize);
	}
};

CFileFlow::CFileFlow(const char *pszFlowName, const char *pszPath, bool bReUse, int nFlowGap)
{
	m_nLongFileFlowGap = nFlowGap;
	m_nCurrIndex = 0;
	m_nFirstIndex = INT_MAX;
	m_pEventHandler = NULL;

	memset(m_sPreFileName, 0, sizeof(m_sPreFileName));
	sprintf(m_sPreFileName, "%s%s", pszPath, pszFlowName, '*');
	m_sName = pszFlowName;

	char findFileName[MAX_PATH_LEN + 1];
	sprintf(findFileName, "%s.*.flow", m_sPreFileName);
	vector<string> flowNames = findNames(findFileName);
	UF_INT8 firstBeginID = UF_INT8_MAX;
	UF_INT8 secondBeginID = UF_INT8_MAX;

	if (flowNames.size() > 0)
	{
		vector<string>::iterator iter = flowNames.begin();
		while (iter != flowNames.end())
		{
			string eachName = *iter;
			//�������Ҫ�ظ�ʹ��ɾ�����е��ļ�
			if (!bReUse)
			{
				remove(eachName.c_str());
				iter++;
				continue;
			}
			vector<string> singlename = Txtsplit(eachName, ".");
			//int nFlowIndex = atoi(singlename[singlename.size()-3].c_str());
			UF_INT8 thisBeginID = atof(singlename[singlename.size() - 2].c_str());
			//�ҵ���С�ĺʹ�С�ļ����������Ӧ�ú�nFlowGap���
			//������ȵĻ���˵��֮ǰ�����ļ��ļ�������еļ����һ�£�����ʹ�ã�ֱ���˳�
			if (thisBeginID < firstBeginID)
			{
				secondBeginID = firstBeginID;
				firstBeginID = thisBeginID;
			}
			else
			{
				if (thisBeginID < secondBeginID)
				{
					secondBeginID = thisBeginID;
				}
			}

			//UF_INT8 thisBeginID = _atoi64(singlename[singlename.size()-2].c_str());
			int nFlowIndex = thisBeginID / nFlowGap;
			if (nFlowIndex > m_nCurrIndex)
			{
				m_nCurrIndex = nFlowIndex;
			}
			if (m_nFirstIndex > nFlowIndex)
			{
				m_nFirstIndex = nFlowIndex;
			}
			SFlowFileInfo *pFlowFileInfo = new SFlowFileInfo();
			pFlowFileInfo->FileName = eachName;
			//pFlowFileInfo->BeginID = pFlowFileInfo->BeginID*nFlowIndex*m_nLongFileFlowGap;
			pFlowFileInfo->BeginID = thisBeginID;
			pFlowFileInfo->pContentFile = NULL;
			m_flowIndex[nFlowIndex] = pFlowFileInfo;
			iter++;
		}
	}

	//���û�������ļ�����ʼ��Ϊ0
	if (m_flowIndex.size() == 0)
	{
		SFlowFileInfo *pFlowFileInfo = new SFlowFileInfo();
		pFlowFileInfo->FileName = m_sPreFileName;
		pFlowFileInfo->FileName += ".0.flow";
		pFlowFileInfo->BeginID = UF_INT8_ZERO;
		pFlowFileInfo->pContentFile = NULL;
		m_flowIndex[0] = pFlowFileInfo;
		m_nFirstIndex = 0;
	}

	if (!(firstBeginID == UF_INT8_MAX || secondBeginID == UF_INT8_MAX))
	{
		int realGap = secondBeginID - firstBeginID;
		if (nFlowGap != realGap)
		{
			printf("%s:Config Flow Gap[%d]!=Flow File Gap[%d]! Please change config or delete flow flies, exit! \n", m_sPreFileName, nFlowGap, realGap);
			exit(-1);
		}
	}

	m_nFirstID = m_flowIndex[m_nFirstIndex]->BeginID;

	//�����һ���ļ�
	OpenFile(m_nCurrIndex);
	fpos_t offset = GetPos(m_nCurrIndex);
	FPOS_SET(m_nCurrOffset, FPOS_GET(offset));
	m_nCount = GetIndexMaxCount(m_nCurrIndex);

	//�����һ���ļ��ĵ�һ����ʼ��
	m_nReadId = m_flowIndex[m_nCurrIndex]->BeginID - 1;
	FPOS_SET(m_nReadOffset, 0);
	m_nReadIndex = m_nCurrIndex;

	//INIT_CRITICAL_VAR(m_criticalVar);
}

CFileFlow::~CFileFlow()
{
	//DELETE_CRITICAL_VAR(m_criticalVar);
	VFileInfoMap::iterator iter = m_flowIndex.begin();
	while (iter != m_flowIndex.end())
	{
		CloseFile(iter->second);
		iter++;
	}
}

UF_INT8 CFileFlow::Append(void *pObject, int length)
{
	// 2014-07-01 �����ٽ��������ٽ����ı߽������ƶ�
//	ENTER_CRITICAL(m_criticalVar);
	m_RWLock.WriteLock();
	int thisIndexID = m_nCount / m_nLongFileFlowGap;
	if (thisIndexID > m_nCurrIndex)
	{
		//�����µ��ļ�
		OpenFile(thisIndexID);
		//�ر����ļ�
		CloseFile(m_flowIndex[m_nCurrIndex]);

		m_nCurrIndex = thisIndexID;
		FPOS_SET(m_nCurrOffset, 0);
	}

	TFlowId flowId;
	flowId.dwSize = length;
	flowId.ChangeEndian();

	fsetpos(m_flowIndex[m_nCurrIndex]->pContentFile, &m_nCurrOffset);

	if (fwrite(&flowId, sizeof(flowId), 1, m_flowIndex[m_nCurrIndex]->pContentFile) != 1)
	{
		sprintf(m_sError, "%s:Can not write flowid for CFlow", m_sPreFileName);
		EMERGENCY_EXIT(m_sError);
	}

	if (fwrite(pObject, 1, length, m_flowIndex[m_nCurrIndex]->pContentFile) != (size_t)length)
	{
		sprintf(m_sError, "%s:Can not write content file for CFlow", m_sPreFileName);
		EMERGENCY_EXIT(m_sError);
	}

	fflush(m_flowIndex[m_nCurrIndex]->pContentFile);

	FPOS_SET(m_nCurrOffset, FPOS_GET(m_nCurrOffset) + length + sizeof(flowId));
	m_nCount++;

	//LEAVE_CRITICAL(m_criticalVar);
	m_RWLock.UnLock();
	if (m_pEventHandler != NULL)
	{
		m_pEventHandler->Notify();
	}

	return m_nCount - 1;
}

int CFileFlow::Get(UF_INT8 id, void *pObject, int length)
{
	//ENTER_CRITICAL(m_criticalVar);
	m_RWLock.ReadLock();
	fpos_t offset;
	//�ж��Ƿ��ڱ��׶�����
	int thisIndexID = id / m_nLongFileFlowGap;
	if (thisIndexID != m_nReadIndex)
	{
		if (m_flowIndex[thisIndexID] == NULL || m_flowIndex[thisIndexID]->pContentFile == NULL)
		{
			OpenFile(thisIndexID);
		}
		offset = GetPos(thisIndexID, id);
	}
	else
	{
		if (m_nReadId + 1 == id)
		{
			offset = m_nReadOffset;
		}
		else
		{
			printf("%s:Read content file of id[%.0f] for LongFileFlow not by ++: that is not good \n", m_sPreFileName, id);
			fflush(stdout);
			offset = GetPos(m_nReadIndex, id);
		}
	}

	TFlowId flowId;
	fsetpos(m_flowIndex[thisIndexID]->pContentFile, &offset);
	if (fread(&flowId, sizeof(flowId), 1, m_flowIndex[thisIndexID]->pContentFile) != 1)
	{
		sprintf(m_sError, "%s:Can not read flowId of content file for CLongFileFlow while Reading %0.f", m_sPreFileName, id);
		EMERGENCY_EXIT(m_sError);
	}
	flowId.ChangeEndian();

	if (flowId.dwSize > length)
	{
		sprintf(m_sError, "%s:Insufficient buffer length while reading CFlow of %0.f", m_sPreFileName, id);
		EMERGENCY_EXIT(m_sError);
	}

	if (fread(pObject, 1, flowId.dwSize, m_flowIndex[thisIndexID]->pContentFile) != (size_t)flowId.dwSize)
	{
		sprintf(m_sError, "%s:Can not read pObject of content file for CLongFileFlow while Reading %0.f", m_sPreFileName, id);
		EMERGENCY_EXIT(m_sError);
	}

	//�����ȡ�ɹ��ˣ��ϵ��ļ��Ϳ�����Ϣ��
	if (thisIndexID != m_nReadIndex && m_nReadIndex != m_nCurrIndex)
	{
		CloseFile(m_flowIndex[m_nReadIndex]);
	}

	m_nReadId = id;
	m_nReadIndex = thisIndexID;

	FPOS_SET(m_nReadOffset, FPOS_GET(offset) + flowId.dwSize + sizeof(flowId));

	//LEAVE_CRITICAL(m_criticalVar);
	m_RWLock.UnLock();

	return flowId.dwSize;
}

FILE* CFileFlow::OpenFile(int nFileIndex)
{
	if (m_flowIndex[nFileIndex] == NULL)
	{
		SFlowFileInfo *pFlowFileInfo = new SFlowFileInfo();
		char sIndexName[256];
		//sprintf(sIndexName,"%s.%d.%.0f.flow",m_sPreFileName,thisIndexID,m_nCount);
		//����д��Ŀ����Ϊ��double�ͣ�����int��ˣ��������int
		pFlowFileInfo->BeginID = nFileIndex;
		pFlowFileInfo->BeginID *= m_nLongFileFlowGap;
		sprintf(sIndexName, "%s.%.0f.flow", m_sPreFileName, pFlowFileInfo->BeginID);
		pFlowFileInfo->FileName = sIndexName;
		pFlowFileInfo->pContentFile = NULL;
		m_flowIndex[nFileIndex] = pFlowFileInfo;
	}

	if (m_flowIndex[nFileIndex]->pContentFile != NULL)
	{
		return m_flowIndex[nFileIndex]->pContentFile;
	}
	else
	{
		FILE *pFile = mfopen(m_flowIndex[nFileIndex]->FileName.c_str(), "a+b");
		if (pFile == NULL)
		{
#ifdef WINDOWS
			sprintf(m_sError, "%s:can not open CFlow file:errorid[%d]", m_flowIndex[nFileIndex]->FileName.c_str(), GetLastError());
#else
			sprintf(m_sError, "%s:can not open CFlow file:errormsg[%s]", m_flowIndex[nFileIndex]->FileName.c_str(), strerror(errno));
#endif
			EMERGENCY_EXIT(m_sError);
		}
		m_flowIndex[nFileIndex]->pContentFile = pFile;
		return pFile;
	}
}

fpos_t CFileFlow::GetPos(int nFileIndex, UF_INT8 id)
{
	//���õ�ʱ����Ҫ��������Ϊ�Ƕ�ȡ�ļ���д�ļ�����Ӱ��
	int OffSetID = id - m_flowIndex[nFileIndex]->BeginID;
	fpos_t offset;
	FPOS_SET(offset, 0);

	TFlowId flowId;
	int i;
	for (i = 0; i < OffSetID; i++)
	{
		if (fsetpos(m_flowIndex[nFileIndex]->pContentFile, &offset) != 0)
		{
			sprintf(m_sError, "%s:fsetpos failed,OffsetID=%d", m_sPreFileName, i);
			EMERGENCY_EXIT(m_sError);
		}
		if (fread(&flowId, sizeof(flowId), 1, m_flowIndex[nFileIndex]->pContentFile) != 1)
		{
			sprintf(m_sError, "%s:fread failed,OffsetID=%d,%s,%d", m_sPreFileName, i, m_flowIndex[nFileIndex]->FileName.c_str(),nFileIndex);
			EMERGENCY_EXIT(m_sError);
		}
		flowId.ChangeEndian();
		FPOS_SET(offset, FPOS_GET(offset) + flowId.dwSize + sizeof(flowId));
	}
	return offset;
}

fpos_t CFileFlow::GetPos(int nFileIndex)
{
#ifdef WIN32	
	struct _stati64 statbuf;
	int result = _stati64(m_flowIndex[m_nCurrIndex]->FileName.c_str(), &statbuf);
#else
	struct stat statbuf;
	int result = stat(m_flowIndex[m_nCurrIndex]->FileName.c_str(), &statbuf);
#endif

	if (result != 0)
	{
		sprintf(m_sError, "%s:Problem getting file information", m_sPreFileName);
		EMERGENCY_EXIT(m_sError);
	}
	fpos_t offset;
	FPOS_SET(offset, statbuf.st_size);
	return offset;
}

UF_INT8 CFileFlow::GetIndexMaxCount(int nFileIndex)
{
	fpos_t size;
	FPOS_SET(size, 0);

	TFlowId flowId;
	int i;
	for (i = 0;; i++)
	{
		if (fsetpos(m_flowIndex[nFileIndex]->pContentFile, &size) != 0)
		{
			sprintf(m_sError, "%s:fsetpos failed", m_sPreFileName);
			EMERGENCY_EXIT(m_sError);
		}
		if (fread(&flowId, sizeof(flowId), 1, m_flowIndex[nFileIndex]->pContentFile) != 1)
			break;
		flowId.ChangeEndian();
		FPOS_SET(size, FPOS_GET(size) + flowId.dwSize + sizeof(flowId));
	}

	return m_flowIndex[nFileIndex]->BeginID + i;
}

bool CFileFlow::InitFile(UF_INT8 nCount)
{
	// 	m_nContentSize = GetOffset(nCount);
	// 
	// #ifdef WIN32
	// 	if(chsize(fileno(m_fpContentFile), m_nContentSize) != 0)
	// 	{
	// 		return false;
	// 	}
	// #else
	// 	ftruncate(fileno(m_fpContentFile), FPOS_GET(m_nContentSize));
	// #endif
	// 	m_nCount = nCount;
	return true;
}

void CFileFlow::CloseFile(SFlowFileInfo* pFlowFileInfo)
{
	if (pFlowFileInfo != NULL && pFlowFileInfo->pContentFile != NULL)
	{
		fclose(pFlowFileInfo->pContentFile);
		pFlowFileInfo->pContentFile = NULL;
	}
}

bool CFileFlow::Truncate(UF_INT8 nCount)
{
	if (m_nCount <= nCount)
	{
		return true;
	}
	return InitFile(nCount);
}

int CFileFlow::GetFlowType()
{
	return FLOW_TYPE_FILEFLOW;
}