#include "QdpShmServer.h"

/************************************************************************/
/*  QdpShmServer			                                                */
/*  in																    */
/*																		*/
/*  out																    */
/************************************************************************/
QdpShmServer::QdpShmServer(CServiceName *pName,int number,int size):CShmServer(pName,number,size)
{
	SInstrumentIDPos=new CInstrumentDataMap();
	count=0;
}

QdpShmServer::~QdpShmServer()
{

}

/************************************************************************/
/*  QdpShmSet			                                                */
/*  in																    */
/*																		*/
/*  out																    */
/************************************************************************/
int QdpShmServer::QdpShmSet()
{
	 m_baseshmadr=ShmSet();

	 return 0;
}

/************************************************************************/
/*  QdpHashKey	根据hash值取余                                          */
/*  in		const char *key											    */
/*																		*/
/*  out																    */
/************************************************************************/
unsigned int QdpShmServer::QdpHashKey(const char *key)
{
	//值计算
		
	unsigned int  jskey =  BKDRHash(key);

	unsigned int  modkey = jskey%999999;

	//printf("modkey=%d\n",modkey);
	
	return modkey;

}

/************************************************************************/
/*  BKDRHash hash算法                                                   */
/*  in		const char *str											    */
/*																		*/
/*  out																    */
/************************************************************************/
unsigned int QdpShmServer::BKDRHash(const char *str)
{

    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..

    unsigned int hash = 0;

    while (*str)
    {

        hash = hash * seed + (*str++);

    }

    return (hash & 0x7FFFFFFF);
}

/************************************************************************/
/*  QdpDataSave		保存数据                                            */
/*  in		CFTDDepthMarketDataField *filed							    */
/*																		*/
/*  out																    */
/************************************************************************/
int QdpShmServer::QdpDataSave(CFTDDepthMarketDataField *filed)
{

	unsigned int key;

	CInstrumentDataMap::iterator iter=SInstrumentIDPos->find(filed->InstrumentID);

	if (iter != SInstrumentIDPos->end())
	{
		
		count=0;

		key=iter->second;

	}else{
		
		count=1;

		key=QdpHashKey(filed->InstrumentID);

		//保存当前合约号的位置
		SInstrumentIDPos->insert(CInstrumentDataMap::value_type(filed->InstrumentID,key));
	
	}

	//固定区域保存
	QdpShmWrite(filed,key);
	
	//闪存位置保存
	QdpShmWrite(filed);

	return 0;
}

/************************************************************************/
/*  QdpShmWrite	闪存区域保存数据                                        */
/*  in		CFTDDepthMarketDataField *filed							    */
/*																		*/
/*  out																    */
/************************************************************************/
int QdpShmServer::QdpShmWrite(CFTDDepthMarketDataField *filed)
{

	struct ConnectStruct * pConnectAddr = (struct ConnectStruct*)m_baseshmadr;

	CFTDDepthMarketDataField *newfiled=new CFTDDepthMarketDataField();

	void *addnew=(void *)((char *)m_baseshmadr + sizeof(ConnectStruct));

	newfiled=(CFTDDepthMarketDataField *)addnew;
	
	CopyDepthMarketDataEntity(newfiled,filed);

	return 0;

}

/************************************************************************/
/*  QdpShmSet	固定key位置存储                                          */
/*  in		QdpShmWrite												    */
/*																		*/
/*  out																    */
/************************************************************************/
int QdpShmServer::QdpShmWrite(CFTDDepthMarketDataField *filed,unsigned int key)
{
	
	struct ConnectStruct * pConnectAddr = (struct ConnectStruct*)m_baseshmadr;

	CFTDDepthMarketDataField *newfiled=new CFTDDepthMarketDataField();
	
	//根据分配的key值取到首地址
	void *addnew=(char *)m_baseshmadr+sizeof(ConnectStruct)+(key+1)*sizeof(CFTDDepthMarketDataField);

	newfiled=(CFTDDepthMarketDataField *)addnew;

	//冲突检测，如果hash出现了冲突。打印
	if ( (1 == count ) && (0 != strcmp(newfiled->InstrumentID,filed->InstrumentID)) &&  (0 != strcmp(newfiled->InstrumentID,"")))
	{

		int kk=QdpHashKey(newfiled->InstrumentID);

		printf("hash  有冲突,newfiled->InstrumentID=%s,filed->InstrumentID=%s,key1=%d,key2=%d\n",newfiled->InstrumentID.getValue(),filed->InstrumentID.getValue(),kk,key);

	}

	CopyDepthMarketDataEntity(newfiled,filed);

	return 0;
}