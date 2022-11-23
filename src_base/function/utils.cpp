/********************************************************
 * �ļ����ƣ�utils.cpp
 * �ļ���ʶ�����ú�����
 *********************************************************/
#include "utils.h"
#include "platform.h"
#ifdef WIN32
#include <windows.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#endif
#define _S_IFDIR        0040000         /* directory */
 /***�ַ�������
 * @param			Ŀ�괮.
 * @param			Դ��.
 * @param	size_t	Ŀ��ߴ�.
 * @return	char *	Ŀ�괮ָ��
 * ����ͬstrncpy, ����֤��NULL��β��
 */
char *StrCopy(char *dest, const char *src, int size)
{
	if (size > 0)
	{
		int count = size - 1;
		strncpy(dest, src, count);
		dest[count] = 0;
	}
	return dest;
}

/***�ü��ַ���ǰ��ո�
* @param			�ַ���.
* @return	char	Ŀ�괮ָ��
*/
char *TrimAll(char *sz)
{
	if (*sz != 0)
	{
		char *p1 = sz;
		while (*p1 == SPACE_MARK)
			//while ((unsigned)*p1 <= SPACE_MARK)
		{
			p1++;
		}
		if (*p1 == 0)
		{
			*sz = 0;
		}
		else
		{
			char *p2 = (sz + STRLEN(sz) - 1);
			while (*p2 == SPACE_MARK)
				//while ((unsigned)*p2 <= SPACE_MARK)
			{
				p2--;
			}
			int len = (int)(p2 - p1 + 1);
			if (p1 != sz)
			{
				memcpy(sz, p1, len);
			}
			sz[len] = 0;
		}
	}
	return sz;
}

#define WIN_DELIMITER '\\'
#define NIX_DELIMITER '/'
#ifdef WIN32
#define DELIMITER WIN_DELIMITER
#else
#define DELIMITER NIX_DELIMITER
#endif

/***�����ļ����ĺ�׺
* @param			��Ҫ�������ļ���.
* @param			�µĺ�׺������Ҫ������".txt".
* @return	char *  Ŀ�괮ָ��
* ����ļ���û�к�׺����ֱ�ӼӺ�׺
*/
char *ChangeFileExt(char *fname, const char *ext)
{
	char *p = 0;
	for (char *q = fname; *q; q++)
	{
		switch (*q)
		{
		case '.':
			p = q;
			break;
		case WIN_DELIMITER:
		case NIX_DELIMITER:
			p = 0;
		}
	}
	if (p) *p = 0;
	strcat(fname, ext);
	return fname;
}

/***ȡ�ļ���·��
* @param			Ŀ�Ĵ�.
* @param			Դ��.
* @return	char *  Ŀ�괮ָ��
* ȡ����Ŀ�괮��'\'(win)��'/'(unix)��β
* ���Դ��������·����Ϣ������Ϊ��
*/
char *ExtractFilePath(char *path, const char *fname)
{
	const char *p = 0;
	for (const char *q = fname; *q; q++)
	{
		switch (*q)
		{
		case WIN_DELIMITER:
		case NIX_DELIMITER:
			p = q;
		}
	}
	if (p)
	{
		int len = (int)(p - fname + 1);
		memcpy(path, fname, len);
		path[len] = 0;
	}
	else
	{
		*path = 0;
	}
	return path;
}

/***ȡ�ļ���
* @param			Ŀ�Ĵ�.
* @param			Դ��.
* @return	char *  Ŀ�괮ָ��
* ���Դ���������ļ���������Ϊ��
*/
char *ExtractFileName(char *fname, const char *path)
{
	const char *p = 0;
	for (const char *q = path; *q; q++)
	{
		switch (*q)
		{
		case WIN_DELIMITER:
		case NIX_DELIMITER:
			p = q;
		}
	}
	if (p)
	{
		strcpy(fname, p + 1);
	}
	else
	{
		strcpy(fname, path);
	}
	return fname;
}

/***����·������β�ָ��
* @param			·����.
* @return	char *  ·����ָ��
* ���ԭ���Ѿ�����β�ָ�������ޱ仯
*/
char *IncludeDelimiter(char *fname)
{
	int len = (int)strlen(fname);
	if (len)
	{
		switch (fname[len - 1])
		{
		case WIN_DELIMITER:
		case NIX_DELIMITER:
			return fname;
		}
	}
	fname[len] = DELIMITER;
	fname[len + 1] = 0;
	return fname;
}

/***ȥ��·������β�ָ��
* @param			·����.
* @return	char *  ·����ָ��
* ���ԭ��������β�ָ�������ޱ仯
*/
char *ExcludeDelimiter(char *fname)
{
	int len = (int)strlen(fname);
	if (len)
	{
		switch (fname[len - 1])
		{
		case WIN_DELIMITER:
		case NIX_DELIMITER:
			fname[len - 1] = 0;
		}
	}
	return fname;
}

/**�ж��ļ��Ƿ����
* @param			�ļ���.
* @return	bool	true���ڣ�false������
*/
bool FileExists(const char *fname)
{
	bool ret = false;
	FILE *hnd;
	if (hnd = fopen(fname, "r"))
	{
		fclose(hnd);
		ret = true;
	}
	return ret;
}

