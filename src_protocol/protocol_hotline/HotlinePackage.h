#ifndef AFX_HOTLINEPACKAGE_H__
#define AFX_HOTLINEPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Package.h"

#define HTID_LOGIN_REQ						0x0001			//ϵͳͬ������
#define HTID_LOGIN_RSP						0x0002			//ϵͳͬ����Ӧ
#define HTID_CONFIRM_REQ					0x0003			//�Ŷ�ȷ������
#define HTID_CONFIRM_RSP					0x0004			//�Ŷ�ȷ����Ӧ
#define HTID_LOGIN_ERROR					0x0005			//ϵͳͬ����Ӧ


#define HOTLINE_PACKAGE_MAX_SIZE	4500

/**SCTP����ͷ�ṹ
*/
struct	THotlineHeader
{
	unsigned short	Length;		/**< ����ͷ֮�⣬��field���Ⱥ� */
	unsigned short	TNo;				/**< HotlineProtcol���ĵ�id	*/
	unsigned short	unused;				/**< ֻ��Ϊ�˶��� */
	UF_INT8	SequenceNo;			/**< ���к� */
	/**�ı���С��
	*/
	inline void ChangeEndian();

	/**��������ת�����ֽ���
	* @param pStream ת�������ֽ����Ļ��������������㹻�ĳ���
	*/
	inline void ToStream(char *pStream);

	/**���ֽ���ת������������
	* @param pStream ������ֽ���
	*/
	inline void FromStream(char *pStream);
};

inline void THotlineHeader::ChangeEndian()
{
	CHANGE_ENDIAN(Length);
	CHANGE_ENDIAN(TNo);
	CHANGE_ENDIAN(SequenceNo);
}

inline void THotlineHeader::ToStream(char *pStream)
{
	memcpy(pStream, this, sizeof(THotlineHeader));
	((THotlineHeader *)pStream)->ChangeEndian();
}

inline void THotlineHeader::FromStream(char *pStream)
{
	memcpy(this, pStream, sizeof(THotlineHeader));
	ChangeEndian();
}

const int HOTLINEHLEN = sizeof(THotlineHeader);

/**���������ݰ�
*/
class CHotlinePackage : public CPackage  
{
public:
	/**���캯��
	*/
	CHotlinePackage();
	
	/**��������
	*/
	virtual ~CHotlinePackage();

	/**���ݰ��ϴ�ʱ���ϲ�Э��Ĵ���
	* @return �ϲ�Э��Ĵ���
	* @remark �������ϲ�Э���һ������ֱ�ӷ���0
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
	
	/**��ȡ����ͷ
	* @return ��õı���ͷ
	*/
	inline THotlineHeader *GetHeader();

	/**���ñ��ĵ�Tid
	* @param tid Ҫ���ñ��ĵ�Tid
	* @remark ͨ���ڷ��ͱ���ǰ����
	*/
	inline void SetTid(unsigned short tid);

	/**��ȡ���ĵ�Tid
	* @return ��õı��ĵ�Tid
	* @remark ��ValidPackageִ�к󣬲��ܻ���յ����ĵ�Tid
	*/
	inline unsigned short GetTid();

	/**���ñ��ĵ����к�
	* @param sequenceNo Ҫ���ñ��ĵ����к�
	* @remark ͨ���ڷ��ͱ���ǰ����
	*/
	inline void SetSequenceNo(UF_INT8 sequenceNo);

	/**��ȡ���ĵ����к�
	* @return ��õı��ĵ����к�
	* @remark ��ValidPackageִ�к󣬲��ܻ���յ����ĵ����к�
	*/
	inline UF_INT8 GetSequenceNo();
protected:
	THotlineHeader m_header;
};

inline THotlineHeader *CHotlinePackage::GetHeader()
{
	return &m_header;
}

inline void CHotlinePackage::SetTid(unsigned short tid)
{
	m_header.TNo = tid;
}

inline unsigned short CHotlinePackage::GetTid()
{
	return m_header.TNo;
}

inline void CHotlinePackage::SetSequenceNo(UF_INT8 sequenceNo)
{
	m_header.SequenceNo = sequenceNo;
}

inline UF_INT8 CHotlinePackage::GetSequenceNo()
{
	return m_header.SequenceNo;
}

#endif
