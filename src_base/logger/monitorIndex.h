/////////////////////////////////////////////////////////////////////////
///monitorIndex.h
///���������йص�����ָ��
/////////////////////////////////////////////////////////////////////////

#ifndef MONITORINDEX_H
#define MONITORINDEX_H

#include "platform.h"
#include "CProbeLogger.h"

/////////////////////////////////////////////////////////////////////////
///IMonitorIndex�ǳ���ļ��ָ��Ľӿ�
/////////////////////////////////////////////////////////////////////////
class IMonitorIndex
{
public:
	virtual ~IMonitorIndex(void)
	{
	};

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger) = 0;
};

/////////////////////////////////////////////////////////////////////////
///CMonitorIndex�Ǽ��ָ��Ļ����࣬���еľ������ͱ���̳д���
/////////////////////////////////////////////////////////////////////////
class CMonitorIndex : public IMonitorIndex
{
public:
	///���췽��
	///@param	frequncy	��ָ���ˢ��Ƶ��,С�ڵ���0��ʾ����Ҫͳһ����
	CMonitorIndex(int frequncy)
	{
		if (!m_inited)
		{
			INIT_CRITICAL_VAR(m_criticalVar);
			m_indexList = new vector<CMonitorIndex *>;
			m_inited = true;
		}

		if (frequncy > 0)
		{
			ENTER_CRITICAL(m_criticalVar);
			m_frequncy = frequncy;
			m_indexList->push_back(this);
			m_nextTime = 0;
			LEAVE_CRITICAL(m_criticalVar);
		}
	}

	///��������
	virtual ~CMonitorIndex(void)
	{
		ENTER_CRITICAL(m_criticalVar);
		vector<CMonitorIndex *>::iterator it;
		for (it = m_indexList->begin(); it < m_indexList->end(); it++)
		{
			if (*it == this)
			{
				m_indexList->erase(it);
				break;
			}
		}
		LEAVE_CRITICAL(m_criticalVar);
	}

	///����̽��
	///@param	pProbeLogger	�µ�̽��
	static void setProbeLogger(CProbeLogger *pProbeLogger);

	///��ȡ̽��
	///@param	pProbeLogger	̽��
	static CProbeLogger * getProbeLogger();

	///-------
	///ͨ�����ù������еı�׼�������
	///-------
	static void init(CProbeLogger *pProbeLogger, const char *logLevelString);

	///���һ�����鴦���������ʵ��ؼ�飬���ڱ�Ҫ������£����Ա���
	static void handleOne(void);

	///�����µ����鴦����Ƶ��
	///@param	newFrequncy	�µ�ˢ��Ƶ��
	static void setHandleCountCheckFrequncy(int newFrequncy);

	///�������е�ָ��
	static void reportAll(void);
protected:
	///��ָ���ˢ��Ƶ��
	int m_frequncy;

	///��������´�ˢ���¼�
	time_t m_nextTime;

	///���µ���һ�ε�ʱ��
	static time_t m_minNextTime;

	///��¼����ָ���嵥
	static vector<CMonitorIndex *>	*m_indexList;

	///ʹ�õ�̽��
	static CProbeLogger *m_probeLogger;

	///��Ҫ�ȶ��ٴ�����Ĵ����ٽ��м��
	static int m_handleCountWait;

	///��������Ĵ���ÿ��������ʱ������һ�μ��
	static int m_handleCountCheckFrequncy;

	///�ٽ�������
	static CRITICAL_VAR m_criticalVar;

	///�Ƿ����б����Ѿ���ɳ�ʼ��
	static bool m_inited;

	///Ŀǰ����־���𣬷�Χ��0��3������Խ����־Խ��
	static int m_logLevel;
};

/////////////////////////////////////////////////////////////////////////
///CIntMonitorIndex�Ǽ��һ������ָ�����
/////////////////////////////////////////////////////////////////////////
class CIntMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CIntMonitorIndex(const char *name, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_value = 0;
	}

	///��������
	~CIntMonitorIndex(void)
	{
	}

	///����ֵ
	///@param	value	�µ�ֵ
	void incValue(int value = 1)
	{
		m_value += value;
	}

	///����ֵ
	///@param	value	�µ�ֵ
	void setValue(int value)
	{
		m_value = value;
	}

	///��ȡֵ
	///@return	�õ��ĵ�ǰֵ
	int getValue(void)
	{
		return m_value;
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_name;
	int m_value;
};

