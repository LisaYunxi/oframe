#ifndef AFX_CLIENT_H__
#define AFX_CLIENT_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "Channel.h"
#include "ServiceName.h"

//CClientʱһ�������δ������ӵ�ͨѶ�ͻ��ˡ�ʵ����һ������������ͨѶ�Ŀͻ��˻������ܡ�
//��ͬ���͵�����ͨѶ��Ҫ�̳�����࣬��ʵ���������ܡ�
class  CClientBase
{
public:
	CClientBase();
	virtual ~CClientBase();
	
	//����ָ���ķ�����������ϣ�ҲӦ���������أ������ڴ˳�ʱ��˯�ߡ���������
	//ʹ��ConnectImpl������������Ӳ���
	//@param   pName	Ҫ���ӵķ�����
	//@return  ��������ϣ��򷵻����������CChannel��������Ӳ��ϣ��򷵻�NULL������Ӳ��ϣ������ʹ��GetErrorEvent��ȡ�������Ϣ
	virtual CChannel *Connect(CServiceName *pName)=0;
	
	//��ȡ����Ĵ�����Ϣ
	//@return	����Ĵ�����Ϣ���������Ĳ�����óɹ����򷵻�NULL
	inline char *GetErrorMsg();
protected:

	//������������Ĵ�����Ϣ
	//@param  msg  ������Ϣ
	inline void SetErrorMsg(char *msg);
private:
	
	char *m_pErrorMsg;		///�洢��������Ĵ�����Ϣ
};

inline char *CClientBase::GetErrorMsg()
{
	return m_pErrorMsg;
}

inline void CClientBase::SetErrorMsg(char *msg)
{
	m_pErrorMsg=msg;
}

#endif
