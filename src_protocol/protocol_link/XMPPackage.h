#ifndef AFX_XMPPACKAGE_H__
#define AFX_XMPPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Package.h"
#include "FieldDescribe.h"

#define XMP_PACKAGE_MAX_SIZE	4096
#define XMP_EXTHEAD_MAX_SIZE	127

#define XMPTagNone				0x00
#define XMPTagDatetime			0x01
#define XMPTagCompressMethod	0x02
#define XMPTagTransactionId		0x03
#define XMPTagSessionState		0x04
#define XMPTagKeepAlive			0x05
#define XMPTagTradedate			0x06
#define XMPTagHeartbeatTimeOut	0x07

/**XMP��ͷ
*/
struct TXMPHeader
{
	unsigned char Type;			/**< ��ʾ�ϲ�Э���Э��ID */	
	unsigned char ExtensionLen;	/**< ��չ���ĳ��� */
	unsigned short Length;		/**< ���峤�� */
	
	inline void Init();
	inline void ChangeEndian();
	inline void ToStream(char *pStream);
	inline void FromStream(char *pStream);
};

inline void TXMPHeader::Init()
{
	memset(this, 0 ,sizeof(TXMPHeader));
}

inline void TXMPHeader::ChangeEndian()
{
	CHANGE_ENDIAN(Length);
}

inline void TXMPHeader::ToStream(char *pStream)
{
	memcpy(pStream, this, sizeof(TXMPHeader));
	((TXMPHeader *)pStream)->ChangeEndian();
}

inline void TXMPHeader::FromStream(char *pStream)
{
	memcpy(this, pStream, sizeof(TXMPHeader));
	ChangeEndian();
}

struct TXMPExtHeader
{
	unsigned char Tag;		/**< ״̬��ʾ��*/
	unsigned char TagLen;	/**< ��չ���ĳ��� */
	unsigned char Data[XMP_EXTHEAD_MAX_SIZE];	/**< ��չ������ */
	inline void Init();
};

inline void TXMPExtHeader::Init()
{
	memset(this, 0 ,sizeof(TXMPExtHeader));
}

//#pragma pack(4)

const int XMPHLEN = sizeof(TXMPHeader);
const int XMPEXTHLEN = 2;

//������XMPProtocolЭ�����ʶ���Package
class  CXMPPackage : public CPackage  
{
public:
	/**���캯��
	*/
	CXMPPackage();
	
	/**��������
	*/
	virtual ~CXMPPackage();
	
	/**��ð����ϲ�Э��ID������·��
	* @return ���ذ����ϲ�Э��ID
	*/
	virtual unsigned int GetActiveID();
	
	/**����Ƿ�XMP���ģ���������XMP��ͷ
	* @return PROTERR_XMP_BADPACKAGE һ������XMP����
	* @return >0 ������XMP��
	* @return 0  ���ֵ�XMP��
	*/
	virtual int ValidPackage();
	
	/**��װ���ݰ�
	* @return 0 ��װ�ɹ�
	* @return <0 ��װʧ��
	*/
	virtual int MakePackage();

	inline TXMPHeader *GetXMPHeader();
	/**��ð���ĳ��ȣ�����XTP��ͷ
	* @return ����ĳ��ȣ�����XTP��ͷ
	*/
	inline int GetBodyLength();
	
	/**�����չ��ͷ�ĳ���
	* @return ��չ��ͷ�ĳ���
	*/
	inline unsigned char GetExtensionLen();
	
	/**�����չ��ͷ�ı�ʾ
	* @return ��չ��ͷ�ı�ʾ
	*/
	inline unsigned char GetExtTag();
	
	/**�����չ��ͷ���ݵĳ���
	* @return ��չ��ͷ���ݵĳ���
	*/
	inline unsigned char GetExtTagLen();

	/**�����չ��ͷ������
	*@return ��չ��ͷ������
	*/
	inline unsigned char *GetExtData();

	/**������չ��ͷ����
	*@param Tag ��չ��ͷ�ı�ʾ
	*@param TagLen ��չ��ͷ���ݵĳ���
	*@param pData ��չ��ͷ������
	*/
	void SetExtHeader(unsigned char Tag, unsigned char TagLen, unsigned char *pData );
protected:
	TXMPHeader m_XMPHeader;		/**< XMP��ͷ */
	TXMPExtHeader m_ExtHeader;	/**< XMP��չ��ͷ */
};

inline TXMPHeader *CXMPPackage::GetXMPHeader()
{
	return &m_XMPHeader;
}

inline int CXMPPackage::GetBodyLength()
{
	return m_XMPHeader.Length;
}

inline unsigned char CXMPPackage::GetExtensionLen()
{
	return m_XMPHeader.ExtensionLen;
}

inline unsigned char CXMPPackage::GetExtTag()
{
	return m_ExtHeader.Tag;
}

inline unsigned char CXMPPackage::GetExtTagLen()
{
	return m_ExtHeader.TagLen;
}

inline unsigned char *CXMPPackage::GetExtData()
{
	return m_ExtHeader.Data;
}

#endif