/////////////////////////////////////////////////////////////////////////
///CUFINT8MonitorIndex�Ǽ��һ��UF_INT8ָ�����
/////////////////////////////////////////////////////////////////////////
class CUFINT8MonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CUFINT8MonitorIndex(const char *name, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_value = 0;
	}

	///��������
	~CUFINT8MonitorIndex(void)
	{
	}

	///����ֵ
	///@param	value	�µ�ֵ
	void incValue(UF_INT8 value = 1)
	{
		m_value += value;
	}

	///����ֵ
	///@param	value	�µ�ֵ
	void setValue(UF_INT8 value)
	{
		m_value = value;
	}

	///��ȡֵ
	///@return	�õ��ĵ�ǰֵ
	UF_INT8 getValue(void)
	{
		return m_value;
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_name;
	UF_INT8 m_value;
};


/////////////////////////////////////////////////////////////////////////
///CIntUsageMonitorIndex�Ǽ��һ������Ϊ��������Ҫ����ʹ���ʵ�ָ����
/////////////////////////////////////////////////////////////////////////
class CIntUsageMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	totalValue	��ָ�������
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CIntUsageMonitorIndex(const char *name, int totalValue, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_value = 0;
		m_totalValue = totalValue;
		sprintf(m_usageName, "%sUsage", m_name);
	}

	///��������
	~CIntUsageMonitorIndex(void)
	{
	}

	///����ֵ
	///@param	value	�µ�ֵ
	void incValue(int value = 1)
	{
		m_value += value;
	}

	///����ֵ
	///@param	value	�µ�ֵ
	void setValue(int value)
	{
		m_value = value;
	}

	///��ȡֵ
	///@return	�õ��ĵ�ǰֵ
	int getValue(void)
	{
		return m_value;
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_name;
	char m_usageName[MAX_PARAMETER_NAME];
	int m_value;
	int m_totalValue;
};

/////////////////////////////////////////////////////////////////////////
///CIntPtrMonitorIndex�Ǽ��һ������ָ����ࣨʹ��ָ�룩
/////////////////////////////////////////////////////////////////////////
class CIntPtrMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	ptr	ָ��������ָ��
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CIntPtrMonitorIndex(const char *name, const int *ptr, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_ptr = ptr;
		m_Total = 0;
	}

	///���캯�� ����ٷֱȵ�ָ�����
	///@param	name	��������
	///@param	ptr	ָ��������ָ��
	///@param	nTotal	ָ��������ָ��
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CIntPtrMonitorIndex(const char *name, const int *ptr, int nTotal, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_ptr = ptr;
		m_Total = nTotal;
	}

	///��������
	~CIntPtrMonitorIndex(void)
	{
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_name;
	const int *m_ptr;
	int	m_Total;
};


/////////////////////////////////////////////////////////////////////////
///CPerIntPtrMonitorIndex�Ǽ��һ����λʱ��������ָ����ࣨʹ��ָ�룩
/////////////////////////////////////////////////////////////////////////
class CPerIntPtrMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	ptr	ָ��������ָ��
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CPerIntPtrMonitorIndex(const char *name, const int *ptr, int frequncy) : CMonitorIndex(frequncy)
	{
		//memset(m_name,0,sizeof(m_name));
		//strncpy(m_name,name,MAX_PARAMETER_NAME-1);
		// ��ȷ����
		int nLength = (int)strlen(name);
		strncpy(m_name, name, nLength);
		m_name[nLength] = '\0';
		m_ptr = ptr;
		m_per = 0;
	}

	///��������
	~CPerIntPtrMonitorIndex(void)
	{
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	char m_name[MAX_PARAMETER_NAME];
	const int *m_ptr;
	int        m_per;
};

/////////////////////////////////////////////////////////////////////////
///CLongPtrMonitorIndex�Ǽ��һ��������ָ����ࣨʹ��ָ�룩
/////////////////////////////////////////////////////////////////////////
class CLongPtrMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	ptr	ָ��������ָ��
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CLongPtrMonitorIndex(const char *name, const UF_INT8 *ptr, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_ptr = ptr;
		m_Total = 0;
	}

	///���캯�� ����ٷֱȵ�ָ�����
	///@param	name	��������
	///@param	ptr	ָ��������ָ��
	///@param	nTotal	ָ��������ָ��
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CLongPtrMonitorIndex(const char *name, const UF_INT8 *ptr, UF_INT8 nTotal, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_ptr = ptr;
		m_Total = nTotal;
	}

	///��������
	~CLongPtrMonitorIndex(void)
	{
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_name;
	const UF_INT8 *m_ptr;
	UF_INT8 m_Total;
};

