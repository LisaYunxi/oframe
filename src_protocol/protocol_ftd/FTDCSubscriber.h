#ifndef __FTDCSUBSCRIBER_H_
#define __FTDCSUBSCRIBER_H_

#include "FTDCPackage.h"

// �����߽ӿڣ���Ҫ���ĵ�Ӧ�ü̳и��ࡣһ��������ֻ�ܶ���һ�����⡣
// �����߱���ָ��ͨѶ�׶���š��ѽ�����Ϣ���������ڿɿ�����
// ��ָ���������Ϣ�ִ�ʱ������������������HandleMessage�������û���
// �����ظ÷���������Ϣ���д���

class CFTDCSubscriber
{
public:
	virtual ~CFTDCSubscriber() {};
	virtual unsigned short GetSequenceSeries() = 0;
	virtual UF_INT8 GetReceivedCount() = 0;
	virtual void HandleMessage(CFTDCPackage *pMessage) = 0;
};

#endif

