/////////////////////////////////////////////////////////////////////////
///version.h
///һ������İ汾��
/////////////////////////////////////////////////////////////////////////

#ifndef VERSION_H
#define VERSION_H

#include "platform.h"

extern const char *version;

///���������в�������ʾ�汾��
///@param	argc	��������
///@param	argv	����ֵ
void showVersion(int argc,char *argv[]);
void checkVersion();

#endif
