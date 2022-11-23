//CFtdcUserDialogFlow是线程安全的CCachedFlow
//对于会话流，应用程序线程和API线程会同时访问，必须加以保护

class CFtdcUserDialogFlow : public CCachedFlow
{
public:
	CFtdcUserDialogFlow(bool bSyncFlag, int nMaxObjects, int nDataBlockSize)
		:CCachedFlow(bSyncFlag, "", nMaxObjects, nDataBlockSize)
	{
		m_nCount = CCachedFlow::GetCount();
	}
	
	virtual bool Truncate(int nCount)
	{
		m_lock.Lock();
		bool bRet = CCachedFlow::Truncate(nCount);
		m_nCount = CCachedFlow::GetCount();
		m_lock.UnLock();
		return bRet;
	}

	virtual UF_INT8 Append(void *pObject, int length)
	{
		m_lock.Lock();
		UF_INT8 nRet = -1;
		if(m_NodeQueue.size()<m_nMaxObjects)
		{
			nRet = CCachedFlow::Append(pObject, length);
			m_nCount = CCachedFlow::GetCount();
		}
		m_lock.UnLock();
		return nRet;
	}

 	virtual int Get(UF_INT8 id, void *pObject, int length)
	{
		m_lock.Lock();
		int nRet = CCachedFlow::Get(id, pObject, length);
		m_nCount = CCachedFlow::GetCount();
		PopFront();
		m_lock.UnLock();
		return nRet;
	}

	virtual int Get(UF_INT8 id, void **pObject, int length) { return -1; }
	
private:
	UF_INT8 m_nCount;
	CMutex m_lock;
};


class CUserFlow : public CFlow
{
public:
	CUserFlow(const char *pszFlowName, const char *pszPath, bool bReuse)
	{
		m_nCount= 0;
		m_fpContentFile=NULL;
		OpenFile(pszFlowName, pszPath, bReuse);
	}
	CUserFlow(int nFlowID, const char *pszPath, bool bReuse)
	{
		char szFlowName[20];
		sprintf(szFlowName, "%08x", nFlowID);
		new (this) CUserFlow(szFlowName, pszPath, bReuse);
	}

	virtual ~CUserFlow()
	{
		CloseFile();
	}
		
	void SetCount(UF_INT8 nCount)
	{
		m_nCount = nCount;
		WriteFile();
	}

	virtual bool Truncate(UF_INT8 nCount)
	{
//		if (m_nCount <= nCount)
//		{
//			return true;
//		}
		m_nCount= nCount;
		return WriteFile();
	}
	
	virtual UF_INT8 Append(void *pObject, int length)
	{
		m_nCount++;
		WriteFile();
		return m_nCount-1;		
	}

 	virtual int Get(UF_INT8 id, void *pObject, int length)
	{
		EMERGENCY_EXIT("Can't use this function - CUserFlow::Get\n");
		return 0;
	}

	virtual int GetFlowType() { return FLOW_TYPE_DIALOGFLOW; }

protected:	
	bool WriteFile()
	{
		fseek(m_fpContentFile, 0, SEEK_SET);
		if(fprintf(m_fpContentFile, "%.0f\n", m_nCount) <= 0)
		{
			return false;
		}
		//不flush文件，提高性能
		//fflush(m_fpContentFile);
		return true;
	}
	void OpenFile(const char *pszFlowName, const char *pszPath, bool bReuse)
	{
		m_nCount = 0;
		CloseFile();
		char szContentFilename[512];
		sprintf(szContentFilename, "%s%s.con", pszPath, pszFlowName);
		m_fpContentFile = mfopen(szContentFilename,"r+b");
		if (m_fpContentFile == NULL)
		{
			m_fpContentFile = mfopen(szContentFilename,"w+b");
			if(m_fpContentFile == NULL)
			{
				CloseFile();
				EMERGENCY_EXIT("can not open CFlow file");
			}			
		}
		
		if (bReuse)
		{
			fseek(m_fpContentFile, 0, SEEK_SET);
			if (fscanf(m_fpContentFile, "%lf\n", &m_nCount)!=2)
			{
				bReuse = false;
			}
		}
		else
		{
			if (!WriteFile())
			{
				CloseFile();
				EMERGENCY_EXIT("can not init CFlow file");
			}
		}
	}

	void CloseFile()
	{
		if(m_fpContentFile != NULL)
		{
			fclose(m_fpContentFile);
			m_fpContentFile = NULL;
		}
	}

private:
	FILE *m_fpContentFile;			/**< 存储内容的文件 */
	UF_INT8 m_nCount;				/**< 已经存在的包个数 */
};

class CFtdcUserFlowCtrlAutoPtr
{
public:
	CFtdcUserFlowCtrlAutoPtr(CMutex *pMutex)
	{
		m_pMutex = pMutex;
		m_pMutex->Lock();
	}
	~CFtdcUserFlowCtrlAutoPtr()
	{
		m_pMutex->UnLock();
	}
private:
	CMutex *m_pMutex;
};