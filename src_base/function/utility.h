/////////////////////////////////////////////////////////////////////////
///utility.h
///��������ϵͳ�����Եĺ���
/////////////////////////////////////////////////////////////////////////

#ifndef UTILITY_H
#define UTILITY_H

#include "platform.h"

///���һ���ַ�������Ŀո�
///@param	s	Ҫ����ո���ַ������ᱻ�ƻ�
///@return	�����ո���ַ�����һ���ַ����s
char *trim(char *s);

///��ȡ��ǰ��ϵͳʱ�䣬�Գ���ʽ��ʽ����
///@return	�Գ���ʽ��ʽ�����ĵ�ǰʱ��
//char *getLongTime(void);
//
/////��ȡ��ǰ��ϵͳʱ�䣬�Զ̸�ʽ��ʽ����
/////@return	�Զ̸�ʽ��ʽ�����ĵ�ǰʱ��
//char *getShortTime(void);
//
/////��ȡ�����ʱ�� hh:mm:ss ��ʽ
//char *getTodayTime(void);

///��ȡ��ǰ���� yyyymmdd
struct tm GetNowDate(char *pszDate);

struct tm GetSlogDate(char *pszDate);

struct tm GetTM(time_t *pTime);

/*
///�����ǹ�ҫ�����

///��filename,��ΪDEBUG�ļ����
///����0:�ɹ�
///����:ʧ��
int dbgOpen(char *strFileName); 

///���strBuffer��Ϣ��DEBUG�ļ���
int dbgPrint(char *strBuffer);

///�ر�debug�ļ�
int dbgClose();

///�õ�debug���ļ�
FILE *getFpDebugOutput();


///�ڶ���DEBUG_LOG������¶������з��� ,������ΪNULL
#ifdef DEBUG_LOG
#define DBG_OPEN(s)     dbgOpen(s)
#define DBG_STRING(s)	\
	{					\
		FILE *fp = getFpDebugOutput();\
		fprintf(fp, "[%s]:%s:%d\n", getLongTime(),__FILE__,__LINE__);\
		fprintf(fp,"%s",s!=NULL?s:"The String is NULL, Please check your program\n");\
		fflush(fp);\
	}	
				
#define DBG_CLASS(p) 	\
	{					\
		FILE *fp = getFpDebugOutput();\
		fprintf(fp, "[%s]:%s:%d\n", getLongTime(),__FILE__,__LINE__);\
		if(p!=NULL)   \
			(p)->dump(fp);\
		else \
			fprintf(fp,"The Class Pointer is NULL, Please check your program\n");	\
		fflush(fp);\
	}	
#define DBG_CLOSE	dbgClose	
#else
#define DBG_OPEN(s)		NULL
#define DBG_STRING(s)	NULL				
#define DBG_CLASS(p) 	NULL
#define DGB_CLOSE		NULL
#endif

#ifdef _DEBUG
#define TRACE0(sz)				{printf("[%s][%d] ",__FILE__,__LINE__);printf("%s",sz);}
#define TRACE1(sz, p1)          {printf("[%s][%d] ",__FILE__,__LINE__);printf(sz, p1);}
#define TRACE2(sz, p1, p2)      {printf("[%s][%d] ",__FILE__,__LINE__);printf(sz, p1, p2);}
#define TRACE3(sz, p1, p2, p3)  {printf("[%s][%d] ",__FILE__,__LINE__);printf(sz, p1, p2, p3);}
#else
#define TRACE0(sz)
#define TRACE1(sz, p1)
#define TRACE2(sz, p1, p2)
#define TRACE3(sz, p1, p2, p3)
#endif

#define TRACE	TRACE0
*/

/*
���ܣ� ��������str�Ƿ��Ƿ������±�׼����ֵ��
    û��С���������
    �����һ��С����ĸ�����
    ������TRIM���ܣ�����пո񣬷��ط�
return 0: ��
rerurn 1: ��
*/
int isValidNumber(char *s);

/*
���ܣ�����ַ����Ƿ�Ϸ������ڸ�ʽyyyymmdd
    ע���ȷ�ǺϷ������ڣ�����������229�Ŀ��ǣ�
���أ�
    return 0: ��
    rerurn 1: ��
 
*/
int isValidDate(char *s);

/*
���ܣ�����ַ����Ƿ�Ϸ���ʱ���ʽhhmmss
���أ�
    return 0: ��
    rerurn 1: ��
*/
int isValidTime(char *s);

/**���һ��IP��ַ�Ƿ���ָ��IP��ַƥ��
*@param pIpAddress �ַ�����ʾ��IP��ַ("xxx.xxx.xxx.xxx")
*@param pIpMatch �ַ�����ʾ��Ҫƥ���IP��ַ("xxx.xxx.xxx.xxx")
*@param pIpMask �ַ�����ʾ������("xxx.xxx.xxx.xxx")
*@return true ��ַƥ�� false ��ַ��ƥ��
*@remark
*/
bool CheckIpAddressMatch(const char *pIpAddress, 
						 const char *pIpMatch, 
						 const char *pIpMask);

