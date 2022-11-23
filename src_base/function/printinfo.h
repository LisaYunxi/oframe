#ifndef __PRINTINFO_H_
#define __PRINTINFO_H_

#include "platform.h"

#define SEP_CHAR	','
#define MSG_INFO	1
#define MSG_WARN	2
#define MSG_ERROR	3
#define MSG_FATAL	4


//char* getTime();
//char* getCurrDate();
//char* getMachineName();
//char* getCurrTime();
//char* getCurrTimeNoGap();
//char* getCurrDateTime();
//char* getCurrDateMinute();

bool formatTime(char* pTime);

int getHourMinus(const char* sFirstTime, const char * sSencondTime);
//��24СʱΪһ����������
int minusHour(const char* sFirstTime, const char * sSencondTime);

int getMiniteMinus(const char* sFirstTime, const char * sSencondTime);
//��60����Ϊһ����������
int minusMinute(const char* sFirstTime, const char * sSencondTime);

int getSecondMinus(const char* sFirstTime, const char * sSencondTime);

//void printInfo(int level,char *sCppName,char* pFormat, ...);
int CheckTime(char* localtime, char *sTime);
int ReplaceStr(char *sSrc, char *sMatchStr, char *sReplaceStr);

// ��string�洢������ʱ�䣬ת��Ϊintͨ��ʱ�������ҪӦ�ø���ʱ���ټӼ�һЩֵ��
//int getLongDayTime(const char* sDay, const char* sTime, const char * patternDay = NULL, const  char * patternTime = NULL);
//int getLongTime(const char* sTime, const char *pattern = NULL);


//char* getLogTime(char* pszDate);
char* getLogMinTime(char* pszDateMin);
char* getLocalTime(char* pBuffer);

//char* convertTimestampToStr(time_t ts);

#endif
