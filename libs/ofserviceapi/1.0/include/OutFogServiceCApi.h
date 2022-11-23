/////////////////////////////////////////////////////////////////////////
///OutFogServiceCApi.h
///��������COutFogServiceCApi
///@history 
///���һ� �������ļ�
/////////////////////////////////////////////////////////////////////////

#ifndef _OUTFOG_SERVICE_CAPI_H_
#define _OUTFOG_SERVICE_CAPI_H_

#include "OutFogServiceApiStruct.h"

#ifdef __cplusplus
extern "C" {
#endif

	//�˺궨��Ϊͳһ�淶������ʹ��C++�������static����const���������붨��OUTFOGSERVICE_API_EXPORT�궨��
	//ʹ�ô�API�ӿ�ʱ��windowsƽ̨���趨��WINDOWS�궨�弴�ɣ�����ƽ̨�����κκ궨��
#ifdef WINDOWS
#ifdef LIB_OUTFOGSERVICE_CAPI_EXPORT
#ifdef OUTFOGSERVICE_CAPI_IS_LIB
#define OUTFOGSERVICE_CAPI_EXPORT __declspec(dllexport)
#else
#define OUTFOGSERVICE_CAPI_EXPORT __declspec(dllimport)
#endif
#else
#define OUTFOGSERVICE_CAPI_EXPORT
#endif
#else
#define OUTFOGSERVICE_CAPI_EXPORT
#endif

//����һ��Field
//���cChain==OUTFOG_CHAIN_LAST,���̷���
//���cChain==OUTFOG_CHAIN_CONTINUE,��������,����ȴ���һ��
//���cChain==OUTFOG_CHAIN_FIRST,ȫ���ȫ��������׼����һ������
	typedef void(*OutFogRspField)(void* pInstance, TOutFogMetaRef* pRspRef, void *pRspBuffer);

	struct TOutFogCallbacks
	{
		OutFogRspField ptrOutFogRspField;
	};

	///ע��ص��������
	///@param pInstance InitApi�������ص�APIʵ����ַ
	///@param callbacks �ص��������ĵ�ַ
	OUTFOGSERVICE_CAPI_EXPORT void OutFogRegisterCallbacks(void* pInstance, TOutFogCallbacks *pCallbacks);

	OUTFOGSERVICE_CAPI_EXPORT const char* OutFogGetVersion(void* pInstance);

	OUTFOGSERVICE_CAPI_EXPORT bool OutFogStart(void* pInstance);

	OUTFOGSERVICE_CAPI_EXPORT bool OutFogReqField(void* pInstance, const TOutFogMetaRef* pReqRef, const void *pBuffer);
	OUTFOGSERVICE_CAPI_EXPORT bool OutFogSyncReqField(const TOutFogMetaRef* pReqRef, const void *pReqBuffer, TOutFogMetaRef* pRspRef, void *pRspBuffer);

	OUTFOGSERVICE_CAPI_EXPORT int OutFogGetServiceID(void* pInstance);

	OUTFOGSERVICE_CAPI_EXPORT void* CreateOutFogServiceCApi(int nServiceNo);


	//��̬�⶯̬���õĺ���
	typedef void _OutFogRegisterCallbacks(void* pInstance, TOutFogCallbacks *pCallbacks);
	typedef const char* _OutFogGetVersion(void* pInstance);
	typedef bool _OutFogStart(void* pInstance);
	typedef bool _OutFogReqField(void* pInstance, const TOutFogMetaRef* pReqRef, const void *pStream);
	typedef bool _OutFogSyncReqField(void* pInstance, const TOutFogMetaRef* pReqRef, const void *pReqStream, TOutFogMetaRef* pRspRef, void *pRspStream);

	typedef int _OutFogGetServiceID(void* pInstance);
	typedef void* _CreateOutFogServiceCApi(int nServiceNo);

#ifdef __cplusplus
}
#endif

#endif

