#include "QdpShmClient.h"

QdpShmClient::QdpShmClient()
{

	CInstrumentIDPos=new CInstrumentDataMap();

}

QdpShmClient::~QdpShmClient()
{
	
	delete CInstrumentIDPos;

}

/************************************************************************/
/*  QdpShmConn连接共享内存                                              */
/*  in		CServiceName											    */
/*			number														*/
/*			msize  		共享内存大小									*/
/*  out																    */
/************************************************************************/
int QdpShmClient::QdpShmConn(char *caddr,int number)
{

	CServiceName *servname=new CServiceName(caddr);

	int CQdplen=sizeof(CFTDDepthMarketDataField);
	
	shmsize=CQdplen*1000000;

	m_pShm = ShmConnect(servname,number);
	
	delete servname;

	return 0;
}

/************************************************************************/
/*  QdpHashKey计算hash值	                                            */
/*  in		const char *key											    */
/*			                        									*/
/*  out		共享内存位置key												*/
/************************************************************************/
unsigned int QdpShmClient::QdpHashKey(const char *key)
{
	
	unsigned int  jskey =  BKDRHash(key);

	unsigned int  modkey = jskey%999999;

	printf("modkey=%d\n",modkey);

	return modkey;
}

/************************************************************************/
/*  BKDRHash			                                                */
/*  in		const char *str											    */
/*																		*/
/*  out																    */
/************************************************************************/
unsigned int QdpShmClient::BKDRHash(const char *str)
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
/*  QdpShmLoadNew			                                                */
/*  in		const char *str											    */
/*																		*/
/*  out																    */
/************************************************************************/
int QdpShmClient::QdpShmLoadNew(CFTDDepthMarketDataField *filed)
{
	QdpShmRead(filed);
	
	//CInstrumentIDPos->insert(InstrumentID,key);
	
	return 	0;

}

/************************************************************************/
/*  QdpShmLoadOne 用合约号去读取共享内存中的数据                        */
/*  in		CFTDDepthMarketDataField *file   返回载体					*/
/*			 InstrumentID		合约号              					*/
/*  out																	*/
/************************************************************************/
int	QdpShmClient::QdpShmLoadByInstrumentID(CFTDDepthMarketDataField *filed,const char *InstrumentID)
{

	unsigned int key=0;

	CInstrumentDataMap::iterator iter=CInstrumentIDPos->find(InstrumentID);

	if (iter !=CInstrumentIDPos->end())
	{

		key=iter->second;

	}else
	{

		key=QdpHashKey(InstrumentID);

	}

	QdpShmRead(filed,key);

	if ( iter == CInstrumentIDPos->end() )
	{

		CInstrumentIDPos->insert(CInstrumentDataMap::value_type(InstrumentID,key));
	}
	
	return 0;
}

/************************************************************************/
/*  QdpShmRead    单条读取共享内存内容							        */
/*  in		CFTDDepthMarketDataField *filed   返回载体					*/
/*										             					*/
/*  out																	*/
/************************************************************************/
int QdpShmClient::QdpShmRead(CFTDDepthMarketDataField *filed)
{

	void *addnew=(char *)m_pShm+sizeof(ConnectStruct);

	CFTDDepthMarketDataField *newfiled=(CFTDDepthMarketDataField *)addnew;

	CopyDepthMarketDataEntity(filed,newfiled);
	
	return 0;
}

/************************************************************************/
/*  QdpShmRead    单条读取共享内存内容	通过key位置				        */
/*  in		CFTDDepthMarketDataField *filed   返回载体					*/
/*			unsigned int key	共享内存位置	      					*/
/*  out																	*/
/************************************************************************/
int QdpShmClient::QdpShmRead(CFTDDepthMarketDataField *filed,unsigned int key)
{

	struct ConnectStruct * pConnectAddr = (struct ConnectStruct*)m_pShm;

	void *addnew=(char *)m_pShm+sizeof(ConnectStruct)+(key+1)*sizeof(CFTDDepthMarketDataField);

	CFTDDepthMarketDataField *newfiled=(CFTDDepthMarketDataField *)addnew;

	CopyDepthMarketDataEntity(filed,newfiled);
	
	return 0;
}