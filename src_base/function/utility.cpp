#include "platform.h"
#include "utility.h"
#include "public.h"

//static FILE *fpDebugOutput=NULL; ///����DEBUG�����֮��,��ΪNULL,����Ϊ��stdout

char* trim(char *str)
{
	if (str == NULL)
		return NULL;
	while (IS_SPACE(*str)) str++;
	int len = STRLEN(str);
	if (len == 0)
		return str;
	char *end = str + len - 1;
	while (IS_SPACE(*end)) end--;
	*(++end) = '\0';
	return str;
}


//char *getLongTime(void)
//{
//	time_t ltime;
//	struct tm *now;
//	static char timeBuffer[21];
//	
//	time(&ltime);
//	now=localtime(&ltime);
//	
//	struct timeb tb;
//	ftime(&tb);
//	sprintf(timeBuffer,"%04d%02d%02d%02d%02d%02d%02d",
//		now->tm_year+1900,
//		now->tm_mon+1,
//		now->tm_mday,
//		now->tm_hour,
//		now->tm_min,
//		now->tm_sec,
//		tb.millitm/10);
//	return timeBuffer;
//}

//char *getShortTime(void)
//{
//	time_t ltime;
//	struct tm *now;
//	static char timeBuffer[21];
//	
//	time(&ltime);
//	now=localtime(&ltime);
//	sprintf(timeBuffer,"%04d%02d%02d%02d%02d%02d",
//		now->tm_year+1900,
//		now->tm_mon+1,
//		now->tm_mday,
//		now->tm_hour,
//		now->tm_min,
//		now->tm_sec);
//	return timeBuffer;
//}
//
//char *getTodayTime(void)
//{
//	time_t ltime;
//	struct tm *now;
//	static char timeBuffer[21];
//	
//	time(&ltime);
//	now=localtime(&ltime);
//
//	sprintf(timeBuffer,"%02d%02d%02d",
//		now->tm_hour,
//		now->tm_min,
//		now->tm_sec);
//	return timeBuffer;
//}

struct tm GetTM(time_t *pTime)
{
	time_t now;
	if (pTime == NULL)
	{
		now = time(NULL);
		pTime = &now;
	}

	struct tm tt;
#ifdef WINDOWS
#if _MSC_VER >= 1700
	localtime_s(&tt, pTime);
#else
	//����
	memcpy(&tt, localtime(pTime), sizeof(tm));
	//����
#endif
#else
	localtime_r(pTime, &tt);
#endif
	return tt;
}

struct tm GetNowDate(char *pszDate)
{
	struct tm tt = GetTM(NULL);
	sprintf(pszDate, "%04d%02d%02d", tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday);
	return tt;
}

struct tm GetSlogDate(char *pszDate)
{
	struct tm tt = GetTM(NULL);
	sprintf(pszDate, "%02d%02d%02d%02d", tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday, tt.tm_hour);
	return tt;
}

/*
int dbgOpen(char *strFileName){
	fpDebugOutput = mfopen(strFileName,"a+b");
	return 0;
}

int dbgPrint(char *strBuffer)
{

	FILE *fp;
	fp = fpDebugOutput==NULL?stdout:fpDebugOutput;
	fprintf(fp,"%s", strBuffer);
	fflush(fp);
	return 0;

}

int dbgClose()
{
	if (fpDebugOutput!=NULL){
		fclose(fpDebugOutput);
		fpDebugOutput=NULL;
	}
	return 0;
}

FILE *getFpDebugOutput()
{
	return  fpDebugOutput==NULL?stdout:fpDebugOutput;

}
*/


