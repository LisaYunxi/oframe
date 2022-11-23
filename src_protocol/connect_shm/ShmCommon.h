// ShmChannel.h: interface for the CShmCommon class .
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ShmCHANNEL_H__FE5CED66_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_ShmCHANNEL_H__FE5CED66_6EC3_11D7_BB64_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "UFEntityCopy.h"
//#include "FtdData.h"
#ifdef UNIX
#include <sys/sem.h>
#else
#include <Windows.h>
#define key_t int
#endif

//typedef map< CInstrumentIDType,CFTDDepthMarketDataField* > CQdpMarketDataMap;

//typedef map< CInstrumentIDType,unsigned int > CInstrumentDataMap;


struct ConnectStruct{
		int iUsedFlag;
		int iPos;
		int iBuffSize;
		void *pBaseAddr;//
		int iType;//0 表示server端，1表示client端
		int iUpWriteCount;//客户端向服务器写数据
		int iUpReadCount;	
		int iDownWriteCount;//服务器向客户端写数据
		int iDownReadCount;
};

/**CShmCommon是一个基于Shm的网络连接对象
*/
class  CShmCommon 
{
public:
	/**构造函数，使用指定的socket，创建此对象
	* @param id	指定的socket
	*/
	CShmCommon();
	
	/**析构函数
	*/
	virtual ~CShmCommon(void);
	


    /** 获取本地的地址IP
    * @return   本地地址
    * lidp 20140716
    */

	
protected:
	DWORD m_dwInAddr; // 对端IP
	DWORD m_dwLocalAddr; // 本地IP
  
};

#endif // !defined(AFX_ShmCHANNEL_H__FE5CED66_6EC3_11D7_BB64_444553540000__INCLUDED_)
