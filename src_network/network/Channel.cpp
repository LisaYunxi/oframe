#include "public.h"
#include "Channel.h"


CChannel::CChannel(TChannelType type, int id)
{
	m_nType = type;
	m_nID = id;
	m_bConnected = true;
	m_fpLog = NULL;
	m_pFlowLog = NULL;
}

CChannel::~CChannel()
{
}

int CChannel::Read(int number, char *buffer)
{
	int readNumber;
	if (!m_bConnected)
	{
		WriteLog(CLRT_DISCONNECTED_READ, 0, NULL);
		return -1;
	}
	readNumber = ReadImp(number, buffer);
	if (readNumber >= 0)
	{
		WriteLog(CLRT_READ, readNumber, buffer);
	}
	else
	{
		WriteLog(CLRT_READ_ERROR, 0, NULL);
	}
	return readNumber;
}

int CChannel::Write(int number, char *buffer)
{
	int writeNumber;
	if (!m_bConnected)
	{
		WriteLog(CLRT_DISCONNECTED_WRITE, 0, NULL);
		return -1;
	}
	writeNumber = WriteImp(number, buffer);
	if (writeNumber >= 0)
	{
		WriteLog(CLRT_WRITE, writeNumber, buffer);
	}
	else
	{
		WriteLog(CLRT_WRITE_ERROR, 0, NULL);
	}
	return writeNumber;
}

bool CChannel::Available(void)
{
	if (!m_bConnected)
		return 0;
	return AvailableImp();
}

bool CChannel::Disconnect()
{
	if (!m_bConnected)
		return 0;
	m_bConnected = DisconnectImp();
	WriteLog(CLRT_DISCONNECT, 0, NULL);
	return m_bConnected;
}

bool CChannel::CheckConnection()
{
	if (!m_bConnected)
	{
		return false;
	}
	m_bConnected = CheckConnectionImp();
	return m_bConnected;
}

int CChannel::GetSelectId(void)
{
	return m_nID;
}

char *CChannel::GetRemoteName(void)
{
	return "";
}

// 获取本地的地址IP
// @return   本地地址
char *CChannel::GetLocalName(void)
{
	return "";
}

char *CChannel::GetRemoteCommonName(void)
{
	return NULL;
}

void CChannel::WriteLog(unsigned short wType, unsigned short wDataLength, const char *pData)
{
	if (m_pFlowLog != NULL)
	{
		TChannelLogHeader header;
		header.dwId = (unsigned int)GetId();

#ifdef WIN32	
		struct _timeb timeout;
		_ftime(&timeout);
		header.dwTime = (unsigned int)timeout.time;
		header.dwMsTime = timeout.millitm;
#else
		struct timeval timeout;
		gettimeofday(&timeout, 0);
		header.dwTime = timeout.tv_sec;
		header.dwMsTime = timeout.tv_usec;
#endif
		header.wType = wType;
		if (wDataLength > 0)
			header.wLength = wDataLength;
		else
			header.wLength = 5;
		header.ChangeEndian();
		m_pFlowLog->Append(&header, sizeof(header));
		//为了方便读线程能够再分步骤读的时候截断的时候不出错，只有读完偶数个才能截断，这里强制写
		if (wDataLength > 0)
			m_pFlowLog->Append((char *)pData, wDataLength);
		else
			m_pFlowLog->Append((char *)"None", 5);
	}
}

void CChannel::SetLogFlow(CChannelLog *fp)
{
	m_pFlowLog = fp;
	char *pszRemoteName = GetRemoteName();
	WriteLog(CLRT_CONNECTED, strlen(pszRemoteName) + 1, pszRemoteName);
}
