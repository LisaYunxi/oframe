#ifndef AFX_PROTOCOL_H__
#define AFX_PROTOCOL_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Package.h"
#include "Reactor.h"

//#define PNOTIFY_NOHEARTBEAT		0x1001			//读心跳超时
//#define PNOTIFY_ERRHEARTBEAT	0x1002			//发送心跳失败

class CProtocol;

class CProtocolCallback
{
public:
	virtual int HandlePackage(CPackage *pPackage, CProtocol *pProtocol) = 0;
	virtual ~CProtocolCallback() {};
};

//CProtocol是一个抽象的网络协议类，每个协议的实现都需要继承它。
// 一个CProtocol对象可以注册一个下层协议和多个上层协议。
// 通过Pop()函数将数据包的传递到上层协议，通过Push()函数将数据包传递到下层协议。

class  CProtocol : public CProtocolCallback, public CEventHandler
{
public:
	CProtocol(CReactor *pReactor, int nHdrReserveLen, CPackage *pDemuxPackage, CPackage *pSendPackage);
	virtual ~CProtocol();

	//注册下层协议
	//@param pLower 下层协议
	//@param nActiveID 对下层协议而言，本层协议的编号
	virtual void AttachLower(CProtocol *pLower, unsigned int nActiveID);

	//取消下层协议
	//@param pLower 下层协议
	//@param nActiveID 对下层协议而言，本层协议的编号
	virtual void DetachLower(CProtocol *pLower);

	//注册错误处理器
	// @param pErrorHandler 错误处理器
	// @remark 出现错误时，通知该错误处理器进行处理
	inline void RegisterErrorHandler(CEventHandler *pErrorHandler);

	//注册数据包处理器
	//@param pUpperHandler 数据包处理器
	//@remark 如果分发数据包时，找不到合适的上层协议，则将数据包交由
	//@remark 注册的数据包处理器来处理。通常发生在协议栈的最上层。
	inline void RegisterUpperHandler(CProtocolCallback *pUpperHandler);

	//将数据包从本层协议传递到下层协议
	// @param pPackage 本层协议的包
	// @return 0   成功
	// @return <>0 失败
	virtual int Send(CPackage *pPackage, CProtocol *pUpper = NULL);

	//在协议栈中，从底层到本层协议的所有包头保留长度的和
	// @return 从底层到本层协议的所有包头保留长度的和
	inline int GetHdrReserveLen();

	//获到本协议对象的协议编号
	//@return 本协议对象的协议编号
	//@remark 协议编号是针对下层协议而言的，只有注册了下层协议，本函数才有意义
	inline unsigned int GetActiveID();

	//由下层协议或XTPControl容器调用，在本层协议实现包的
	// 检查、处理和继续上传至上层协议。
	// @param pPackage 下层协议传进的包指针
	// @param pProtocol 下层协议
	// @return 0   成功
	// @return <>0 失败
	virtual int HandlePackage(CPackage *pPackage, CProtocol *pProtocol);

protected:

	//注册上层协议
	// @param pUpper 上层协议的协议指针
	void AddUpper(CProtocol *pUpper);

	//根据协议编号，查找上层协议
	// @return 上层协议的指针
	inline CProtocol *GetUpper(unsigned int nActiveID);

	//根据协议编号，将上层协议取消注册
	// @param ActiveID 上层协议的协议ID
	// @return 上层协议的指针
	CProtocol *RemoveUpper(unsigned int nActiveID);

	//将数据包分发到到合适的上层协议
	// @param pPackage 需要分发的数据包
	// @return 0   成功
	// @return <>0 失败
	virtual int Pop(CPackage *pPackage);

	//收到一个错误的数据包时调用
	// @param pPackage 错误的数据包
	// @return 0   成功
	// @return <>0 失败
	virtual int OnRecvErrorPackage(CPackage *pPackage);

	//通知错误处理器
	// @param message 消息ID
	// @param dwParam 双字型参数
	// @param pParam 无符号指针型参数
	inline void NotifyError(int message, unsigned int dwParam, void *pParam);

	//上层协议将包传送到本层协议
	// @param pPackage 要传送的上层协议的包
	// @param pUpper 上层协议
	// @return 0   成功
	// @return <>0 失败
	virtual int Push(CPackage *pPackage, CProtocol *pUpper);

	//为向下层协议传送而组装数据包
	// @param pPackage 要组装的数据包
	// @param pUpper 上层协议
	// @return 0   成功
	// @return <>0 失败
	virtual int MakePackage(CPackage *pPackage, CProtocol *pUpper);
protected:
	CPackage *m_pDemuxPackage;	//多路分解时使用的临时包，防止破坏下层传上来的包
	CPackage *m_pSendPackage;	//下传使用的临时包
	typedef vector<CProtocol *> CProtocolArray;
	CProtocolArray m_lowers;	//下层协议
	CProtocol *m_pUpper;		//上层协议
	CProtocol *m_pBrother;		//同层协议
	int m_nHdrReserveLen;		//在协议栈中，从底层到本层协议的所有包头长度的和
	unsigned int m_nActiveID;			//本协议对在下层协议中的协议编号
	CProtocolCallback *m_pUpperHandler;	// 数据包处理器 
protected:
	CEventHandler *m_pErrorHandler;
};

inline int CProtocol::GetHdrReserveLen()
{
	return m_nHdrReserveLen;
}

inline void CProtocol::RegisterErrorHandler(CEventHandler *pErrorHandler)
{
	m_pErrorHandler = pErrorHandler;
}

inline void CProtocol::RegisterUpperHandler(CProtocolCallback *pUpperHandler)
{
	m_pUpperHandler = pUpperHandler;
}

inline void CProtocol::NotifyError(int message, unsigned int dwParam, void *pParam)
{
	if (m_pErrorHandler != NULL)
	{
		m_pErrorHandler->SendEvent(message, dwParam, pParam);
	}
}

inline unsigned int CProtocol::GetActiveID()
{
	return m_nActiveID;
}

inline CProtocol *CProtocol::GetUpper(unsigned int nActiveID)
{
	CProtocol *pCurr = m_pUpper;
	while (pCurr != NULL)
	{
		if (pCurr->GetActiveID() == nActiveID)
		{
			break;
		}
		pCurr = pCurr->m_pBrother;
	}
	return pCurr;
}

#endif
