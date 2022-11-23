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
/*  QdpHashKey	����hashֵȡ��                                          */
/*  in		const char *key											    */
/*																		*/
/*  out																    */
/************************************************************************/
unsigned int QdpShmServer::QdpHashKey(const char *key)
{
	//ֵ����
		
	unsigned int  jskey =  BKDRHash(key);

	unsigned int  modkey = jskey%999999;

	//printf("modkey=%d\n",modkey);
	
	return modkey;

}

/************************************************************************/
/*  BKDRHash hash�㷨                                                   */
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
/*  QdpDataSave		��������                                            */
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

		//���浱ǰ��Լ�ŵ�λ��
		SInstrumentIDPos->insert(CInstrumentDataMap::value_type(filed->InstrumentID,key));
	
	}

	//�̶����򱣴�
	QdpShmWrite(filed,key);
	
	//����λ�ñ���
	QdpShmWrite(filed);

	return 0;
}

/************************************************************************/
/*  QdpShmWrite	�������򱣴�����                                        */
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
/*  QdpShmSet	�̶�keyλ�ô洢                                          */
/*  in		QdpShmWrite												    */
/*																		*/
/*  out																    */
/************************************************************************/
int QdpShmServer::QdpShmWrite(CFTDDepthMarketDataField *filed,unsigned int key)
{
	
	struct ConnectStruct * pConnectAddr = (struct ConnectStruct*)m_baseshmadr;

	CFTDDepthMarketDataField *newfiled=new CFTDDepthMarketDataField();
	
	//���ݷ����keyֵȡ���׵�ַ
	void *addnew=(char *)m_baseshmadr+sizeof(ConnectStruct)+(key+1)*sizeof(CFTDDepthMarketDataField);

	newfiled=(CFTDDepthMarketDataField *)addnew;

	//��ͻ��⣬���hash�����˳�ͻ����ӡ
	if ( (1 == count ) && (0 != strcmp(newfiled->InstrumentID,filed->InstrumentID)) &&  (0 != strcmp(newfiled->InstrumentID,"")))
	{

		int kk=QdpHashKey(newfiled->InstrumentID);

		printf("hash  �г�ͻ,newfiled->InstrumentID=%s,filed->InstrumentID=%s,key1=%d,key2=%d\n",newfiled->InstrumentID.getValue(),filed->InstrumentID.getValue(),kk,key);

	}

	CopyDepthMarketDataEntity(newfiled,filed);

	return 0;
}