/**���һ��IP��ַ�Ƿ���ָ��IP��ַƥ��
*@param pIpAddress ������ʾ��IP��ַ
*@param pIpMatch ������ʾ��Ҫƥ���IP��ַ
*@param pIpMask ������ʾ������
*@return true ��ַƥ�� false ��ַ��ƥ��
*@remark
*/
bool CheckIpAddressMatch(const unsigned long ulIpAddress, 
						 const unsigned long ulIpMatch, 
						 const unsigned long ulpMask);

///�ж������в����Ƿ�˵��Ӧ�����ûָ�ģʽ
///@param	argc	��������
///@param	argv	����ֵ��
///@return	true��ʾ�ǻָ�ģʽ��false��ʾ���ǻָ�ģʽ
bool isRecovery(int argc, char *argv[]);

char * StrCCopy(char * dest, const char * source, size_t count);

#ifdef WIN32
//*********************************************************************************************************
//������Ӳ�����кŻ�ȡ
//*********************************************************************************************************
#include <iostream>  
#include <windows.h>  

#define DFP_GET_VERSION         0x00074080   
#define DFP_SEND_DRIVE_COMMAND  0x0007c084   
#define DFP_RECEIVE_DRIVE_DATA  0x0007c088   

typedef struct _GETVERSIONOUTPARAMS   
{   
    char bVersion; // Binary driver version.   
    char bRevision; // Binary driver revision.   
    char bReserved; // Not used.   
    char bIDEDeviceMap; // Bit map of IDE devices.   
    int fCapabilities; // Bit mask of driver capabilities.   
    int dwReserved[4]; // For future use.   
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;   

#if _MSC_VER <= 1300

typedef struct _IDEREGS   
{   
    char bFeaturesReg; // Used for specifying SMART "commands".   
    char bSectorCountReg; // IDE sector count register   
    char bSectorNumberReg; // IDE sector number register   
    char bCylLowReg; // IDE low order cylinder value   
    char bCylHighReg; // IDE high order cylinder value   
    char bDriveHeadReg; // IDE drive/head register   
    char bCommandReg; // Actual IDE command.   
    char bReserved; // reserved for future use. Must be zero.   
} IDEREGS, *PIDEREGS, *LPIDEREGS;   

typedef struct _SENDCMDINPARAMS   
{   
    int cBufferSize; // Buffer size in bytes   
    IDEREGS irDriveRegs; // Structure with drive register values.   
    char bDriveNumber; // Physical drive number to send command to (0,1,2,3).   
    char bReserved[3]; // Reserved for future expansion.   
    int dwReserved[4]; // For future use.   
} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;   

typedef struct _DRIVERSTATUS   
{   
    char bDriverError; // Error code from driver,   
    // or 0 if no error.   
    char bIDEStatus; // Contents of IDE Error register.   
    // Only valid when bDriverError   
    // is SMART_IDE_ERROR.   
    char bReserved[2]; // Reserved for future expansion.   
    int dwReserved[2]; // Reserved for future expansion.   
} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;   

typedef struct _SENDCMDOUTPARAMS   
{   
    int cBufferSize; // Size of bBuffer in bytes   
    DRIVERSTATUS DriverStatus; // Driver status structure.   
    char bBuffer[512]; // Buffer of arbitrary length   
    // in which to store the data read from the drive.   
} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;   
#endif _MSC_VER <= 1300

typedef struct _IDSECTOR   
{   
    USHORT wGenConfig;   
    USHORT wNumCyls;   
    USHORT wReserved;   
    USHORT wNumHeads;   
    USHORT wBytesPerTrack;   
    USHORT wBytesPerSector;   
    USHORT wSectorsPerTrack;   
    USHORT wVendorUnique[3];   
    CHAR sSerialNumber[20];   
    USHORT wBufferType;   
    USHORT wBufferSize;   
    USHORT wECCSize;   
    CHAR sFirmwareRev[8];   
    CHAR sModelNumber[40];   
    USHORT wMoreVendorUnique;   
    USHORT wDoubleWordIO;   
    USHORT wCapabilities;   
    USHORT wReserved1;   
    USHORT wPIOTiming;   
    USHORT wDMATiming;   
    USHORT wBS;   
    USHORT wNumCurrentCyls;   
    USHORT wNumCurrentHeads;   
    USHORT wNumCurrentSectorsPerTrack;   
    ULONG ulCurrentSectorCapacity;   
    USHORT wMultSectorStuff;   
    ULONG ulTotalAddressableSectors;   
    USHORT wSingleWordDMA;   
    USHORT wMultiWordDMA;   
    BYTE bReserved[128];   
} IDSECTOR, *PIDSECTOR;   

void funccloseHandle(HANDLE  hDrive);
//--------------------------------------------------------------  
//                      Ӳ�����к�  
//--------------------------------------------------------------  
BOOL GetHDSerial(char *lpszHD, int len=128);  

#endif

#endif
