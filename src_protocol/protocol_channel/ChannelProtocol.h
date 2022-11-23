#ifndef AFX_CHANNELPROTOCOL_H__
#define AFX_CHANNELPROTOCOL_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Protocol.h"
#include "Channel.h"
#include "CacheList.h"
#include "ChannelPackage.h"

#define MSG_CHNLERR_READ		0x1001
#define MSG_CHNLERR_WRITE		0x1002
#define MSG_DRSTROY_ME			0x1003

//协议栈容器的最底层协议，负责从网络中发送和接受网络数据包，并传递到上层协议

class  CChannelProtocol : public CProtocol
{
public:
	//构造函数
	//@param pReactor 指向运行引擎的指针
	//@param pChannel 捆绑的Channel指针
	//@param nPackageMaxSize 接受缓冲区的最大长度
	CChannelProtocol(CReactor *pReactor, CChannel *pChannel, int nPackageMaxSize);

	//析构函数
	virtual ~CChannelProtocol();

	//将发送包增加的发送列表,同时将发送列表中未发送的数据转发出去
	//@param pPackage 需要发送的包指针
	virtual int Push(CPackage *pPackage, CProtocol *pUpper);

	//获取Channel的Id，并检查是否有发送任务
	//@param pReadId　 Channel的Id，用于返回　
	//@param pWriteId  检查是否有发送任务，如有，返回Channel的Id，否则，返回NULL
	virtual void GetIds(int *pReadId, int *pWriteId);

	//由容器调用，负责从网络读取数据包，并传至上层协议
	//@return 0   成功
	//@return <>0 失败
	virtual int HandleInput();

	//由容器调用，负责向网络发送数据包
	//@return >=0   成功
	//@return <0    失败
	virtual int HandleOutput();

	//处理定时操作，由CReactor调用
	//@param nIDEvent 定时器标识
	virtual void OnTimer(int nIDEvent);

	//负责向网络发送数据包
	//@return >=0   成功
	//@return <0    失败
	int Flush();

	//检查是否有要写出的内容
	//@return true 有要写出的内容, false 没有要写出的内容
	bool IsWriteBusy();
protected:
	CChannel *m_pChannel;				//指向Channel的指针
	CCacheList m_CacheList;				//发送缓冲区链
	CChannelPackage m_ChannelPackage;	 // 本层协议的对应Package
};

#endif
