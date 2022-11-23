#include "public.h"
#include "ServiceName.h"

CServiceName::CServiceName(const char *location)
{
	if ((location == NULL) || (*location == '\0'))
	{
		EMERGENCY_EXIT("empty location for service name");
	}
	m_pLocation = new char[strlen(location) + 1];
	m_pBuffer = new char[strlen(location) + 1];
	strcpy(m_pLocation, location);
	strcpy(m_pBuffer, location);
	char *buffer = m_pBuffer;
	char *startPort = "";
	m_pChannel = buffer;
	m_pHost = "";
	m_nPort = 0;
	m_pServiceName = "";
	char *p = strchr(buffer, ':');
	if (p == NULL)
	{
		//no semicolon in location, there should be only channel in this location
		return;
	}
	*p = '\0';
	p++;
	//check for // after semicolon
	if ((*p != '/') || (*(p + 1) != '/'))
		EMERGENCY_EXIT("Invalid location");
	p += 2;
	m_pHost = p;
	//loop until the end of hostname
	while ((*p) && (*p != ':') && (*p != '/'))
		p++;
	if (*p == ':')
	{
		*p = '\0';
		p++;
		startPort = p;
		//loop until the end of port
		while ((*p) && (*p != '/'))
			p++;
	}
	if (*p == '/')
	{
		*p = '\0';
		p++;
		m_pServiceName = p;
	}
	m_nPort = atoi(startPort);
	if (0 == m_nPort)
	{
		PRINT_TO_STD("Invalid port! ServiceName string: %s\n", location);
	}

	//获取之前的域名情况
	//对域名进行重新解析
	char tmpstr[256];
	if (strcmp(m_pBuffer, "tcp") == 0)
	{
		hostent *pHostent = gethostbyname(m_pHost);
		if (pHostent == NULL)
		{
			EMERGENCY_EXIT("Can not find host");
		}

		sprintf(tmpstr, "tcp://%s:%d", inet_ntoa(*(struct in_addr *)*pHostent->h_addr_list), m_nPort);
		strcpy(m_pLocation, tmpstr);
		strcpy(m_pBuffer, tmpstr);
		char *buffer = m_pBuffer;
		char *startPort = "";
		m_pChannel = buffer;
		m_pHost = "";
		m_pServiceName = "";
		char *p = strchr(buffer, ':');
		if (p == NULL)
		{
			//no semicolon in location, there should be only channel in this location
			return;
		}
		*p = '\0';
		p++;
		//check for // after semicolon
		if ((*p != '/') || (*(p + 1) != '/'))
			EMERGENCY_EXIT("Invalid location");
		p += 2;
		m_pHost = p;
		//loop until the end of hostname
		while ((*p) && (*p != ':') && (*p != '/'))
			p++;
		if (*p == ':')
		{
			*p = '\0';
			p++;
			startPort = p;
			//loop until the end of port
			while ((*p) && (*p != '/'))
				p++;
		}
		if (*p == '/')
		{
			*p = '\0';
			p++;
			m_pServiceName = p;
		}
	}
}

CServiceName::~CServiceName()
{
	delete[] m_pLocation;
	delete[] m_pBuffer;
}

