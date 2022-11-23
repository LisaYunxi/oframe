#ifndef _QDPSHMCLIENT_H_
#define _QDPSHMCLIENT_H_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ShmClient.h"
#define CFFEX 1
#define SHFE 2


class QdpShmClient : CShmClient
{
public:
	/**构造函数，在指定的服务名上侦听网络的连接
	* @param  pName	指定的服务名称
	*/
	QdpShmClient();
	
	/**析构函数，释放占用的资源
	*/
	virtual ~QdpShmClient();
	
	/************************************************************************/
	/*  链接共享内存                                                        */
	/************************************************************************/
	int QdpShmConn(char *caddr,int number);

	/************************************************************************/
	/*  取共享内存闪存中数据			                                    */
	/************************************************************************/
	int QdpShmLoadNew(CFTDDepthMarketDataField *filed);

	/************************************************************************/
	/*  用合约号去读取共享内存中的数据                                      */
	/************************************************************************/
	int QdpShmLoadByInstrumentID(CFTDDepthMarketDataField *filed,const char *InstrumentID);

private:

	/************************************************************************/
	/* 单条共享内存内容读取				                                    */
	/************************************************************************/
	int QdpShmRead(CFTDDepthMarketDataField *filed);


	/************************************************************************/
	/* 指定位置读取共享内存   									            */
	/************************************************************************/
	int QdpShmRead(CFTDDepthMarketDataField *filed,unsigned int key);


	/************************************************************************/
	/* 计算hash值				                                            */
	/************************************************************************/
	unsigned int QdpHashKey(const char *key);

	/************************************************************************/
	/* 获取hashkey                                                          */
	/************************************************************************/
	unsigned int BKDRHash(const char *str);

	CQdpMarketDataMap *QdpmAllData;

	CInstrumentDataMap *CInstrumentIDPos;
	
	void *m_pShm;
};

#endif