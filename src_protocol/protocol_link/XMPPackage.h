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

/**XMP报头
*/
struct TXMPHeader
{
	unsigned char Type;			/**< 标示上层协议的协议ID */	
	unsigned char ExtensionLen;	/**< 扩展报文长度 */
	unsigned short Length;		/**< 包体长度 */
	
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
	unsigned char Tag;		/**< 状态标示　*/
	unsigned char TagLen;	/**< 扩展包的长度 */
	unsigned char Data[XMP_EXTHEAD_MAX_SIZE];	/**< 扩展包包体 */
	inline void Init();
};

inline void TXMPExtHeader::Init()
{
	memset(this, 0 ,sizeof(TXMPExtHeader));
}

//#pragma pack(4)

const int XMPHLEN = sizeof(TXMPHeader);
const int XMPEXTHLEN = 2;

//定义了XMPProtocol协议层所识别的Package
class  CXMPPackage : public CPackage  
{
public:
	/**构造函数
	*/
	CXMPPackage();
	
	/**析构函数
	*/
	virtual ~CXMPPackage();
	
	/**获得包的上层协议ID，用来路由
	* @return 返回包的上层协议ID
	*/
	virtual unsigned int GetActiveID();
	
	/**检查是否XMP报文，并解析出XMP报头
	* @return PROTERR_XMP_BADPACKAGE 一个坏的XMP包文
	* @return >0 完整的XMP包
	* @return 0  部分的XMP包
	*/
	virtual int ValidPackage();
	
	/**组装数据包
	* @return 0 组装成功
	* @return <0 组装失败
	*/
	virtual int MakePackage();

	inline TXMPHeader *GetXMPHeader();
	/**获得包体的长度，包括XTP报头
	* @return 包体的长度，包括XTP报头
	*/
	inline int GetBodyLength();
	
	/**获得扩展报头的长度
	* @return 扩展报头的长度
	*/
	inline unsigned char GetExtensionLen();
	
	/**获得扩展报头的标示
	* @return 扩展报头的标示
	*/
	inline unsigned char GetExtTag();
	
	/**获得扩展报头内容的长度
	* @return 扩展报头内容的长度
	*/
	inline unsigned char GetExtTagLen();

	/**获得扩展报头的内容
	*@return 扩展报头的内容
	*/
	inline unsigned char *GetExtData();

	/**设置扩展报头内容
	*@param Tag 扩展报头的标示
	*@param TagLen 扩展报头内容的长度
	*@param pData 扩展报头的内容
	*/
	void SetExtHeader(unsigned char Tag, unsigned char TagLen, unsigned char *pData );
protected:
	TXMPHeader m_XMPHeader;		/**< XMP报头 */
	TXMPExtHeader m_ExtHeader;	/**< XMP扩展报头 */
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
