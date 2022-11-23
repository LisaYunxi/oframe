#include "public.h"
#include "SocketInit.h"

CSocketInit::CSocketInit()
{
#ifdef WIN32
	WSADATA initData;
	initData.wVersion = 0;
	initData.wHighVersion = 2;
	strcpy(initData.szDescription, "");
	strcpy(initData.szSystemStatus, "");
	initData.iMaxSockets = 100;
	initData.lpVendorInfo = 0;
	initData.lpVendorInfo = "";
	WSAStartup(2, &initData);
	if (WSAStartup(2, &initData) != 0)
		EMERGENCY_EXIT("Can not init socket");
#endif

}

CSocketInit::~CSocketInit()
{

}
