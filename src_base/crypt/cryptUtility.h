/////////////////////////////////////////////////////////////////////////
///cryptUtility.h
///	���������ɳ��õļ���ʹ�õĹ���
/////////////////////////////////////////////////////////////////////////

#ifndef CRYPT_UTILITY_H
#define CRYPT_UTILITY_H

#include "hexEncode.h"
#include "md5Handler.h"

///����MD5ժҪ�㷨����ת��Ϊ16���Ʊ��뷽ʽ����
///@param	result	���ڷ��÷���ֵ�Ļ�������������33���ַ��Ŀռ�
///@param	string1	Ҫ����ժҪ�ĵ�1���ַ���
///@param	string2	Ҫ����ժҪ�ĵ�2���ַ���
///@param	string3	Ҫ����ժҪ�ĵ�3���ַ���
///@param	string4	Ҫ����ժҪ�ĵ�4���ַ���
void MD5Hex(char *result, const char *string1, const char *string2=NULL, const char *string3=NULL, const char *string4=NULL);

#endif
