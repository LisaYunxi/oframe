#include "BaseFunction.h"

string& Txttrim(string &s)
{
	if (s.empty())
	{
		return s;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

//Tom=Happy&Jerry=Sad : s1=& s2==
map<string, string> mapSplit(char* str, int len, char s1, char s2)
{
	map<string, string> mapstr;
	if (len == 0)
		return mapstr;
	if (str == NULL)
		return mapstr;
	char* p1 = str;
	char* p2 = NULL;
	int i = 0;
	for (i = 0; i < len; i++)
	{
		if (str[i] == s1)
		{
			str[i] = '\0';
			mapstr.insert(map<string, string>::value_type(p1, p2));
			p1 = str + i + 1;
		}
		else if (str[i] == s2)
		{
			str[i] = '\0';
			p2 = str + i + 1;
		}
	}
	str[len] = '\0';
	mapstr.insert(map<string, string>::value_type(p1, p2));
	return mapstr;
}

map<string, string> mapSplit(const string& str, string s1, string s2)
{
	map<string, string> mapstr;
	mapSplit(str, s1, s2, mapstr);
	return mapstr;
}

void mapSplit(const string& str, string s1, string s2, map<string, string>& mapStr)
{
	mapStr.clear();

	int separate_characterLen = (int)s1.size();
	int lastPosition = 0;
	int index = -1;
	while (-1 != (index = (int)str.find(s1, lastPosition)))
	{
		string thisStr = str.substr(lastPosition, index - lastPosition);
		int index2 = -1;
		if (-1 != (index2 = (int)thisStr.find(s2)))
		{
			string a = thisStr.substr(0, index2);
			string b = thisStr.substr(index2, thisStr.size());
			mapStr.insert(map<string, string>::value_type(a, b));
		}
		lastPosition = index + separate_characterLen;
	}
	string lastString = str.substr(lastPosition);
	if (!lastString.empty())
	{
		string thisStr = str.substr(lastPosition, index - lastPosition);
		int index2 = -1;
		if (-1 != (index2 = (int)thisStr.find(s2)))
		{
			string a = thisStr.substr(0, index2);
			string b = thisStr.substr(index2, thisStr.size());
			mapStr.insert(map<string, string>::value_type(a, b));
		}
	}
}

vector<string> Txtsplit(const string& str, string separate_character)
{
	vector<string> vestr;
	Txtsplit(str, separate_character, vestr);
	return vestr;
}

void Txtsplit(const string& str, string separate_character, vector<string>& vct)
{
	vct.clear();

	int separate_characterLen = (int)separate_character.size();
	int lastPosition = 0;
	int index = -1;
	while (-1 != (index = (int)str.find(separate_character, lastPosition)))
	{
		vct.push_back(str.substr(lastPosition, index - lastPosition));
		lastPosition = index + separate_characterLen;
	}
	string lastString = str.substr(lastPosition);
	if (!lastString.empty())
	{
		vct.push_back(lastString);
	}
}

void splitstr(const char* content, const char* token, vector<std::string>& vect)
{
	if (content == NULL)
		return;
	int len = (int)strlen(content);
	if (len <= 0)
		return;
	char* pBuf = (char*)malloc(len + 1);
	strcpy(pBuf, content);
	char* str = strtok(pBuf, token);
	while (str != NULL)
	{
		vect.push_back(str);
		str = strtok(NULL, token);
	}
	free(pBuf);
}

#ifdef WIN32
bool GetMacByCmd(char *lpszMac)
{
	//命令行输出缓冲大小
	const long MAX_COMMAND_SIZE = 10000;
	//获取MAC命令行
	char szFetCmd[] = "ipconfig /all";
	//网卡MAC地址的前导信息
	const string str4Search = "Physical Address. . . . . . . . . : ";
	//初始化返回MAC地址缓冲区
	memset(lpszMac, 0x00, sizeof(lpszMac));
	BOOL bret;
	SECURITY_ATTRIBUTES sa;
	HANDLE hReadPipe, hWritePipe;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = true;

	//创建管道
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if (!bret)
	{
		return false;
	}
	//控制命令行窗口信息
	STARTUPINFO si;
	//返回进程信息
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE; //隐藏命令行窗口
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	//创建获取命令行进程
	bret = CreateProcess(NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!bret)
	{
		return false;
	}
	char szBuffer[MAX_COMMAND_SIZE + 1]; //放置命令行输出缓冲区
	string strBuffer;

	//使用上面的win7会造成假死情况
	//WaitForSingleObject (pi.hProcess, INFINITE);
	WaitForSingleObject(pi.hProcess, 200);
	unsigned long count;
	memset(szBuffer, 0x00, sizeof(szBuffer));
	bret = ReadFile(hReadPipe, szBuffer, MAX_COMMAND_SIZE, &count, 0);

	//关闭所有的句柄
	CloseHandle(hWritePipe);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hReadPipe);

	if (!bret)
	{
		return false;
	}

	//	printf("%s \n",szBuffer);

	// 		strBuffer = szBuffer;
	// 		long ipos;
	// 		ipos = strBuffer.find(str4Search);
	// 		//提取MAC地址串
	// 		strBuffer = strBuffer.substr(ipos+str4Search.length());
	// 		ipos = strBuffer.find("/n");
	// 		strBuffer = strBuffer.substr(0, ipos);

	//提取MAC地址串
	// 			strBuffer = strBuffer.substr(ipos+str4Search.length());
	// 			ipos = strBuffer.find("/n");
	// 			strBuffer = strBuffer.substr(0, ipos);
	// 			
	// 			string strExp("([0-9a-fA-F]{2})-([0-9a-fA-F]{2})-([0-9a-fA-F]{2})-([0-9a-fA-F]{2})-([0-9a-fA-F]{2})-([0-9a-fA-F]{2})");
	// 			boost::regex expression(strExp, boost::regex::perl|boost::regex::icase);
	// 			boost::cmatch what; 
	// 			if(boost::regex_search(strBuffer.c_str(), what, expression))
	// 			{
	// 				string str1 = what[1];
	// 				string str2 = what[2];
	// 				string str3 = what[3];
	// 				string str4 = what[4];
	// 				string str5 = what[5];
	// 				string str6 = what[6];
	// 				strMac = str1+str2+str3+str4+str5+str6;
	// 			}

	// 	memset(szBuffer, 0x00, sizeof(szBuffer));
	// 	strcpy(szBuffer, strBuffer.c_str());
	// 	//去掉中间的“00-50-EB-0F-27-82”中间的'-'得到0050EB0F2782
	// 	int j = 0;
	// 	for(int i=0; i<strlen(szBuffer); i++)
	// 	{
	// 		if(szBuffer[i] != '-')
	// 		{
	// 			lpszMac[j] = szBuffer[i];
	// 			j++;
	// 		}
	// 	}

	return true;
}

#endif


/* */

char *base64_encode(const char* data, int data_len)
{
	const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	//int data_len = strlen(data); 
	int prepare = 0;
	int ret_len;
	int temp = 0;
	char *ret = NULL;
	char *f = NULL;
	int tmp = 0;
	char changed[4];
	int i = 0;
	ret_len = data_len / 3;
	temp = data_len % 3;
	if (temp > 0)
	{
		ret_len += 1;
	}
	ret_len = ret_len * 4 + 1;
	ret = (char *)malloc(ret_len);

	if (ret == NULL)
	{
		printf("No enough memory.\n");
		exit(0);
	}
	memset(ret, 0, ret_len);
	f = ret;
	while (tmp < data_len)
	{
		temp = 0;
		prepare = 0;
		memset(changed, '\0', 4);
		while (temp < 3)
		{
			//printf("tmp = %d\n", tmp); 
			if (tmp >= data_len)
			{
				break;
			}
			prepare = ((prepare << 8) | (data[tmp] & 0xFF));
			tmp++;
			temp++;
		}
		prepare = (prepare << ((3 - temp) * 8));
		//printf("before for : temp = %d, prepare = %d\n", temp, prepare); 
		for (i = 0; i < 4; i++)
		{
			if (temp < i)
			{
				changed[i] = 0x40;
			}
			else
			{
				changed[i] = (prepare >> ((3 - i) * 6)) & 0x3F;
			}
			*f = base[changed[i]];
			//printf("%.2X", changed[i]); 
			f++;
		}
	}
	*f = '\0';

	return ret;

}
/* */
static int find_pos(char ch)
{
	const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	char *ptr = (char*)strrchr(base, ch);//the last position (the only) in base[] 
	return (int)(ptr - base);
}
/* */
char *base64_decode(const char *data, int data_len)
{
	int ret_len = (data_len / 4) * 3;
	int equal_count = 0;
	char *ret = NULL;
	char *f = NULL;
	int tmp = 0;
	int temp = 0;
	char need[3];
	int prepare = 0;
	int i = 0;
	if (*(data + data_len - 1) == '=')
	{
		equal_count += 1;
	}
	if (*(data + data_len - 2) == '=')
	{
		equal_count += 1;
	}
	if (*(data + data_len - 3) == '=')
	{//seems impossible 
		equal_count += 1;
	}
	switch (equal_count)
	{
	case 0:
		ret_len += 4;//3 + 1 [1 for NULL] 
		break;
	case 1:
		ret_len += 4;//Ceil((6*3)/8)+1 
		break;
	case 2:
		ret_len += 3;//Ceil((6*2)/8)+1 
		break;
	case 3:
		ret_len += 2;//Ceil((6*1)/8)+1 
		break;
	}
	ret = (char *)malloc(ret_len);
	if (ret == NULL)
	{
		printf("No enough memory.\n");
		exit(0);
	}
	memset(ret, 0, ret_len);
	f = ret;
	while (tmp < (data_len - equal_count))
	{
		temp = 0;
		prepare = 0;
		memset(need, 0, 4);
		while (temp < 4)
		{
			if (tmp >= (data_len - equal_count))
			{
				break;
			}
			prepare = (prepare << 6) | (find_pos(data[tmp]));
			temp++;
			tmp++;
		}
		prepare = prepare << ((4 - temp) * 6);
		for (i = 0; i < 3; i++)
		{
			if (i == temp)
			{
				break;
			}
			*f = (char)((prepare >> ((2 - i) * 8)) & 0xFF);
			f++;
		}
	}
	*f = '\0';
	return ret;
}

void run_command(const char *format, ...)
{
	static char buffer[1024];
	size_t result, status;
	va_list v;
	//	char *arg_list[30];
	char *args[30];
	int i;
	static char command_buffer[1024];
	va_start(v, format);
	vsprintf(buffer, format, v);
	va_end(v);
	/* printf("%s\n",buffer); */
	strcpy(command_buffer, buffer);
	for (i = 0; i < 30; i++)
	{
		char *this_token;
		if (i == 0)
			this_token = strtok(buffer, " \t\n");
		else
			this_token = strtok(NULL, " \t\n");
		args[i] = this_token;
		if (this_token == NULL)
			break;
	}

	//	for (i=0;args[i]!=NULL;i++)
	//	{
	//		printf("args[%d]=%s\n",i,args[i]);
	//	}

	//	printf("%s\n",command_buffer);

#ifdef WIN32
	result = _spawnvp(_P_WAIT, args[0], args);
	status = result;
#else
	result = system(command_buffer);
	status = WEXITSTATUS(result);
#endif

	if (status == 0)
		return;
	printf("Failed in running command below:\n\t%s\n", command_buffer);
}

//字符串拼接之后返回Dest的下一个开始的位置
char* fastStrcat(char *pszDest, const char* pszSrc, char cSplit)
{
	while (*pszDest)
		pszDest++;
	if (cSplit != '\0')
	{
		*pszDest = cSplit;
		pszDest++;
	}
	while ((*pszDest++ = *pszSrc++));
	return --pszDest;
}

char* fastStrcatLimit(char *pszDest, const char* pszSrc, int nLength)
{
	while (*pszDest)
		pszDest++;

	int i = 0;
	while ((*pszDest = *pszSrc))
	{
		i++;
		if (i > nLength)
			break;
		pszDest++;
		pszSrc++;
	}

	*pszDest = '\0';
	return pszDest;
}

//获取下一个cSplit的字符串，返回下一个开始位置
//此举会破坏原有的 pszDest
char* getSplit(char *pszDest, char cSplit)
{
	while (!(*pszDest == cSplit || *pszDest == '\0'))
		pszDest++;

	while (*pszDest != cSplit)
	{
		if (*pszDest == '\0')
			return NULL;
		pszDest++;
	}

	*pszDest = '\0';
	pszDest++;
	if (*pszDest == '\0')
		return NULL;
	else
		return pszDest;
}

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//                              _ooOoo_                                //
//                             o8888888o                               //
//                             88" . "88                               //
//                             (| ^_^ |)                               //
//                             O\  =  /O                               //
//                          ____/`---'\____                            //
//                        .'  \\|     |//  `.                          //
//                       /  \\|||  :  |||//  \                         //
//                      /  _||||| -:- |||||-  \                        //
//                      |   | \\\  -  /// |   |                        //
//                      | \_|  ''\---/''  |   |                        //
//                      \  .-\__  `-`  ___/-. /                        //
//                    ___`. .'  /--.--\  `. . ___                      //
//                 ."" '<  `.___\_<|>_/___.'  >'"".                    //
//                | | :  `- \`.;`\ _ /`;.`/ - ` : | |                  //
//                \  \ `-.   \_ __\ /__ _/   .-` /  /                  //
//         ========`-.____`-.___\_____/___.-`____.-'========           //
//                              `=---='                                //
//         ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^           //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

void PrintGodBless()
{
	printf("///////////////////////////////////////////////////////////////////////\n");
	printf("//                                                                   //\n");
	printf("//                              _ooOoo_                              //\n");
	printf("//                             o8888888o                             //\n");
	printf("//                             88\" . \"88                             //\n");
	printf("//                             (| ^_^ |)                             //\n");
	printf("//                             O\\  =  /O                             //\n");
	printf("//                          ____/'---'\\____                          //\n");
	printf("//                        .'  \\\\|     |//  `.                        //\n");
	printf("//                       /  \\\\|||  :  |||//  \\                       //\n");
	printf("//                      /  _||||| -:- |||||-  \\                      //\n");
	printf("//                      |   | \\\\\\  -  /// |   |                      //\n");
	printf("//                      | \\_|  ''\\---/''  |   |                      //\n");
	printf("//                      \\  .-\\__  `-`  ___/-. /                      //\n");
	printf("//                    ___`. .'  /--.--\\  `. . ___                    //\n");
	printf("//                 .\"\" '<  `.___\\_<|>_/___.'  >' \"\".                 //\n");
	printf("//                | | :  `- \\`.;`\\ _ /`;.`/ - ` : | |                //\n");
	printf("//                \\  \\ `-.   \\_ __\\ /__ _/   .-` /  /                //\n");
	printf("//         ========`-.____`-.___\\_____/___.-`____.-'========         //\n");
	printf("//                              `=---='                              //\n");
	printf("//         ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^         //\n");
	printf("//                                                                   //\n");
	printf("///////////////////////////////////////////////////////////////////////\n");
}
