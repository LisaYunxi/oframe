#ifndef AFX_AGGRPACKAGE_H__
#define AFX_AGGRPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Package.h"

#define AGGR_TYPE_LOGIN				0x0001			//��¼
#define AGGR_TYPE_CARRY				0x0002			//�����ϲ�Э�鱨��

#define AGGR_PACKAGE_MAX_SIZE	0

/**AGGR����ͷ�ṹ
*/
struct	TAggrHeader
{
	unsigned short	Length;		/**< ����ͷ֮�⣬��field���Ⱥ�  */
	unsigned short	Type;				/**< �������� */	
	unsigned int	AggregationID;		/**< �ۼ���� */
	UF_INT8	SequenceNo;			/**< ���к� */
	inline void ChangeEndian();
	inline void ToStream(char *pStream);
	inline void FromStream(char *pStream);
};

const int AGGRHLEN = sizeof(TAggrHeader);

inline void TAggrHeader::ChangeEndian()
{
	CHANGE_ENDIAN(Length);
	CHANGE_ENDIAN(Type);
	CHANGE_ENDIAN(AggregationID);
	CHANGE_ENDIAN(SequenceNo);
}

inline void TAggrHeader::ToStream(char *pStream)
{
	memcpy(pStream, this, sizeof(TAggrHeader));
	((TAggrHeader *)pStream)->ChangeEndian();
}

inline void TAggrHeader::FromStream(char *pStream)
{
	memcpy(this, pStream, sizeof(TAggrHeader));
	ChangeEndian();
}

class CAggrPackage : public CPackage  
{
public:
	CAggrPackage();
	virtual ~CAggrPackage();
	virtual unsigned int GetActiveID();
	virtual int ValidPackage();
	virtual int MakePackage();

	inline TAggrHeader *GetHeader();
	inline void SetType(unsigned short Type);
	inline unsigned short GetType();
	inline void SetAggregationID(unsigned int AggregationID);
	inline unsigned short GetAggregationID();
	inline void SetSequenceNo(UF_INT8 SequenceNo);
	inline UF_INT8 GetSequenceNo();
protected:
	TAggrHeader m_header;
};

inline TAggrHeader *CAggrPackage::GetHeader()
{
	return &m_header;
}

inline void CAggrPackage::SetType(unsigned short Type)
{
	m_header.Type = Type;
}

inline unsigned short CAggrPackage::GetType()
{
	return m_header.Type;
}

inline void CAggrPackage::SetAggregationID(unsigned int AggregationID)
{
	m_header.AggregationID = AggregationID;
}

inline unsigned short CAggrPackage::GetAggregationID()
{
	return m_header.AggregationID;
}

inline void CAggrPackage::SetSequenceNo(UF_INT8 SequenceNo)
{
	m_header.SequenceNo = SequenceNo;
}

inline UF_INT8 CAggrPackage::GetSequenceNo()
{
	return m_header.SequenceNo;
}

#endif