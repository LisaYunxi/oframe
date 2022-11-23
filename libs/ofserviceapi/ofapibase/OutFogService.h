/////////////////////////////////////////////////////////////////////////
///@system OutFogServiceApiWrapper
///OutFogServiceApiWrapper.h
///������C++ʵ��C��api��һ���򵥵Ľ��ּ�
///@history 
///20190909	���һ�	�������ļ�
/////////////////////////////////////////////////////////////////////////

#ifndef _OUTFOG_SERVICE_H_
#define _OUTFOG_SERVICE_H_

#include "OutFogServiceApiWrapper.h"
#include "BaseFunction.h"

#define OUTFOG_DLL_TYPE_C			1		//C����API
#define OUTFOG_DLL_TYPE_CPLUSPLUS	2		//C++����API

class COutFogService
{
public:
	COutFogService()
	{
	}
	virtual ~COutFogService()
	{
	}

	static COutFogServiceApi* getOutFogService(const char* pName, COutFogServiceCallback *pCallback = NULL, int nServiceID = 0)
	{
		COutFogServiceApi* pTradeEngineApi = NULL;
		vector<string> fileNames = Txtsplit(pName, ":");
		if (fileNames.size() != 2)
		{
			printf("getOutFogService: DLL should be dll:2 or dll:1 \n");
			return NULL;
		}
		if (atoi(fileNames[1].c_str()) == OUTFOG_DLL_TYPE_C)
		{
			pTradeEngineApi = new COutFogServiceApiWrapper(pName);
			return pTradeEngineApi;
		}
		return LoadOutFogApi(fileNames[0].c_str(), pCallback, nServiceID);
	}

	//��������
	static COutFogServiceApi* LoadOutFogApi(const char* psApiname, COutFogServiceCallback *pCallback, int nServiceID)
	{
		COutFogServiceApi* pTradeEngineApi = NULL;
		char ServiceDllName[128];
#ifdef WINDOWS
		sprintf(ServiceDllName, "%s.dll", psApiname);
		HINSTANCE hDll = LoadLibrary(ServiceDllName);
		if (hDll != NULL)
		{
			_CreateOutFogServiceApi* MyCreateServiceApi = (_CreateOutFogServiceApi*)GetProcAddress(hDll, "CreateOutFogServiceApi");
			if (MyCreateServiceApi != NULL)
			{
				pTradeEngineApi = MyCreateServiceApi(pCallback, nServiceID);
				if (pTradeEngineApi == NULL)
				{
					printf("error: CreateOutFogServiceApi return NULL\n");
					return NULL;
				}
				else
				{
					printf("kernel: Load Library:%s \n", ServiceDllName);
				}
			}
			else
			{
				int errorid = GetLastError();
				printf("error: Can not GetProcAddress:CreateOutFogServiceApi errorid:%d\n", errorid);
				return NULL;
			}
		}
		else
		{
			int errorid = GetLastError();
			printf("error: Can not LoadLibrary:%s errorid:%d\n", ServiceDllName, errorid);
			return NULL;
		}
		if (pTradeEngineApi == NULL)
		{
			int errorid = GetLastError();
			printf("error: Can not LoadLibrary:%s errorid:%d\n", ServiceDllName, errorid);
		}
#else
		sprintf(ServiceDllName, "lib%s.so", psApiname);
		void *handle = dlopen(ServiceDllName, RTLD_NOW);
		if (handle)
		{
			_CreateOutFogServiceApi* MyCreateServiceApi = (_CreateOutFogServiceApi*)dlsym(handle, "CreateOutFogServiceApi");
			if (MyCreateServiceApi != NULL)
			{
				pTradeEngineApi = MyCreateServiceApi(pCallback, nServiceID);
				if (pTradeEngineApi == NULL)
				{
					printf("error: CreateOutFogServiceApi return NULL\n");
					return NULL;
				}
				else
				{
					printf("kernel:Load Library:%s \n", ServiceDllName);
				}
			}
			else
			{
				printf("error: Can not dlsym:CreateOutFogServiceApi msg:%s\n", dlerror());
				return NULL;
			}
		}
		else
		{
			printf("error: Can not dlopen:%s msg:%s\n", ServiceDllName, dlerror());
			return NULL;
		}

#endif
		return pTradeEngineApi;
	}

	char* showVersion(const char* pName)
	{

	}
};

#endif

