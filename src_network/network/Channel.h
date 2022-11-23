#ifndef AFX_CHANNEL_H__
#define AFX_CHANNEL_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "platform.h"

//������CChannel������
enum TChannelType
{
	CT_STREAM,
	CT_DATAGRAM
};

enum TChannelRecordType
{
	CLRT_CONNECTED,
	CLRT_READ,
	CLRT_WRITE,
	CLRT_READ_ERROR,
	CLRT_WRITE_ERROR,
	CLRT_DISCONNECT,
	CLRT_DISCONNECTED_READ,
	CLRT_DISCONNECTED_WRITE,
};

struct TChannelLogHeader 
{
	unsigned int dwId;					//Channel�ľ��
	unsigned int dwTime;				//�¼�����ʱ��
	unsigned int dwMsTime;				//�¼�����΢��
	unsigned short wType;				//�¼�����
	unsigned short wLength;				//�¼����ݳ���

	void ChangeEndian()
	{
		CHANGE_ENDIAN(dwId);
		CHANGE_ENDIAN(dwTime);
		CHANGE_ENDIAN(dwMsTime);
		CHANGE_ENDIAN(wType);
		CHANGE_ENDIAN(wLength);
	}
};


//CChannel��һ��������������ӣ�ʵ����һ������������ͨѶ�Ļ������ܡ���ͬ���͵�
//����ͨѶ��Ҫ�̳�����࣬��ʵ���������ܡ�

class CChannelLog
{
public:
	virtual UF_INT8 Append(void *pObject, int length) = 0;	
};

class  CChannel  
{
public:
	//���캯�����������ͺ�select�õ��ļ���
	//@param type	��д���ͣ���ֵ������READ_ONLY��WRITE_ONLY����READ_WRITE
	//@param id	select�õ��ļ��š������CChannel���ܱ�select������ֻ���ò�ѯ��ʽ��������Ӧ����Ϊ-1
	CChannel(TChannelType type, int id);
	
	virtual ~CChannel();

	//�ӱ�CChannel�ж�ȡ�������������ҲӦ���������أ������ڴ�˯�ߡ���������ʹ��
	//ReadImp��������Ķ�ȡ�������ڷ�������ʱ�������Ͽ�����
	//@param	number	Ҫ��ȡ���ֽ�����Ӧ������0
	//@param	buffer	��ȡ�Ļ�������ַ
	//@return	�������ֽ��������û�ж����κ����ݣ��򷵻�0��������������Ѿ���·���򷵻�-1
	int Read(int number, char *buffer);
	
	
	//�ӱ�CChannel��д��������޷�д����ҲӦ���������أ������ڴ�˯�ߡ���������ʹ��writeImp���������д���������ڷ�������ʱ�������Ͽ�����
	//@param	number	Ҫд�����ֽ�����Ӧ������0
	//@param	buffer	д���Ļ�������ַ
	//@return	���д�����ֽ��������û��д���κ����ݣ��򷵻�0��������������Ѿ���·���򷵻�-1
	int Write(int number, char *buffer);
	
	//�жϱ�CChannel�Ƿ������ݿɶ�����ʹ���������������ݣ�Ҳ����֤read���Եõ����ݡ�������
	//��Ҫ�Բ�ѯ��ʽ������CChannel�Ƚ����á���������ʹ��availableImp��������ļ�����
	//@return   true��ʾ�����ݣ�false��ʾ������
	bool Available(void);
	
	//�Ͽ������ӡ���������disconnectImp����������ĶϿ����Ӳ���
	//@return	true��ʾ�������ӣ�false��ʾ��ɶϿ�
	bool Disconnect();
	
	//��������Ƿ���Ч����ʹ����������������Ч��Ҳ����֤�ڽ�һ����read����write�Ĳ����в�����
	//�����Ѿ���·����������ʹ��checkConnectionImp����������ļ�����
	// @return true��ʾ������Ч��false��ʾ�Ѿ��Ͽ�
	bool CheckConnection();
	
	//��ȡ�Է��ĵ�ַ��������IP��ַ��
	//@return	�Է��ĵ�ַ
	virtual char *GetRemoteName(void);
	
    //��ȡ���صĵ�ַIP
    //@return   ���ص�ַ
    virtual char *GetLocalName(void);