/////////////////////////////////////////////////////////////////////////
///CPerLongPtrMonitorIndex�Ǽ��һ����λʱ���ڳ�����ָ����ࣨʹ��ָ�룩
///@author	wl
///@version	1.0,20101029
/////////////////////////////////////////////////////////////////////////
class CPerLongPtrMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	ptr	ָ��������ָ��
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CPerLongPtrMonitorIndex(const char *name, const UF_INT8 *ptr, int frequncy) : CMonitorIndex(frequncy)
	{
		//memset(m_name,0,sizeof(m_name));
		//strncpy(m_name,name,MAX_PARAMETER_NAME-1);
		//��ȷ����
		int nLength = STRLEN(name);
		strncpy(m_name, name, nLength);
		m_name[nLength] = '\0';
		m_ptr = ptr;
		m_per = 0;
	}

	///��������
	~CPerLongPtrMonitorIndex(void)
	{
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	char m_name[MAX_PARAMETER_NAME];
	const UF_INT8 *m_ptr;
	UF_INT8 m_per;
};

/////////////////////////////////////////////////////////////////////////
///CBoolMonitorIndex�Ǽ��һ������ֵָ�����
/////////////////////////////////////////////////////////////////////////
class CBoolMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CBoolMonitorIndex(const char *name, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_value = 0;
	}

	///��������
	~CBoolMonitorIndex(void)
	{
	}

	///����ֵ
	///@param	value	�µ�ֵ
	void setValue(bool value)
	{
		m_value = value;
	}

	///��ȡֵ
	///@return	�õ��ĵ�ǰֵ
	bool getValue(void)
	{
		return m_value;
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_name;
	bool m_value;
};

/////////////////////////////////////////////////////////////////////////
///CStringMonitorIndex�Ǽ��һ���ַ���ֵָ�����
/////////////////////////////////////////////////////////////////////////
class CStringMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	value	ָ�����ַ���
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CStringMonitorIndex(const char *name, const char *value, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_value = value;
	}

	///��������
	~CStringMonitorIndex(void)
	{
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_name;
	const char *m_value;
};

/////////////////////////////////////////////////////////////////////////
///CIntTotalMonitorIndex�Ǽ��һ������������ָ����
/////////////////////////////////////////////////////////////////////////
class CIntTotalMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CIntTotalMonitorIndex(const char *name, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_value = 0;
		m_totalValue = 0;
		sprintf(m_totalName, "%sTotal", m_name);
	}

	///��������
	~CIntTotalMonitorIndex(void)
	{
	}

	///����ֵ
	///@param	value	�µ�ֵ
	void incValue(int value = 1)
	{
		m_value += value;
	}

	///��ȡֵ
	///@return	�õ��ĵ�ǰֵ
	int getValue(void)
	{
		return m_value;
	}

	///��ȡ�ۼ�ֵ
	///@return	��ǰ���ۼ�ֵ
	int getTotalValue(void)
	{
		return m_totalValue + m_value;
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_name;
	char m_totalName[MAX_PARAMETER_NAME];
	int m_value;
	int m_totalValue;
};

class CUFINT8TotalMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CUFINT8TotalMonitorIndex(const char *name, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_value = 0;
		m_totalValue = 0;
		sprintf(m_totalName, "%sTotal", m_name);
	}

	///��������
	~CUFINT8TotalMonitorIndex(void)
	{
	}

	///����ֵ
	///@param	value	�µ�ֵ
	void incValue(int value = 1)
	{
		m_value += value;
	}

	///��ȡֵ
	///@return	�õ��ĵ�ǰֵ
	int getValue(void)
	{
		return m_value;
	}

	///��ȡ�ۼ�ֵ
	///@return	��ǰ���ۼ�ֵ
	double getTotalValue(void)
	{
		return m_totalValue + m_value;
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_name;
	char m_totalName[MAX_PARAMETER_NAME];
	int m_value;
	double m_totalValue;
};

