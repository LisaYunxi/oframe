/////////////////////////////////////////////////////////////////////////
///@file CLogger.h
///@brief��������CLogger
/////////////////////////////////////////////////////////////////////////

#ifndef CLOGGER_H
#define CLOGGER_H

#include "platform.h"

///������LOG����
#if 0
#define LL_EMERG       0       /* system is unusable */
#define LL_ALERT       1       /* action must be taken immediately */
#define LL_CRIT        2       /* critical conditions */
#define LL_ERR         3       /* error conditions */
#define LL_WARNING     4       /* warning conditions */
#define LL_NOTICE      5       /* normal but signification condition */
#define LL_INFO        6       /* informational */
#endif
#define LL_DEBUG       7       /* debug-level messages */

/**CLogger��һ���������־�ӿڣ�����Ӧ�ó��������־
* ����ȫ��ֻ��һ��ʵ����Ĭ��ʵ��Ϊ��stdout�����־��
* Ӧ�ó�����Լ̳�CLogger��ʵ�����ѵ���־��
*/
class CLogger
{
public:
	/** ��ȡCLoggerʵ��
	*@return ��ȡ��CLoggerʵ��
	*/
	static CLogger *GetInstance();

	///������־���
	///@param	indent	���ʱ������
	///@param	level	��־����ֵԽС������Խ��
	///@param	format	�����ʽ������printf�ĸ�ʽҪ��
	///@param	...	����������
	void output(int indent,int level,char *format,...);
	
	///������־���
	///@param	level	��־����ֵԽС������Խ��
	///@param	format	�����ʽ������printf�ĸ�ʽҪ��
	///@param	...	����������
	virtual void output(int level,char *format,...) = 0;
	
	///������־�������־����ΪLL_DEBUG
	///@param	format	�����ʽ������printf�ĸ�ʽҪ��
	///@param	...	����������
	void output(char *format,...);

protected:
	/**��������
	*/
	virtual ~CLogger(){};
protected:
	static CLogger *m_pInstance;		/**<CLoggerʵ��*/
};

class CFileLogger :public CLogger
{
public:
	CFileLogger()
	{
		m_fpLogFile = NULL;
	}
	~CFileLogger()
	{
		if (m_fpLogFile != NULL)
		{
			fclose(m_fpLogFile);
		}
	}
	
	bool Open(const char *pszFileName)
	{
		m_fpLogFile = fopen(pszFileName, "wb");
		if (m_fpLogFile == NULL)
		{
			return false;
		}
		return true;
	}
	
	virtual void output(int level, char *format, ...)
	{
		va_list v;
		va_start(v,format);
		vfprintf(m_fpLogFile,format,v);
		va_end(v);
		fflush(m_fpLogFile);
	}
private:
	FILE *m_fpLogFile;
};


#define LOG0(level, sz)					CLogger::GetInstance()->output(level, "%s", sz)
#define LOG1(level, sz, p1)				CLogger::GetInstance()->output(level, sz, p1)
#define LOG2(level, sz, p1, p2)			CLogger::GetInstance()->output(level, sz, p1, p2)
#define LOG3(level, sz, p1, p2, p3)		CLogger::GetInstance()->output(level, sz, p1, p2, p3)

#endif