	//��ȡ�Է��ı�ʶ
	//@return	�Է��ı�ʶ
	//@remark   ����NULL��ʾδ֪
	virtual char *GetRemoteCommonName(void);
	
	//��ȡselect���õ��ļ���
	//@return	�ļ���
	virtual int GetSelectId(void);

	//��ȡ��д�õ��ļ���
	//@return	�ļ���
	inline int GetId(void);

	//��ȡChannel����
	//@return Channel����
	inline TChannelType GetType();

	//������־�ļ�ָ��, ����¼Channnel��Ϣ
	//@param fp ��־�ļ�ָ��, NULL��ʾ������־
	//@remark
	void SetLogFile(FILE *fp);

	//ȡ��־�ļ�ָ��
	//@return ��־�ļ�ָ��, NULL��ʾ������־
	inline FILE * GetLogFile();
	
	//������־��ָ��, ����¼Channnel��Ϣ
	//@param fp ��־�ļ�ָ��, NULL��ʾ������־
	//@remark
	void SetLogFlow(CChannelLog *fp);

	//ȡ��־��ָ��
	//@return ��־�ļ�ָ��, NULL��ʾ������־
	inline CChannelLog * GetLogFlow();

protected:
	//�ӱ�CChannel�ڶ�ȡ��ʵ�ַ������Ǵ��麯�����̳ж�����Ҫʵ�ִ˷���
	//@param	number	Ҫ��ȡ���ֽ�����Ӧ������0
	//@param	buffer	��ȡ�Ļ�������ַ
	//@return	�������ֽ��������û�ж����κ����ݣ��򷵻�0��������������Ѿ���·���������ش����򷵻�-1
	virtual int ReadImp(int number, char *buffer)=0;
	
	//�ӱ�CChannel��д����ʵ�ַ������Ǵ��麯�����̳ж�����Ҫʵ�ִ˷���
	//@param	number	Ҫд�����ֽ�����Ӧ������0
	//@param	buffer	д���Ļ�������ַ
	//@return	���д�����ֽ��������û��д���κ����ݣ��򷵻�0��������������Ѿ���·���������ش����򷵻�-1
	virtual int WriteImp(int number, char *buffer)=0;
	
	//�жϱ�CChannel�Ƿ������ݿɶ���ʵ�ֺ�������ʹ���������������ݣ�Ҳ����֤readImp���Եõ����ݡ�
	//��������Ҫ�Բ�ѯ��ʽ������CChannel�Ƚ����á��������Ǵ��麯�����̳ж�����Ҫʵ�ִ˷���
	// @return 1��ʾ�����ݣ�0��ʾ������
	virtual bool AvailableImp(void)=0;
	
	//�Ͽ������ӵ�ʵ�ֺ������Ǵ��麯�����̳ж�����Ҫʵ�ִ˷���
	//@return true��ʾ�������ӣ�false��ʾ��ɶϿ�
	virtual bool DisconnectImp(void)=0;
	
	//��������Ƿ���Ч����ʹ����������������Ч��Ҳ����֤�ڽ�һ����readImp����writeImp�Ĳ����в�����
	//�����Ѿ���·�����������ش��󡣱������Ǵ��麯�����̳ж�����Ҫʵ�ִ˷���
	// @return	true��ʾ������Ч��false��ʾ�Ѿ��Ͽ�
	virtual bool CheckConnectionImp(void)=0;

	//д��־
	//@param wType ��־��¼����
	//@param wDataLength ��־��¼���ݳ���
	//@param pData ��־��¼��������
	void WriteLog(unsigned short wType, unsigned short wDataLength, const char *pData);

protected:	
	TChannelType m_nType;		//��д����
	bool m_bConnected;			//�Ƿ�������״̬
	int m_nID;					//selectʱ�õ�id
	FILE *m_fpLog;				//��־�ļ�ָ��
	CChannelLog *m_pFlowLog;	//��־�ļ�ָ��
};

inline int CChannel::GetId(void)
{
	return m_nID;
}

inline TChannelType CChannel::GetType()
{
	return m_nType;
}

inline FILE * CChannel::GetLogFile()
{
	return m_fpLog;
}

inline CChannelLog * CChannel::GetLogFlow()
{
	return m_pFlowLog;
}

#endif