/**�ж�·���Ƿ����
* @param			·����.
* @return	bool	true���ڣ�false������
* Դ�����Էָ����β
*/
bool PathExists(const char *path)
{
	bool ret = false;
#ifdef WIN32
	int code = ::GetFileAttributes(path);
	if (code != -1)
	{
		ret = ((FILE_ATTRIBUTE_DIRECTORY&code) != 0);
	}
#else
	struct stat st;
	if (stat(path, &st) == 0)
	{
		ret = ((st.st_mode&_S_IFDIR) != 0);
	}
#endif
	return ret;
}

/**����·��
* @param			·����.
* @return	bool	true�ɹ���falseʧ��
* Windowsϵͳ����ȫ·����
*/
bool ForcePath(const char *path)
{
	if (*path == 0)
	{
		return false;
	}

	bool ret = true;
	char dir[1024];
	strcpy(dir, path);
	ExcludeDelimiter(dir);
#ifdef WIN32
	if (strlen(dir) >= 3 && !PathExists(dir))
	{
		char sz[1024];
		ExtractFilePath(sz, dir);
		if (strcmp(sz, dir) != 0)
		{
			ret = ForcePath(sz) && ::CreateDirectory(dir, NULL);
		}
	}
#else
	if (dir[0] != 0 && !PathExists(dir))
	{
		char sz[1024];
		ExtractFilePath(sz, dir);
		if (ForcePath(sz))
		{
			sprintf(sz, "mkdir %s", dir);
			::system(sz);
			ret = PathExists(dir);
		}
		else
		{
			ret = false;
		}
	}
#endif
	return ret;
}

//�ַ������ܡ�����

static char MMEnum[64] =
{ '!','$','0','1','2','3','4','5','6','7','8','9','A','B','C','D',
'E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T',
'U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j',
'k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };

static char MMReve[128];

/**��ʼ������
* ȫ�ֵ���һ��
*/
void MMInit()
{
	srand((unsigned)time(NULL));
	memset(MMReve, 0, sizeof(MMReve));
	for (int i = 0; i < 64; i++)
	{
		MMReve[MMEnum[i]] = i;
	}
}

//ȡ�����
int Random(int r)
{
	int ret = 0;
	if (r != 0)
	{
		ret = rand() % r;
	}
	return ret;
}

//���ֽ�
void One2(char *cc, char c)
{
	*cc = MMEnum[((Random(4) << 4) | (c >> 4) & 0x0f) & 0xff];
	cc++;
	*cc = MMEnum[((Random(4) << 4) | (c & 0x0f)) & 0xff];
}

//�ϲ��ֽ�
char Two1(char c1, char c2)
{
	return ((MMReve[c1] << 4) | (MMReve[c2] & 0x0f)) & 0xff;
}

/**����һ���ַ���
* ԭ�����Ȳ�����255�ֽ�
* Ŀ�곤�Ȳ�С��512�ֽ�
*/
char *EncodeStr(char *dest, const char *src)
{
	int len = (int)strlen(src);
	if (len > 255)
	{
		len = 255;
	}
	One2(dest, len);
	dest += 2;
	int i;
	for (i = 0; i < len; i++, dest += 2)
	{
		One2(dest, src[i]);
	}
	for (int j = (i + 1) * 2; j < 30; j++, dest++)
	{
		*dest = MMEnum[Random(64)];
	}
	*dest = 0;
	return dest;
}

/**����һ���ַ���
* Ŀ�곤�Ȳ�С��256�ֽ�
*/
char *DecodeStr(char *dest, const char *src)
{
	if (strlen(src) >= 4)
	{
		int len = Two1(src[0], src[1]);
		for (int i = 0; i < len; i++, dest++)
		{
			*dest = Two1(src[i * 2 + 2], src[i * 2 + 3]);
		}
	}
	*dest = 0;
	return dest;
}

//************************************
// Method: strrpl
// FullName: strrpl
// Access: public 
// Returns: void
// Qualifier: �ַ����滻����,���滻���е�Ҫ�滻���ַ���,���滻���ַ������滻���ַ�����һ��һ����.
// Parameter: char * pDstOut,����ַ���,Ҫ��֤�㹻�Ŀռ���Դ洢�滻����ַ���.
// Parameter: char * pSrcIn,�����ַ���.
// Parameter: char * pSrcRpl,���滻���ַ���.
// Parameter: char * pDstRpl,�滻����ַ���.
// ע��:���ϵ��ַ�����Ҫ�� '\0 '��β.
//************************************
void strrpl(char* pDstOut, const char* pSrcIn, const char* pSrcRpl, const char* pDstRpl)
{
	char sTemp[256];
	strncpy(sTemp, pSrcIn, sizeof(sTemp));

	char* pi = sTemp;
	char* po = pDstOut;
	int nSrcRplLen = (int)strlen(pSrcRpl);
	int nDstRplLen = (int)strlen(pDstRpl);
	char *p = NULL;
	int nLen = 0;
	do
	{
		// �ҵ���һ���滻��
		p = strstr(pi, pSrcRpl);
		if (p != NULL)
		{
			// ������һ���滻�����һ���滻���м���ַ���
			nLen = (int)(p - pi);
			memcpy(po, pi, nLen);

			// ������Ҫ�滻���ַ���
			memcpy(po + nLen, pDstRpl, nDstRplLen);
		}
		else
		{
			strcpy(po, pi);
			//���û����Ҫ�������ַ���,˵��ѭ��Ӧ�ý���
			break;
		}

		pi = p + nSrcRplLen;
		po = po + nLen + nDstRplLen;

	} while (p != NULL);
}
