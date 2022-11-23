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
	/**���캯������ָ���ķ��������������������
	* @param  pName	ָ���ķ�������
	*/
	QdpShmClient();
	
	/**�����������ͷ�ռ�õ���Դ
	*/
	virtual ~QdpShmClient();
	
	/************************************************************************/
	/*  ���ӹ����ڴ�                                                        */
	/************************************************************************/
	int QdpShmConn(char *caddr,int number);

	/************************************************************************/
	/*  ȡ�����ڴ�����������			                                    */
	/************************************************************************/
	int QdpShmLoadNew(CFTDDepthMarketDataField *filed);

	/************************************************************************/
	/*  �ú�Լ��ȥ��ȡ�����ڴ��е�����                                      */
	/************************************************************************/
	int QdpShmLoadByInstrumentID(CFTDDepthMarketDataField *filed,const char *InstrumentID);

private:

	/************************************************************************/
	/* ���������ڴ����ݶ�ȡ				                                    */
	/************************************************************************/
	int QdpShmRead(CFTDDepthMarketDataField *filed);


	/************************************************************************/
	/* ָ��λ�ö�ȡ�����ڴ�   									            */
	/************************************************************************/
	int QdpShmRead(CFTDDepthMarketDataField *filed,unsigned int key);


	/************************************************************************/
	/* ����hashֵ				                                            */
	/************************************************************************/
	unsigned int QdpHashKey(const char *key);

	/************************************************************************/
	/* ��ȡhashkey                                                          */
	/************************************************************************/
	unsigned int BKDRHash(const char *str);

	CQdpMarketDataMap *QdpmAllData;

	CInstrumentDataMap *CInstrumentIDPos;
	
	void *m_pShm;
};

#endif