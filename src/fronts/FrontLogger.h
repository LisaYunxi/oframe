#ifndef __FrontLogger_H__
#define __FrontLogger_H__

#include "mongoose.h"
#include "platform.h"
#include "Thread.h"
#include "FlowReader.h"
#include "OneTimeFlow.h"
#include "printinfo.h"

class CFrontLogger : public CThread
{
public:
	CFrontLogger(const char* sFlowName, int nFrontID, int nGap);
	bool WriteLog(mg_connection* nc, const char *format, ...);
	bool WriteLogBase(mg_connection* nc, const char *pChar, int nLength = 1024);
	virtual void Run();

private:
	bool OpenLogFile();
	FILE *m_pLogFile;
	CFlowReader m_reader;
	string m_sFlowName;
	int m_nCount;
	int m_nGap;
	char m_sDate[13];				//日志文件日期
	int m_nFrontNo;
	CFlow *m_pFlow;
};
#endif
