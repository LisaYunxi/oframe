#ifndef __FIBSUBSCRIBER_H_
#define __FIBSUBSCRIBER_H_

#include "XTPPackage.h"

/** 订阅者接口，需要订阅的应用继承该类。一个订阅者只能订阅一个主题。
* 订阅者必须指明通讯阶段序号、已接收消息个数，用于可靠传输
* 当指定主题的消息抵达时，订阅驱动器将调用HandleMessage方法，用户可
* 以重载该方法，对消息进行处理。
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

