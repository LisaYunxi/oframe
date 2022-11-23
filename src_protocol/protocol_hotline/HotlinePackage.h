#ifndef AFX_HOTLINEPACKAGE_H__
#define AFX_HOTLINEPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Package.h"

#define HTID_LOGIN_REQ						0x0001			//系统同步请求
#define HTID_LOGIN_RSP						0x0002			//系统同步响应
#define HTID_CONFIRM_REQ					0x0003			//排队确认请求
#define HTID_CONFIRM_RSP					0x0004			//排队确认响应
#define HTID_LOGIN_ERROR					0x0005			//系统同步响应


#define HOTLINE_PACKAGE_MAX_SIZE	4500

/**SCTP报文头结构
*/
struct	THotlineHeader
{
	unsigned short	Length;		/**< 除报头之外，各field长度和 */
	unsigned short	TNo;				/**< HotlineProtcol报文的id	*/
	unsigned short	unused;				/**< 只是为了对齐 */
	UF_INT8	SequenceNo;			/**< 序列号 */
	/**改变大端小端
	*/
	inline void ChangeEndian();

	/**将本对象转换成字节流
	* @param pStream 转换出的字节流的缓冲区，必须有足够的长度
	*/
	inline void ToStream(char *pStream);

	/**将字节流转换到本对象中
	* @param pStream 输入的字节流
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

/**心跳线数据包
*/
class CHotlinePackage : public CPackage  
{
public:
	/**构造函数
	*/
	CHotlinePackage();
	
	/**析构函数
	*/
	virtual ~CHotlinePackage();

	/**数据包上传时，上层协议的代码
	* @return 上层协议的代码
	* @remark 不考虑上层协议进一步处理，直接返回0
	*/
	virtual unsigned int GetActiveID();

	/** 检查包内数据是否构成一个完整的包。若有多余的数据，则截掉。
	* @return >=0 是一个完整的包 
	* @return -1 数据包不完整
	* @return <-1 数据包有错误
	*/
	virtual int ValidPackage();

	/**组装数据包
	* @return 0 组装成功
	* @return <0 组装失败
	*/
	virtual int MakePackage();
	
	/**获取报文头
	* @return 获得的报文头
	*/
	inline THotlineHeader *GetHeader();

	/**设置报文的Tid
	* @param tid 要设置报文的Tid
	* @remark 通常在发送报文前调用
	*/
	inline void SetTid(unsigned short tid);

	/**获取报文的Tid
	* @return 获得的报文的Tid
	* @remark 在ValidPackage执行后，才能获得收到报文的Tid
	*/
	inline unsigned short GetTid();

	/**设置报文的序列号
	* @param sequenceNo 要设置报文的序列号
	* @remark 通常在发送报文前调用
	*/
	inline void SetSequenceNo(UF_INT8 sequenceNo);

	/**获取报文的序列号
	* @return 获得的报文的序列号
	* @remark 在ValidPackage执行后，才能获得收到报文的序列号
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
