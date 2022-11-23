#ifndef _FTDCPACKAGE_H__
#define _FTDCPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 
#include "FieldSet.h"
#include "Package.h"

const unsigned char FTDC_CHAIN_CONTINUE = 'C';		//报文链的中间FTDC报文。
const unsigned char FTDC_CHAIN_LAST = 'L';			//报文链的最后一个FTDC报文。

#define FTD_VERSION 1

//注意4字节对齐和8字节对齐字段不会被拆分

struct	TFTDCHeader
{
	unsigned char	Version;			// 版本号:放在第一个用于区分XTP报文
	unsigned char	Chain;				// 报文链
	char	Remark[14];					// 自定义字段,存放错误信息，可以临时借用SubIndexName
	char	SubIndexName[48];			// 关联投资者
	UF_INT8	SequenceNo;					// 序列号
	UF_INT8	BusinessNo;					// 业务员序号
	unsigned int	MessageNo;			// 信息正文号
	unsigned int	RequestNo;			// 请求编号(由发送请求者维护，应答中会带回)
	unsigned int	Length;				// 信息正文长度
	unsigned int	SubjectNo;			// 序列类别号
//	unsigned short	FieldCount;			// 数据域数量
		
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

	//创建一个FTDC包，并分配空间。空间大小为保留空间＋FTDC最大报文件长度
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