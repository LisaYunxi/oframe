/////////////////////////////////////////////////////////////////////////
///hexEncode.h
///������16���Ʊ������Ľӿ�
/////////////////////////////////////////////////////////////////////////

#ifndef HEX_ENCODE_H
#define HEX_ENCODE_H

#include "platform.h"

/////////////////////////////////////////////////////////////////////////
///CHexEncoder��һ������16���Ʊ���ı�����
/////////////////////////////////////////////////////////////////////////
class CHexEncoder
{
public:
	///���췽��
	CHexEncoder(void);
	virtual ~CHexEncoder(){};
	
	///���뷽��
	///@param	buffer	�����������Ŀռ�
	///@param	bufferLen	�������ռ�ĳ���
	///@param	input	����Ĵ�������ַ���
	///@param	inputLen	�����ַ����ĳ��ȣ����Ϊ-1��ʾ�ַ�����'\0'��β
	///@return	true��ʾ�ɹ���false��ʾʧ�ܣ�ʧ�ܵ�Ψһԭ�����������Ŀռ䲻��
	virtual bool encode(char *buffer, int bufferLen, const char *input, int inputLen=-1);
	
	///����������ĳ���
	///@param	input	����Ĵ�������ַ���
	///@param	inputLen	�����ַ����ĳ��ȣ����Ϊ-1��ʾ�ַ�����'\0'��β
	///@return	����ĳ���
	virtual int encodeLen(const char *input, int inputLen=-1);
};

#endif
