/////////////////////////////////////////////////////////////////////////
///������Ҫ�õ��Ĺ�������
/////////////////////////////////////////////////////////////////////////

#ifndef _BaseFunction_H_
#define _BaseFunction_H_

#include "platform.h"

void PrintGodBless();

char *base64_encode(const char* data, int data_len);
static int find_pos(char ch);
char *base64_decode(const char *data, int data_len);
string& Txttrim(string &s);

vector<string> Txtsplit(const string& str, string separate_character);
void Txtsplit(const string& str, string separate_character, vector<string>& vct);

map<string, string> mapSplit(const string& str, string s1, string s2);
void mapSplit(const string& str, string s1, string s2, map<string,string>& mapStr);

//Tom=Happy&Jerry=Sad : s1=& s2==
map<string, string> mapSplit(char* str, int len, char s1, char s2);

void splitstr(const char* content, const char* token, vector<std::string>& vect);

void run_command(const char *format,...);

char* fastStrcat(char *pszDest, const char* pszSrc, char cSplit = '\0');

char* fastStrcatLimit(char *pszDest, const char* pszSrc, int nLength = 1024);

char* getSplit(char *pszDest, char cSplit);

#ifdef WIN32
//�˺�����windows�����ʹ��
////////////////////////////////////////////////////////////////////////////
// �������� GetMacByCmd(char *lpszMac)
// ������
//      ���룺 void
//      ����� lpszMac,���ص�MAC��ַ��
// ����ֵ��
//      TRUE:  ���MAC��ַ��
//      FALSE: ��ȡMAC��ַʧ�ܡ�
// ���̣�
//      1. ����һ�������ܵ���
//      2. ����һ��IPCONFIG �Ľ��̣���������ض��򵽹ܵ���
//      3. �ӹܵ���ȡ�����з��ص�������Ϣ���뻺����lpszBuffer��
//      4. �ӻ�����lpszBuffer�л�ó�ȡ��MAC����
//
//  ��ʾ�����Է�����ɴ˳�����IP��ַ��������Ϣ��
//        ���������Ŀ���ͨ���������ʽ�õ�����Ϣֻ��ı�strFetCmd �� 
//        str4Search�����ݼ��ɡ�
///////////////////////////////////////////////////////////////////////////
bool GetMacByCmd(char *lpszMac);
#endif



#endif
