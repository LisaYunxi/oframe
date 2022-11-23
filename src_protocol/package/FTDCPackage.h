#ifndef _FTDCPACKAGE_H__
#define _FTDCPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 
#include "FieldSet.h"
#include "Package.h"

const unsigned char FTDC_CHAIN_CONTINUE = 'C';		//���������м�FTDC���ġ�
const unsigned char FTDC_CHAIN_LAST = 'L';			//�����������һ��FTDC���ġ�

#define FTD_VERSION 1

//ע��4�ֽڶ����8�ֽڶ����ֶβ��ᱻ���

struct	TFTDCHeader
{
	unsigned char	Version;			// �汾��:���ڵ�һ����������XTP����
	unsigned char	Chain;				// ������
	char	Remark[14];					// �Զ����ֶ�,��Ŵ�����Ϣ��������ʱ����SubIndexName
	char	SubIndexName[48];			// ����Ͷ����
	UF_INT8	SequenceNo;					// ���к�
	UF_INT8	BusinessNo;					// ҵ��Ա���
	unsigned int	MessageNo;			// ��Ϣ���ĺ�
	unsigned int	RequestNo;			// ������(�ɷ���������ά����Ӧ���л����)
	unsigned int	Length;				// ��Ϣ���ĳ���
	unsigned int	SubjectNo;			// ��������
//	unsigned short	FieldCount;			// ����������
		
	void Init()
	{
		memset(this, 0, sizeof(TFTDCHeader));
	}

	void ChangeEndian()
	{
		CHANGE_ENDIAN(SubjectNo);
		CHANGE_ENDIAN(MessageNo);
		CHANGE_ENDIAN(SequenceNo);
		CHANGE_ENDIAN(BusinessNo);
//		CHANGE_ENDIAN(FieldCount);
		CHANGE_ENDIAN(Length);
		CHANGE_ENDIAN(RequestNo);
	}

	void ToStream(char *pStream)
	{
		memcpy(pStream, this, sizeof(TFTDCHeader));
		((TFTDCHeader *)pStream)->ChangeEndian();
	}
	void FromStream(char *pStream)
	{
		memcpy(this, pStream, sizeof(TFTDCHeader));
		ChangeEndian();
	}
};

const int  FTDCHLEN = sizeof(TFTDCHeader);

class  CFTDCPackage : public CFieldSet
{
public:
	CFTDCPackage();
	virtual ~CFTDCPackage();

	//virtual unsigned int GetActiveID();

	virtual int ValidPackage();

	virtual int MakePackage();

	inline TFTDCHeader *GetFTDCHeader();

	//inline unsigned int GetTID();

	//inline void SetChain(unsigned char chain);

	//inline unsigned char GetChain();

	//inline void SetRequestId(unsigned int requestId);

	//inline unsigned int GetRequestId();

	//inline unsigned int GetSequenceSeries();

	//inline UF_INT8 GetVersion();

	void PreparePackage(unsigned int tid, unsigned char chain);

	void PrepareResponse(CFTDCPackage *pRequest, unsigned int tid, unsigned char chain);

	bool OutputHeader(char* header);

	//����һ��FTDC����������ռ䡣�ռ��СΪ�����ռ䣫FTDC����ļ�����
	static CFTDCPackage *CreatePackage(int nHeaderReserve);

protected:
	TFTDCHeader m_FTDCHeader;
};

inline TFTDCHeader *CFTDCPackage::GetFTDCHeader()
{
	return &m_FTDCHeader;
}

//inline unsigned int CFTDCPackage::GetTID()
//{
//	return m_FTDCHeader.MessageNo;
//}
//
//inline void CFTDCPackage::SetChain(unsigned char chain)
//{
//	m_FTDCHeader.Chain = chain;
//}
//
//inline unsigned char CFTDCPackage::GetChain()
//{
//	return m_FTDCHeader.Chain;
//}
//
//inline void CFTDCPackage::SetRequestId(unsigned int requestId)
//{
//	m_FTDCHeader.RequestNo = requestId;
//}
//
//inline unsigned int CFTDCPackage::GetRequestId()
//{
//	return m_FTDCHeader.RequestNo;
//}
//
//inline unsigned int CFTDCPackage::GetSequenceSeries()
//{
//	return m_FTDCHeader.SubjectNo;
//}
//
//inline UF_INT8 CFTDCPackage::GetVersion()
//{
//	return m_FTDCHeader.Version;
//}

#endif