int isValidNumber(char *str1)
/*
���ܣ� ��������str�Ƿ��Ƿ������±�׼����ֵ��
	û��С���������
	�����һ��С����ĸ�����
	������TRIM���ܣ�����пո񣬷��ط�
return 0: ��
rerurn 1: ��
*/
{
	char str[100];
	strcpy(str, str1);
	trim(str);
	int length = STRLEN(str);
	int dotnumber = 0, digitnumber = 0;
	///���length==0���ط�
	if (length == 0)
		return 0;
	///���ÿһ���ַ������ֻ���С���㣬������С����ĸ���,��������쳣�ַ����Ҳ��������ţ�ֱ�ӷ��ط�
	for (int i = 0; i < length; i++)
	{
		if (!isdigit(str[i]))
		{
			if (str[i] == '.')
				dotnumber++;
			else
			{
				if ((str[i] == '-' || str[i] == '+') && i == 0)
					;
				else
					return 0;
			}
		}
		else
			digitnumber++;
	}
	///���û�����֣����ط�
	if (digitnumber == 0)
		return 0;
	///���С����ĸ�������һ���ط�
	if (dotnumber > 1)
		return 0;
	///������
	return 1;
}


int isValidDate(char *str1)
/*
���ܣ�����ַ����Ƿ�Ϸ������ڸ�ʽyyyymmdd
	ע���ȷ�ǺϷ������ڣ�����������229�Ŀ��ǣ�
���أ�
	return 0: ��
	rerurn 1: ��

*/
{
	char str[100];
	strcpy(str, str1);
	trim(str);


	int length = STRLEN(str);
	char buffer[10];
	struct tm time_check;
	int year, month, day;
	///����ַ������Ȳ���ȷ��ֱ�ӷ���
	if (length != 8)
		return 0;
	///���ÿһ���ַ�������
	for (int i = 0; i < length; i++)
	{
		if (!isdigit(str[i]))
			return 0;
	}

	///�ж��������ںϷ��ķ�Χ�� 9999 > year > 1900 12 >= month >= 1 31>=day>=1
	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, str, 4);
	year = atoi(buffer);
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = str[4];
	buffer[1] = str[5];
	month = atoi(buffer);
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = str[6];
	buffer[1] = str[7];
	day = atoi(buffer);
	///������Ч
	time_check.tm_year = year - 1900;
	time_check.tm_mon = month - 1;
	time_check.tm_mday = day;
	time_check.tm_hour = 0;
	time_check.tm_min = 0;
	time_check.tm_sec = 1;
	time_check.tm_isdst = -1;
	if (mktime(&time_check) == -1)
		return 0;
	///ת��������Ӧ����ԭ�����ַ���һ��
	memset(buffer, 0, sizeof(buffer));
	strftime(buffer, sizeof(buffer) - 1, "%Y%m%d", &time_check);
	if (strcmp(buffer, str) != 0)
		return 0;
	return 1;
}


int isValidTime(char *str)
/*
���ܣ�����ַ����Ƿ�Ϸ���ʱ���ʽhhmmss
���أ�
	return 0: ��
	rerurn 1: ��
*/
{
	int length = STRLEN(str);
	int result;
	char buffer[10];
	if (length != 6)
		return 0;
	///���ÿһ���ַ�������
	for (int i = 0; i < length; i++)
	{
		if (!isdigit(str[i]))
			return 0;
	}
	///��� 0<=hh<=23 0<=mm<=59 0<=ss<=59
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = str[0];
	buffer[1] = str[1];
	result = atoi(buffer);
	if (!(result >= 0 && result <= 23))
		return 0;
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = str[2];
	buffer[1] = str[3];
	result = atoi(buffer);
	if (!(result >= 0 && result <= 59))
		return 0;
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = str[4];
	buffer[1] = str[5];
	result = atoi(buffer);
	if (!(result >= 0 && result <= 59))
		return 0;
	return 1;
}

bool CheckIpAddressMatch(const char *pIpAddress, const char *pIpMatch, const char *pIpMask)
{
	return CheckIpAddressMatch(inet_addr(pIpAddress), inet_addr(pIpMatch), inet_addr(pIpMask));
}

bool CheckIpAddressMatch(const unsigned long ulIpAddress,
	const unsigned long ulIpMatch,
	const unsigned long ulpMask)
{
	return (ulIpAddress&ulpMask) == (ulIpMatch&ulpMask);
}

