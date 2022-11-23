#ifndef CSYSLOGSERVICE_H
#define CSYSLOGSERVICE_H

#ifdef UNIX
#include <syslog.h>

#include "CProbeLogger.h"

class CSyslogService : public CProbeLogger
{
public:
	///���췽��
	///@param	programName	ָ���ĳ�����
	///@param	globalSeq	����ȫ�����к�
	CSyslogService(): CProbeLogger()
	{
		openlog( "Syslog", LOG_PID, LOG_USER );
	}
	
	///��������
	virtual ~CSyslogService(void)
	{
		closelog();
	}

	virtual void SendProbeMessageImpl(const char *parameter, const char *value);
private:
	char m_ident[100];
};

#endif

#endif
