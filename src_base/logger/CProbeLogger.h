#ifndef CPROBELOGGER_H
#define CPROBELOGGER_H

#include "platform.h"

#define MAX_PARAMETER_NAME 100

class CProbeLogger
{
public:
	///���췽��
	///@param	programName	ָ���ĳ�����
	///@param	globalSeq	����ȫ�����к�
	CProbeLogger()
	{
	}
	
	///��������
	virtual ~CProbeLogger(void)
	{
	}

	///����̽����Ϣ��ֵΪ�ַ���
	///@param	parameter	������
	///@param	value	Ҫ���͵���Ϣ
	void SendProbeMessage(const char *parameter, const char *value);

	///����̽����Ϣ��ֵΪ����
	///@param	parameter	������
	///@param	value	Ҫ���͵���Ϣ
	void SendProbeMessage(const char *parameter, const int value);

	///����̽����Ϣ��ֵΪ������
	///@param	parameter	������
	///@param	value	Ҫ���͵���Ϣ
	void SendProbeMessage(const char *parameter, const double value);

	///����̽����Ϣ��ֵΪ���������԰ٷֱȷ�ʽ��ʾ
	///@param	parameter	������
	///@param	value	Ҫ���͵���Ϣ
	void SendPercentageProbeMessage(const char *parameter, const double value);
protected:
	virtual void SendProbeMessageImpl(const char *parameter, const char *value) = 0;
};

#endif
