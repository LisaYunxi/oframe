#ifndef __FTDCSUBSCRIBER_H_
#define __FTDCSUBSCRIBER_H_

#include "FTDCPackage.h"

// 订阅者接口，需要订阅的应用继承该类。一个订阅者只能订阅一个主题。
// 订阅者必须指明通讯阶段序号、已接收消息个数，用于可靠传输
// 当指定主题的消息抵达时，订阅驱动器将调用HandleMessage方法，用户可
// 以重载该方法，对消息进行处理。

class CFTDCSubscriber
{
public:
	virtual ~CFTDCSubscriber() {};
	virtual unsigned short GetSequenceSeries() = 0;
	virtual UF_INT8 GetReceivedCount() = 0;
	virtual void HandleMessage(CFTDCPackage *pMessage) = 0;
};

#endif

