#ifndef __FIBSUBSCRIBER_H_
#define __FIBSUBSCRIBER_H_

#include "XTPPackage.h"

/** �����߽ӿڣ���Ҫ���ĵ�Ӧ�ü̳и��ࡣһ��������ֻ�ܶ���һ�����⡣
* �����߱���ָ��ͨѶ�׶���š��ѽ�����Ϣ���������ڿɿ�����
* ��ָ���������Ϣ�ִ�ʱ������������������HandleMessage�������û���
* �����ظ÷���������Ϣ���д���
*/
class CFIBSubscriber
{
public:
	virtual ~CFIBSubscriber(){};
	virtual void SetCommPhaseNo(DWORD wCommPhaseNo) = 0;
	virtual WORD GetSequenceSeries() = 0;
	virtual DWORD GetReceivedCount() = 0;
	virtual void HandleMessage(CXTPPackage *pMessage) = 0;
};
#endif

