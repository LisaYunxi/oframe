// ArbPackage.h: interface for the CGuardPackage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARBPACKAGE_H__3460066F_023C_48D4_AF3A_28997AD17340__INCLUDED_)
#define AFX_ARBPACKAGE_H__3460066F_023C_48D4_AF3A_28997AD17340__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Package.h"

enum TCompStateID
{
	CS_INIT,
	CS_SINGLE,
	CS_PRIMARY,
	CS_SECONDARY,
	CS_PREPARE,
	CS_DOWN
};

#define HTID_STATE_COMMAND				0x0006			//运行状态命令
#define HTID_STATE_REPORT				0x0007			//运行状态报告
#define HTID_EVENT_NOTIFY				0x0008			//事件通知

#define ARB_EVENT_PREPARE_OK			0x0001			//排队机准备完毕
#define ARB_EVENT_HOTLINE_OK			0x0002			//心跳线连接成功
#define ARB_EVENT_HOTLINE_FAIL			0x0003			//心跳线中断

#define ARB_PACKAGE_MAX_SIZE	0

/**SCTP报文头结构
*/
struct	TArbHeader
{
	unsigned short	Length;		/**< 除报头之外，各field长度和 */
	unsigned short	TNo;				/**< SCP报的id	*/
	unsigned int	StateID;			/**< 排队机状态 */
	inline void ToStream(char *pStream);
	inline void FromStream(char *pStream);
};

inline void TArbHeader::ToStream(char *pStream)
{
#ifndef _BIG_ENDIAN_						//低位在前
	((TArbHeader *)pStream)->Length = ChangeEndian(Length);
	((TArbHeader *)pStream)->TNo = ChangeEndian(TNo);
	((TArbHeader *)pStream)->StateID = ChangeEndian(StateID);
#else
	memcpy(pStream, this, sizeof(TArbHeader));
#endif
}

inline void TArbHeader::FromStream(char *pStream)
{
#ifndef _BIG_ENDIAN_						//低位在前
	Length = ChangeEndian(((TArbHeader *)pStream)->Length);
	TNo = ChangeEndian(((TArbHeader *)pStream)->TNo);
	StateID = ChangeEndian(((TArbHeader *)pStream)->StateID);
#else
	memcpy(this, pStream, sizeof(TArbHeader));
#endif
}

const int ARBHLEN = sizeof(TArbHeader);


class CGuardPackage : public CPackage  
{
public:
	CGuardPackage();
	virtual ~CGuardPackage();
	virtual unsigned int GetActiveID();
	virtual int ValidPackage();
	virtual int MakePackage();

	inline TArbHeader *GetHeader();
	inline void SetTid(unsigned short tid);
	inline unsigned short GetTid();
	inline void SetStateID(unsigned int stateID);
	inline unsigned int GetStateID();
protected:
	TArbHeader m_header;
};

inline TArbHeader *CGuardPackage::GetHeader()
{
	return &m_header;
}

inline void CGuardPackage::SetTid(unsigned short tid)
{
	m_header.TNo = tid;
}

inline unsigned short CGuardPackage::GetTid()
{
	return m_header.TNo;
}

inline void CGuardPackage::SetStateID(unsigned int stateID)
{
	m_header.StateID = stateID;
}

inline unsigned int CGuardPackage::GetStateID()
{
	return m_header.StateID;
}

#endif // !defined(AFX_ARBPACKAGE_H__3460066F_023C_48D4_AF3A_28997AD17340__INCLUDED_)
