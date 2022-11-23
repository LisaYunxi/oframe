#include "FrontLogger.h"
#include "BaseFunction.h"

CFrontLogger::CFrontLogger(const char* sFlowName, int nFrontID, int nGap)
{
	m_pFlow = new COneTimeFlow();
	m_nFrontNo = nFrontID;
	m_sFlowName = sFlowName;
	m_pLogFile = NULL;
	OpenLogFile();
	m_reader.AttachFlow(m_pFlow, 0);
	m_nCount = 0;
	m_nGap = nGap;
}

bool CFrontLogger::WriteLog(mg_connection* nc, const char *format, ...)
{
	char sLogBuffer[1024];
	sLogBuffer[0] = '\0';
	va_list v;
	va_start(v, format);
	vsnprintf(sLogBuffer + strlen(sLogBuffer), sizeof(sLogBuffer) - strlen(sLogBuffer), format, v);
	va_end(v);

	WriteLogBase(nc, sLogBuffer, 1024);

	return true;
}

bool CFrontLogger::WriteLogBase(mg_connection* nc, const char *pChar, int nLength)
{
	char sTimeBuffer[4098];
#ifdef WIN32	
	struct _timeb timeout;
	_ftime(&timeout);
	struct tm tt;
	LOCALTIME(&tt, &(timeout.time));
	int m_nSendUsec = tt.tm_sec * 1000 + timeout.millitm;
	if (nc != NULL)
	{
		if (nc->last_usec < 0)
		{
			nc->last_usec = m_nSendUsec;
			sprintf(sTimeBuffer, "[%02d-%02d %02d:%02d:%02d %06d][%06d][%08x][%s]",
				tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, timeout.millitm, nc->session_id, nc, inet_ntoa(nc->sa.sin.sin_addr));
		}
		else
		{
			int nLantency = m_nSendUsec - nc->last_usec;
			if (nLantency < 0)
				nLantency += 60 * 1000;
			sprintf(sTimeBuffer, "[%02d-%02d %02d:%02d:%02d %06d][%06d][%08x][%s][%04dms]",
				tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, timeout.millitm, nc->session_id, nc, inet_ntoa(nc->sa.sin.sin_addr), nLantency);
		}
	}
	else
	{
		sprintf(sTimeBuffer, "[%02d-%02d %02d:%02d:%02d %06d]", tt.tm_mon, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, timeout.millitm);
	}
#else
	struct timeval timeout;
	gettimeofday(&timeout, 0);
	struct tm tt;
	LOCALTIME(&tt, &(timeout.tv_sec));
	int m_nSendUsec = tt.tm_sec * 1000000 + timeout.tv_usec;
	if (nc != NULL)
	{
		if (nc->last_usec < 0)
		{
			nc->last_usec = m_nSendUsec;
			sprintf(sTimeBuffer, "[%02d-%02d %02d:%02d:%02d %06d][%06d][%08x][%s]",
				tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, timeout.tv_usec, nc->session_id, nc, inet_ntoa(nc->sa.sin.sin_addr));
		}
		else
		{
			int nLantency = m_nSendUsec - nc->last_usec;
			if (nLantency < 0)
				nLantency += 60 * 1000000;
			sprintf(sTimeBuffer, "[%02d-%02d %02d:%02d:%02d %06d][%06d][%08x][%s][%07dus]", 
				tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, timeout.tv_usec, nc->session_id, nc, inet_ntoa(nc->sa.sin.sin_addr), nLantency);
		}
	}
	else
	{
		sprintf(sTimeBuffer, "[%02d-%02d %02d:%02d:%02d %06d]", tt.tm_mon, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, timeout.tv_usec);
	}
#endif
	char* pNext = fastStrcatLimit(sTimeBuffer, " ");
	pNext = fastStrcatLimit(pNext, pChar, MIN(nLength, 4000));
	pNext = fastStrcatLimit(pNext, "\n");
	//sFlowBuffer += " ";
	//sFlowBuffer += pChar;
	//sFlowBuffer += "\n";
	//m_pFlow->Append((void *)(sFlowBuffer.c_str()), sFlowBuffer.length());
	m_pFlow->Append(sTimeBuffer, pNext - sTimeBuffer);
	return true;
}

void CFrontLogger::Run()
{
	char *pBuff;
	char sBuff[8192];
	int nDataLen = 8192;
	int iLen = 0;
	for (;;)
	{
		if (m_reader.GetFlow()->GetFlowType() == FLOW_TYPE_ONETIMEFLOW)
		{
			pBuff = (char *)m_reader.GetNext(iLen);
			if (pBuff == NULL)
			{
				SleepMs(1);
				continue;
			}
		}
		else
		{
			iLen = m_reader.GetNext(sBuff, nDataLen);
			if (iLen <= 0)
			{
				SleepMs(1);
				continue;
			}
			pBuff = sBuff;
		}

		//写入文件头和身体是两个报文，所以必须要偶数，要不然头和身体会在两个文件中，造成无法解析
		if (m_nCount > m_nGap && m_nCount % 2 == 0)
		{
			OpenLogFile();
			m_nCount = 0;
		}

		if (NULL != m_pLogFile)
		{
			fwrite(pBuff, 1, iLen, m_pLogFile);
			fflush(m_pLogFile);
			m_nCount++;
		}
	}
}

bool CFrontLogger::OpenLogFile()
{
	if (m_pLogFile)
		fclose(m_pLogFile);
	getLogMinTime(m_sDate);
	char szFullFilename[512];
	sprintf(szFullFilename, "%s_%d.%s.log", m_sFlowName.c_str(), m_nFrontNo, m_sDate);
	m_pLogFile = fopen(szFullFilename, "ab");
	if (m_pLogFile == NULL)
		return false;
	return true;
}