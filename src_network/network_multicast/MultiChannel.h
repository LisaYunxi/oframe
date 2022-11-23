// MultiChannel.h: interface for the CMultiChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MultiCHANNEL_H__6DE6311B_C987_4246_A61D_53ADA1F3D619__INCLUDED_)
#define AFX_MultiCHANNEL_H__6DE6311B_C987_4246_A61D_53ADA1F3D619__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Channel.h"

struct sockaddr_in;

class CMultiChannel : public CChannel
{
public:
	CMultiChannel(int id, char *pTargetAddress, unsigned short wTargetPort);
	virtual ~CMultiChannel();
			
	void SetMultiCastIP(char *pMultiCastIP,char *pHostIP);
	bool LeaveMultiGroup();

	//返回多播地址
	virtual char *GetRemoteName(void);

protected:
	/**基于TCP的读取实现函数
	* @param number	buffer缓冲区的长度
	* @param buffer	存放缓冲区
	* @return >0 实际读取得长度
	* @return 0  读取时遭中断返回
	* @return -1 连结断开
	*/
	virtual int ReadImp(int number, char *buffer);


	/**基于TCP的写出实现函数
	* @param number	准备发送的长度
	* @param buffer	存放缓冲区
	* @return >0 实际发送得长度
	* @return 0  读取时遭中断返回
	* @return -1 发送失败
	*/
	virtual int WriteImp(int number, char *buffer);

	
	/**基于TCP的判断可读性实现函数
	* @return true  可读
	* @return false 不可读
	*/
	virtual bool AvailableImp(void);
	
	/**基于TCP的断开连接实现函数
	* @return true  断开成功
	* @return false 断开失败
	*/
	virtual bool DisconnectImp(void);
	
	/**基于TCP的检查连接性实现函数
	* @return true  连接正常
	* @return false 连接断开
	*/
	virtual bool CheckConnectionImp(void);


private:
	sockaddr_in *m_pTargetAddr; //服务器地址结构
	char *m_MultiCastIP;	/*存放多播地址*/
	char *m_sHostIP;
	char *m_sMultiCastName;
	unsigned short m_nPort;
};

inline char *CMultiChannel::GetRemoteName()
{
	return m_sMultiCastName;
}

#endif // !defined(AFX_MultiCHANNEL_H__6DE6311B_C987_4246_A61D_53ADA1F3D619__INCLUDED_)
