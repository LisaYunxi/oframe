#ifndef APIKEYUTIL_H
#define APIKEYUTIL_H

#include "platform.h"

/**����ǩ��
*@param	SecretKey	����ǩ����key
*@param	RequestTimestamp	����ʱ���
*@param	RequestMethod	���󷽷���GET/POST��
*@param	RequestPath	����URL·��
*@param	RequestBody	�����壨GET�������Ϊ���ַ�����
*@return	ǩ������ַ���
*/
string GenerateSign(string SecretKey, string RequestTimestamp, string RequestMethod, string RequestPath, string RequestBody);

/**HMACSHA����
*@param	Key	���ڼ��ܵ�key
*@param	Input	��Ҫ���ܵ�����
*@return	���ܺ���ַ���
*/
string HmacShaEncode(const char *Key, const char *Input);

#endif //APIKEYUTIL_H