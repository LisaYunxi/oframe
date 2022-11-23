#ifndef AFX_SCPPACKAGE_H__
#define AFX_SCPPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Package.h"
#include "FieldSet.h"

#define SCP_PACKAGE_MAX_SIZE	4096

/**SCP包体内每个filed的域头
*/
struct TSCPFieldHeader
{
	unsigned char FieldNo;	/**< 域标示符 */
	unsigned char Size;	/**< 域的长度 */
};

/**SCTP报文头结构
*/
struct	TSCPHeader
{
	unsigned short	Length;		/**< 除报头之外，各field长度和 */
	unsigned short	TNo;				/**< SCP报的id	*/

	inline void ToStream(char *pStream);
	inline void FromStream(char *pStream);
};

inline void TSCPHeader::ToStream(char *pStream)
{
#ifndef _BIG_ENDIAN_						//低位在前
	((TSCPHeader *)pStream)->Length = ChangeEndian(Length);
	((TSCPHeader *)pStream)->TNo = ChangeEndian(TNo);
#else
	memcpy(pStream, this, sizeof(TSCPHeader));
#endif
}

inline void TSCPHeader::FromStream(char *pStream)
{
#ifndef _BIG_ENDIAN_						//低位在前
	Length = ChangeEndian(((TSCPHeader *)pStream)->Length);
	TNo = ChangeEndian(((TSCPHeader *)pStream)->TNo);
#else
	memcpy(this, pStream, sizeof(TSCPHeader));
#endif
}

const int SCPHLEN = sizeof(TSCPHeader);

class CSCPPackage : public CFieldSet  
{
public:
	CSCPPackage();
	virtual ~CSCPPackage();
	virtual unsigned int GetActiveID();
	virtual int ValidPackage();
	virtual int MakePackage();

	inline void SetTid(unsigned short tid);
	inline unsigned short GetTid();
protected:
	TSCPHeader m_header;
};

inline void CSCPPackage::SetTid(unsigned short tid)
{
	m_header.TNo = tid;
}

inline unsigned short CSCPPackage::GetTid()
{
	return m_header.TNo;
}

#define SCP_GET_SINGLE_FIELD(pPackage, pField)	((pPackage)->GetSingleField(&((pField)->m_Describe), (pField)))
#define SCP_ADD_FIELD(pPackage, pField) ((pPackage)->AddField(&((pField)->m_Describe), (pField)))
#endif
