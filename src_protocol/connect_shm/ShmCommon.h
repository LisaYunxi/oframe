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
		int iType;//0 ��ʾserver�ˣ�1��ʾclient��
		int iUpWriteCount;//�ͻ����������д����
		int iUpReadCount;	
		int iDownWriteCount;//��������ͻ���д����
		int iDownReadCount;
};

/**CShmCommon��һ������Shm���������Ӷ���
*/
class  CShmCommon 
{
public:
	/**���캯����ʹ��ָ����socket�������˶���
	* @param id	ָ����socket
	*/
	CShmCommon();
	
	/**��������
	*/
	virtual ~CShmCommon(void);
	


    /** ��ȡ���صĵ�ַIP
    * @return   ���ص�ַ
    * lidp 20140716
    */

	
protected:
	DWORD m_dwInAddr; // �Զ�IP
	DWORD m_dwLocalAddr; // ����IP
  
};

#endif // !defined(AFX_ShmCHANNEL_H__FE5CED66_6EC3_11D7_BB64_444553540000__INCLUDED_)
