/********************************************************
	��ȡ�����ļ���
	�ļ���: profile.h
**********************************************************/
#ifndef __PROFILE_H
#define __PROFILE_H

#include "platform.h"

char *mytrim(char *s);

class TIniFile
{
	//���������
	FILE *fin, *fout;
	//����ָ��section���Ƶ��ַ���ָ��
	char *titlePos(char *buf, int *len);
	//�Ƿ���һ��section�ַ���
	bool isTitleLine(char *bufPtr);
	//�Ƿ���ָ����section
	bool containTitle(char *buf, const char *section);
	//�ƶ��ļ�ָ�뵽��Ӧ��sectionλ��
	bool gotoSection(const char *section);
	//����ָ��entry���Ƶ��ַ���ָ��
	char *textPos(char *buf, const char *entry);
	//ȥ���ַ���ǰ��Ŀո�
	void stripQuotationChar(char *buf);
	//��ȡһ��entry��ֵ
	int readEntry(const char *entry, char *buf, int bufSize, bool strip);

public:
	// ������: TIniFile::ReadTString
	// ����  : �����Ķ�ȡ�����ֶε��ַ������ҹ������ҿո��'\t'
	// ����  : char * 
	// ����  : const char *section
	// ����  : const char *entry
	// ����  : const char *defaultString
	// ����  : char *buffer
	// ����  : int bufLen
	char * ReadTString(const char *section, const char *entry, const char *defaultString, char *buffer, int bufLen);

	// ������: TIniFile::LRTrim
	// ����  : ��str�н����ҿո��'\t'��ȡ��
	// ����  : char * 
	// ����  : char *str
	static char * LRTrim(char *str);
	//�����ļ�����
	char FileName[128];
	//�������ļ�
	bool Open(const char *filename);
	//��ȡһ��int��entry��ֵ
	int ReadInt(const char *section, const char *entry, int defaultInt);
	//��ȡһ��string�͵�entry��ֵ
	int ReadString(const char *section, const char *entry, const char *defaultString, char *buffer, int bufLen);
	//��ȡһ��string�͵�entry��ֵ
	void ReadStringList(const char *section, const char *entry, const char *defaultString, vector<string> *temp);
	//дһ��string�͵�entry��ָ����section��
	//дһ��int�͵�entry��ָ����section��
	//�ر������ļ�
	void Close();

	TIniFile();
	~TIniFile();
private:
	char * FGetS(char *pbuf, int size, FILE *fp);
	void out_close();
	void in_close();
	char * percolate(char *str);
};
#endif
