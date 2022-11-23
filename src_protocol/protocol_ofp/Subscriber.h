#ifndef __SUBSCRIBER_H_
#define __SUBSCRIBER_H_

#include "XTPPackage.h"

/** �����߽ӿڣ���Ҫ���ĵ�Ӧ�ü̳и��ࡣһ��������ֻ�ܶ���һ�����⡣
* �����߱���ָ��ͨѶ�׶���š��ѽ�����Ϣ���������ڿɿ�����
* ��ָ���������Ϣ�ִ�ʱ������������������HandleMessage�������û���
* �����ظ÷���������Ϣ���д���
*/
class CSubscriber
{
public:

	virtual ~CSubscriber(){};
	
	/**��ȡ���ĵ��������
	*@return ���ĵ��������
	*/
	virtual unsigned int GetSubjectNo() = 0;

	/**��ȡ���յ�����Ϣ����
	*@return ���յ�����Ϣ����
	*/
	virtual unsigned int GetReceivedCount() = 0;

	/**�����յ���һ����Ϣ
	*@param pMessage �յ���һ����Ϣ
	*/
	virtual void HandleMessage(CXTPPackage *pMessage) = 0;

	/**�����ٽ���
	*/
	virtual void EnterCritical() {}
	
	/**�뿪�ٽ���
	*/
	virtual void LeaveCritical() {}

};

#endif

