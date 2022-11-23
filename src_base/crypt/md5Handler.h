/////////////////////////////////////////////////////////////////////////
///	������md5�㷨�Ľӿ�
/////////////////////////////////////////////////////////////////////////

#ifndef MD5_H
#define MD5_H

#include "platform.h"

///md5ժҪֵ�ó���
#define MD5_DIGEST_LEN	16

typedef unsigned char byte;
typedef unsigned int uint32;

/////////////////////////////////////////////////////////////////////////
///CMD5Handler��һ������MD5�㷨����Ĵ�����
/////////////////////////////////////////////////////////////////////////
class CMD5Handler
{
public:
	///���췽����û�����ó�ʼֵ
	CMD5Handler(void);
	
	///���췽���������ó�ʼֵ
	///@param	input	������ַ���
	///@param	length	�ַ����ĳ��ȣ����Ϊ-1��ʾ�ַ�����'\0'��β
	CMD5Handler(const char *input, int length=-1);
	
	///����һ���ַ���
	///@param	input	������ַ���
	///@param	length	�ַ����ĳ��ȣ����Ϊ-1��ʾ�ַ�����'\0'��β
	void update(const char *input, int length=-1);

	///��ȡժҪֵ
	///@return	�õ���ժҪֵ������һ��ΪMD5_DIGEST_LEN(16)���ֽ�
	const char* digest(void);

	///���ֵ
	void reset(void);

private:
	///������ղ��裬����ժҪֵ
	void final(void);

	///MD5�㷨ʹ�õ��ڲ�����
	void transform(const char block[64]);
	void encode(const uint32 *input, byte *output, size_t length);
	void decode(const byte *input, uint32 *output, size_t length);

	///��ֹ���ƶ�����ķ���
	///@param	v	Ҫ���Ƶ�Դ
	CMD5Handler(const CMD5Handler& v)
	{
	}

	///��ֹ���ڶ�����ķ���
	///@param	v	Ҫ���Ƶ�Դ
	CMD5Handler& operator=(const CMD5Handler& v)
	{
	}

private:
	uint32 _state[4];	// state (ABCD)
	uint32 _count[2];	// number of bits, modulo 2^64 (low-order word first)
	char _buffer[64];	// input buffer
	char _digest[16];	// message digest
	bool _finished;		// calculate finished?
};

#endif