/////////////////////////////////////////////////////////////////////////
///CStringBufferMonitorIndex�Ǽ��һ���ַ���ֵָ�����
/////////////////////////////////////////////////////////////////////////
class CStringBufferMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CStringBufferMonitorIndex(const char *name, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_value[0] = '\0';
	}

	///���캯��
	///@param	name	��������
	///@param	value	����ֵ
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CStringBufferMonitorIndex(const char *name, const char *value, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		setValue(value);
	}

	///��������
	~CStringBufferMonitorIndex(void)
	{
	}

	///����ֵ
	///@param	value	����ֵ
	void setValue(const char *value)
	{
		strcpy(m_value, value);
	}

	///��ȡֵ
	///@return	��ǰ��ֵ
	const char *getValue(void)
	{
		return m_value;
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_name;
	char m_value[1024];
};

/////////////////////////////////////////////////////////////////////////
///CStringBufferArrayMonitorIndex�Ǽ��һ���ַ���ֵָ���������
/////////////////////////////////////////////////////////////////////////
class CStringBufferArrayMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	size	�����С
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CStringBufferArrayMonitorIndex(const char *name, int size, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_size = size;
		m_value = new char*[size];
		for (int i = 0; i < size; i++)
		{
			m_value[i] = new char[1024];
			m_value[i][0] = '\0';
		}
	}

	///��������
	~CStringBufferArrayMonitorIndex(void)
	{
	}

	///����ֵ
	///@param	id	�±�
	///@param	value	����ֵ
	void setValue(int id, const char *value)
	{
		if ((id < 1) || (id > m_size))
		{
			return;
		}
		strcpy(m_value[id - 1], value);
	}

	///��ȡֵ
	///@param	id	�±�
	///@return	��ǰ��ֵ
	const char *getValue(int id)
	{
		if ((id < 1) || (id > m_size))
		{
			return "";
		}
		return m_value[id - 1];
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_name;
	char **m_value;
	int m_size;
};

/////////////////////////////////////////////////////////////////////////
///CFloatMonitorIndex�Ǽ��һ��������ָ�����
/////////////////////////////////////////////////////////////////////////
class CFloatMonitorIndex : public CMonitorIndex
{
public:
	///���캯��
	///@param	name	��������
	///@param	frequncy	��ָ���ˢ��Ƶ��
	CFloatMonitorIndex(const char *name, int frequncy) : CMonitorIndex(frequncy)
	{
		m_name = name;
		m_value = 0.0;
	}

	///��������
	~CFloatMonitorIndex(void)
	{
	}

	///����ֵ
	///@param	value	�µ�ֵ
	void incValue(double value)
	{
		m_value += value;
	}

	///����ֵ
	///@param	value	�µ�ֵ
	void setValue(double value)
	{
		m_value = value;
	}

	///��ȡֵ
	///@return	�õ��ĵ�ǰֵ
	double getValue(void)
	{
		return m_value;
	}

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_name;
	double m_value;
};

#define LOG_NONE		0
#define LOG_EMERGENCY	1
#define LOG_CRITICAL	2
#define LOG_ERROR		3
#define LOG_WARNING		4
#define LOG_INFO		5
#define LOG_DEBUG		6

/////////////////////////////////////////////////////////////////////////
///CEventMonitor�Ǽ��һ���¼�����
/////////////////////////////////////////////////////////////////////////
class CEventMonitor : public CMonitorIndex
{
public:
	///���캯��
	///@param	type	����
	///@param	ipAdr	IP��ַ
	///@param	user	�û���
	CEventMonitor(const char *type = "", const char *ipAdr = "", const char *user = "") : CMonitorIndex(0)
	{
		m_type = type;
		m_ipAdr = ipAdr;
		m_user = user;
	}

	///��������
	~CEventMonitor(void)
	{
	}

	///�����¼�
	///@param	detail	Ҫ�������ϸ����
	void report(const char *detail);

	///�����¼�
	///@param	type	����
	///@param	detail	Ҫ�������ϸ����
	void report(const char *type, const char *detail);

	///����ĳ�����͵��¼�
	///@param	level	����ļ���
	///@param	type	����
	///@param	format	����ĸ�ʽ
	///@param	...	����Ĳ���
	void report(int level, const char *type, const char *format, ...);

	///�����¼�
	///@param	type	����
	///@param	ipAdr	IP��ַ
	///@param	user	�û���
	///@param	detail	Ҫ�������ϸ����
	void report(const char *type, const char *ipAdr, const char *user, const char *detail);

	///����ָ�����Ա���
	///@param	pProbeLogger	����ʱʹ�õ�̽��
	virtual void report(CProbeLogger *pProbeLogger);
private:
	const char *m_type;
	const char *m_ipAdr;
	const char *m_user;
};

extern CEventMonitor stdEventMonitor;

#define REPORT_EVENT stdEventMonitor.report

#endif
