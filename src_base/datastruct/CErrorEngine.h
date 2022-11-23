/////////////////////////////////////////////////////////////////////////
///CErrorEngine.h
///��������CErrorEngine
/////////////////////////////////////////////////////////////////////////

#ifndef CERRORENGINE_H
#define CERRORENGINE_H

#include "platform.h"
//#include "CBaseObject.h"
//#include "DesignError.h"
#include "public.h"

#define MAX_ERROR_MSG 400

typedef struct
{
	int errorID;
	char *errorMsg;
}	TErrorType;

/////////////////////////////////////////////////////////////////////////
///CErrorEngine��һ������������࣬�ṩ�˴������ͱ���Ļ���
///@author	xuzh
///@version	1.0,20041001
/////////////////////////////////////////////////////////////////////////
class CErrorEngine: public map<int, char *>
{
public:
	///���췽����������������
	CErrorEngine(void);
		
	///�����������������ͷ���ռ�õĿռ�
	virtual ~CErrorEngine(void);
	
	///����һ����������
	///@param	errorID	������
	///@param	errorMsg	������Ϣ
	///@exception	����ô��������Ѿ����ڣ��򱨳�CDesignError
	void registerErrorType(int errorID, char *errorMsg);

	///����һ���������
	///@param	pErrorType	�����������飬���һ����errorIDӦ��Ϊ0
	///@exception	����ô��������Ѿ����ڣ��򱨳�CDesignError
	void registerErrorType(TErrorType pErrorType[]);

	///����һ������
	///@param	errorID	Ҫ����Ĵ�����
	bool reportError(const int errorID, const char *format = NULL, ...);
	
	///��ȡһ������Ŷ�Ӧ�Ĵ�����Ϣ
	///@param	errorID	������
	///@return	��Ӧ�Ĵ�����Ϣ
	char *getErrorMsg(const int errorID);

	///��ȡ����Ĵ���ͬʱ�������
	///@param	pLastErrorNo	��������Ĵ����ţ�0��ʾû�д���
	///@param	pLastErrorMsg	��������Ĵ�����Ϣ
	virtual void getLastError(int *pLastErrorNo=NULL,char **pLastErrorMsg=NULL);

private:
	int m_nLastErrorNo;			//����Ĵ�����
	char *m_pLastErrorMsg;		//����Ĵ�����Ϣ
	char m_sLastErrorMsg[128];	//�����յ��ӿڵ�SubIndexName֮��������ַ����������ĳ��Ⱦ���
};

#endif
