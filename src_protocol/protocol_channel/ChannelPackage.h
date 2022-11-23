#ifndef AFX_CHANNELPACKAGE_H__
#define AFX_CHANNELPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Package.h"
#include "Channel.h"

/**定义从Channel获取的原始的Package
*/
class  CChannelPackage : public CPackage
{
public:
	/**构造函数
	* @param nPackageMaxSize定义了数据缓冲区长度, 应大于协议栈能处理的最大报文长度
	*/
	CChannelPackage(int nPackageMaxSize);

	/**析构函数
	*/
	virtual ~CChannelPackage();

	/**从Channel中读取数据
	* @param pChannel 指向Channel指针
	* @return 从Channel中读到的数据长度
	*/
	int ReadFromChannel(CChannel *pChannel);

	/**通讯包上传时，上层协议的编号，本框架中返回的是XMPProtocol协议编号
	* @return 上层协议的编号 说明(本函数返回0，因为上层协议只有XMPProtocol并
	* 且编号为0)
	*/
	virtual unsigned int GetActiveID();

	/** 检查包内数据是否构成一个完整的包。说明(由于对应协议未做组包，所以
	* 本方总是返回完整的数据包)
	* @return 1 是一个完整的包
	* @return 0 数据包不完整
	* @return -1 数据包有错误
	*/
	virtual int ValidPackage();
};

#endif