bool isRecovery(int argc, char *argv[])
{
	if (!strcmp(argv[argc - 1], "recovery"))
	{
		return true;
	}
	return false;
}
char* StrCCopy(char * dest, const char * source, size_t count)
{
	if (NULL == source)
	{
		if (dest)
		{
			dest[0] = '\0';
			return dest;
		}
		else
		{
			return NULL;
		}
	}

	char *start = dest;
	count--;
	while (count && (*dest++ = *source++))
	{
		count--;
	}

	if (0 == count)
	{
		*dest = '\0';
	}

	return start;
}


#ifdef WIN32
void ChangeByteOrder(PCHAR szString, USHORT uscStrSize)
{
	USHORT  i = 0;
	CHAR    temp = '\0';

	for (i = 0; i < uscStrSize; i += 2)
	{
		temp = szString[i];
		szString[i] = szString[i + 1];
		szString[i + 1] = temp;
	}
}

//--------------------------------------------------------------  
//                      Ӳ�����к�  
//--------------------------------------------------------------  
BOOL GetHDSerial(char *lpszHD, int len/*=128*/)
{
	BOOL        bRtn = FALSE;
	unsigned long       bytesRtn = 0;
	char        szhd[80] = { 0 };
	PIDSECTOR   phdinfo;
	HANDLE      hDrive = NULL;
	GETVERSIONOUTPARAMS vers;
	SENDCMDINPARAMS     in;
	SENDCMDOUTPARAMS    out;

	ZeroMemory(&vers, sizeof(vers));
	ZeroMemory(&in, sizeof(in));
	ZeroMemory(&out, sizeof(out));

	//�����ĸ�����Ӳ�̣�ȡ��һ�������ݵ�����Ӳ��  
	for (int j = 0; j < 4; j++)
	{
		sprintf(szhd, "\\\\.\\PhysicalDrive%d", j);
		hDrive = CreateFileA(szhd,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			0,
			OPEN_EXISTING,
			0,
			0);
		if (NULL == hDrive)
		{
			continue;
		}

		if (!DeviceIoControl(hDrive, DFP_GET_VERSION, 0, 0, &vers, sizeof(vers), &bytesRtn, 0))
		{
			funccloseHandle(hDrive);
		}

		//If IDE identify command not supported, fails   
		if (!(vers.fCapabilities & 1))
		{
			funccloseHandle(hDrive);
		}

		//Identify the IDE drives   
		if (j & 1)
		{
			in.irDriveRegs.bDriveHeadReg = 0xb0;
		}
		else
		{
			in.irDriveRegs.bDriveHeadReg = 0xa0;
		}

		if (vers.fCapabilities&(16 >> j))
		{
			//We don't detect a ATAPI device.   
			funccloseHandle(hDrive);
		}
		else
		{
			in.irDriveRegs.bCommandReg = 0xec;
		}
		in.bDriveNumber = j;
		in.irDriveRegs.bSectorCountReg = 1;
		in.irDriveRegs.bSectorNumberReg = 1;
		in.cBufferSize = 512;

		if (!DeviceIoControl(hDrive, DFP_RECEIVE_DRIVE_DATA, &in, sizeof(in), &out, sizeof(out), &bytesRtn, 0))
		{
			//"DeviceIoControl failed:DFP_RECEIVE_DRIVE_DATA"<<endl;   
			funccloseHandle(hDrive);
		}
		phdinfo = (PIDSECTOR)out.bBuffer;

		char    s[21] = { 0 };
		memcpy(s, phdinfo->sSerialNumber, 20);
		s[20] = 0;
		ChangeByteOrder(s, 20);

		//ɾ���ո��ַ�  
		int ix = 0;
		for (ix = 0; ix < 20; ix++)
		{
			if (s[ix] == ' ')
			{
				continue;
			}
			break;
		}
		memcpy(lpszHD, s + ix, 20);
		bRtn = TRUE;
		break;
		//FOREND:  
		  //      CloseHandle(hDrive);   
		  //      hDrive = NULL;   
	}

	CloseHandle(hDrive);
	hDrive = NULL;
	return(bRtn);
}

void funccloseHandle(HANDLE  hDrive)
{
	CloseHandle(hDrive);
	hDrive = NULL;
}

#endif