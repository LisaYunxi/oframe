#ifndef _GW_UTILS_H
#define _GW_UTILS_H
/********************************************************
 * �ļ����ƣ�utils.h
 * �ļ���ʶ�����ú�����
 *********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//�ո�
#define SPACE_MARK ' '

/***�ַ�������
* @param			Ŀ�괮.
* @param			Դ��.
* @param	size_t	Ŀ��ߴ�.
* @return	char *	Ŀ�괮ָ��
* ����ͬstrncpy, ����֤��NULL��β��
*/
char *StrCopy(char *, const char *, int);

/***�ü��ַ���ǰ��ո�
* @param			�ַ���.
* @return	char	Ŀ�괮ָ��
*/
char *TrimAll(char *);

/***�����ļ����ĺ�׺
* @param			��Ҫ�������ļ���.
* @param			�µĺ�׺������Ҫ������".txt".
* @return	char *  Ŀ�괮ָ��
* ����ļ���û�к�׺����ֱ�Ӽ���
*/
char *ChangeFileExt(char *, const char *);

/***ȡ�ļ���·��
* @param			Ŀ�Ĵ�.
* @param			Դ��.
* @return	char *  Ŀ�괮ָ��
* ȡ����Ŀ�괮��'\'(win)��'/'(unix)��β
* ���Դ��������·����Ϣ������Ϊ��
*/
char *ExtractFilePath(char *, const char *);

/***ȡ�ļ���
* @param			Ŀ�Ĵ�.
* @param			Դ��.
* @return	char *  Ŀ�괮ָ��
* ���Դ���������ļ���������Ϊ��
*/
char *ExtractFileName(char *, const char *);

/***����·������β�ָ��
* @param			·����.
* @return	char *  ·����ָ��
* ���ԭ���Ѿ�����β�ָ�������ޱ仯
*/
char *IncludeDelimiter(char *);

/***ȥ��·������β�ָ��
* @param			·����.
* @return	char *  ·����ָ��
* ���ԭ��������β�ָ�������ޱ仯
*/
char *ExcludeDelimiter(char *);

/**�ж��ļ��Ƿ����
* @param			�ļ���.
* @return	bool	true���ڣ�false������
*/
bool FileExists(const char *);

/**�ж�·���Ƿ����
* @param			·����.
* @return	bool	true���ڣ�false������
* Դ�����Էָ����β
*/
bool PathExists(const char *);

/**����·��
* @param			·����.
* @return	bool	true�ɹ���falseʧ��
* Windowsϵͳ����ȫ·����
*/
bool ForcePath(const char *);

/**��ʼ������
* ȫ�ֵ���һ��
*/
void MMInit();

/**����һ���ַ���
* ԭ�����Ȳ�����255�ֽ�
* Ŀ�곤�Ȳ�С��512�ֽ�
*/
char *EncodeStr(char *dest,const char *src);

/**����һ���ַ���
* Ŀ�곤�Ȳ�С��256�ֽ�
*/
char *DecodeStr(char *dest,const char *src);

void strrpl(char* pDstOut, const char* pSrcIn, const char* pSrcRpl, const char* pDstRpl);

#endif
