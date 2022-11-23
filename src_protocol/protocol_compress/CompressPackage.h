#ifndef AFX_COMPRESSPACKAGE_H__
#define AFX_COMPRESSPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Package.h"

#define CRP_PACKAGE_MAX_SIZE	4096

struct TCRPHeader
{
	unsigned char Type;			/**< ��ʾ�ϲ�Э���Э��ID */	
	unsigned char Method;		/**< ѹ���㷨���� */
	
	inline void Init();
	inline void ToStream(char *pStream);
	inline void FromStream(char *pStream);
};

inline void TCRPHeader::Init()
{
	memset(this, 0 ,sizeof(TCRPHeader));
}

inline void TCRPHeader::ToStream(char *pStream)
{
	memcpy(pStream, this, sizeof(TCRPHeader));
}

inline void TCRPHeader::FromStream(char *pStream)
{
	memcpy(this, pStream, sizeof(TCRPHeader));
}

const int CRPHLEN = sizeof(TCRPHeader);

class CCompressPackage : public CPackage  
{
public:
	CCompressPackage();
	virtual ~CCompressPackage();
	
	/**���ݰ��ϴ�ʱ���ϲ�Э��ı�ţ��������ʵ�ִ˺���
	* @return �ϲ�Э��ı��
	*/
	virtual unsigned int GetActiveID();

	/** �����������Ƿ񹹳�һ�������İ������ж�������ݣ���ص���
	* @return >=0 ��һ�������İ� 
	* @return -1 ���ݰ�������
	* @return <-1 ���ݰ��д���
	*/
	virtual int ValidPackage();

	/**��װ���ݰ�
	* @return 0 ��װ�ɹ�
	* @return <0 ��װʧ��
	*/
	virtual int MakePackage();

	inline unsigned char GetCompressMethod();

	inline TCRPHeader *GetCRPHeader();

protected:
	TCRPHeader m_header;		/**< CRP��ͷ */
};

inline unsigned char CCompressPackage::GetCompressMethod()
{
	return m_header.Method;
}

inline TCRPHeader *CCompressPackage::GetCRPHeader()
{
	return &m_header;
}

#endif
