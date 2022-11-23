#ifndef __SUBSCRIBER_H_
#define __SUBSCRIBER_H_

#include "XTPPackage.h"

/** 订阅者接口，需要订阅的应用继承该类。一个订阅者只能订阅一个主题。
* 订阅者必须指明通讯阶段序号、已接收消息个数，用于可靠传输
* 当指定主题的消息抵达时，订阅驱动器将调用HandleMessage方法，用户可
* 以重载该方法，对消息进行处理。
*/
class CSubscriber
{
public:

	virtual ~CSubscriber(){};
	
	/**获取订阅的主题代码
	*@return 订阅的主题代码
	*/
	virtual unsigned int GetSubjectNo() = 0;

	/**获取已收到的消息个数
	*@return 已收到的消息个数
	*/
	virtual unsigned int GetReceivedCount() = 0;

	/**处理收到的一个消息
	*@param pMessage 收到的一个消息
	*/
	virtual void HandleMessage(CXTPPackage *pMessage) = 0;

	/**进入临界区
	*/
	virtual void EnterCritical() {}
	
	/**离开临界区
	*/
	virtual void LeaveCritical() {}

};

#endif

