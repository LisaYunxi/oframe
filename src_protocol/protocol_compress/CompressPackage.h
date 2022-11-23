#ifndef AFX_COMPRESSPACKAGE_H__
#define AFX_COMPRESSPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Package.h"

#define CRP_PACKAGE_MAX_SIZE	4096

struct TCRPHeader
{
	unsigned char Type;			/**< 标示上层协议的协议ID */	
	unsigned char Method;		/**< 压缩算法代码 */
	
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
	
	/**数据包上传时，上层协议的编号，子类必须实现此函数
	* @return 上层协议的编号
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

	inline unsigned char GetCompressMethod();

	inline TCRPHeader *GetCRPHeader();

protected:
	TCRPHeader m_header;		/**< CRP报头 */
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
