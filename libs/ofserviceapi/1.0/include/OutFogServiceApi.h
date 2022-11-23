/////////////////////////////////////////////////////////////////////////
///OutFogServiceApi.h
///��������COutFogServiceApi
///@history 
///���һ� �������ļ�
/////////////////////////////////////////////////////////////////////////

#ifndef _OUTFOG_SERVICE_API_H_
#define _OUTFOG_SERVICE_API_H_

#include "OutFogServiceApiStruct.h"

//�˺궨��Ϊͳһ�淶������ʹ��C++�������static����const���������붨��OUTFOGSERVICE_API_EXPORT�궨��
//ʹ�ô�API�ӿ�ʱ��windowsƽ̨���趨��WINDOWS�궨�弴�ɣ�����ƽ̨�����κκ궨��
#ifdef WINDOWS
#ifdef LIB_OUTFOGSERVICE_API_EXPORT
#ifdef OUTFOGSERVICE_API_IS_LIB
#define OUTFOGSERVICE_API_EXPORT __declspec(dllexport)
#else
#define OUTFOGSERVICE_API_EXPORT __declspec(dllimport)
#endif
#else
#define OUTFOGSERVICE_API_EXPORT
#endif
#else
#define OUTFOGSERVICE_API_EXPORT
#endif

class COutFogServiceCallback
{
public:
	//����һ��Field
	//���cChain==OUTFOG_CHAIN_LAST,���̷���
	//���cChain==OUTFOG_CHAIN_CONTINUE,��������,����ȴ���һ��
	//���cChain==OUTFOG_CHAIN_FIRST,ȫ���ȫ��������׼����һ������
	virtual bool RspField(const TOutFogMetaRef* pRspRef, const void *pRspBuffer) = 0;
};

class OUTFOGSERVICE_API_EXPORT COutFogServiceApi
{
public:
	COutFogServiceApi(COutFogServiceCallback *pCallback = 0)
	{
		m_pServiceCallback = pCallback;
	}
	virtual ~COutFogServiceApi()
	{
	}

	virtual const char* getVersion() = 0;

	//�첽����ӿڣ�ͨ��COutFogServiceCallback���ɷ��ض�����¼
	virtual bool ReqField(const TOutFogMetaRef* pReqRef, const void *pReqBuffer) = 0;

	//ͬ������ӿڣ�ͨ���������أ�ֻ����һ����¼
	virtual bool SyncReqField(const TOutFogMetaRef* pReqRef, const void *pReqBuffer, TOutFogMetaRef* pRspRef, void *pRspBuffer) = 0;

	virtual void setServiceCallback(COutFogServiceCallback *pCallback)
	{
		m_pServiceCallback = pCallback;
	}

	COutFogServiceCallback* getServiceCallback()
	{
		return m_pServiceCallback;
	}

protected:
	COutFogServiceCallback* m_pServiceCallback;
};

extern "C" OUTFOGSERVICE_API_EXPORT COutFogServiceApi* CreateOutFogServiceApi(COutFogServiceCallback *pCallback, int nServiceNo);

//��̬�⶯̬���õĺ���
typedef COutFogServiceApi* _CreateOutFogServiceApi(COutFogServiceCallback *pCallback, int nServiceNo);

#endif

