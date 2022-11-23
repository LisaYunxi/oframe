/////////////////////////////////////////////////////////////////////////
///CExchangeResponser.h
/// �����˻ص��ӿڵļ���ʹ�÷�ʽ������Ǹ�������ϵͳʹ�õ�
///@history 
///���һ� �������ļ�
/////////////////////////////////////////////////////////////////////////

#ifndef _Responser_exchange_H_
#define _Responser_exchange_H_

#include "OutFogServiceApi.h"
#include "FieldDescribe.h"
#include "BaseFunction.h"

//#ifndef CopyHeadMeta
//#define CopyHeadMeta(target,source)	\
//	(target)->MessageNo=(source)->MessageNo;	\
//	(target)->Type=(source)->Type;	\
//	(target)->Chain=(source)->Chain;	\
//	(target)->FrontNo=(source)->FrontNo;	\
//	(target)->SessionNo=(source)->SessionNo;	\
//	(target)->RequestNo=(source)->RequestNo;	\
//	(target)->SubjectNo=(source)->SubjectNo; \
//	(target)->SequenceNo=(source)->SequenceNo;	\
//	(target)->GlobalNo=(source)->GlobalNo;		\
//	(target)->BusinessNo=(source)->BusinessNo;		\
//	(target)->Direction=(source)->Direction;		\
//	memcpy((target)->Remark,(source)->Remark,128);
//#endif

//��ʹ�����ֶο�����ʹ��memcpy��ԭ����Ϊ�˼ӿ������ٶ�
//FieldNo��IsReplay���ܿ����������api���еģ������в���Ҫ
//�����ֽڶ���ԭ�����ﲻ��ʹ��sizeof(TOutFogMetaRef)����ʹ���˹�����Ĵ�С
#ifndef CopyHeadMeta
#define CopyHeadMeta(target,source)	\
	memcpy((target),(source),176);
#endif

//����һ������
//--���ͻر���������response/public/private/subject
//---CXTPResponser.SendResponse(**,OUTFOG_CHAIN_SINGLE);

//����һ������
//--��һ��һ��Ҫ�б��ģ�������response/public/private/subject
//---CXTPResponser.SendToPublic(**,OUTFOG_CHAIN_FIRST);
//--������͵��м䱨�ĳ��ִ����������������Զ����´��
//---CXTPResponser.SendNext(**,OUTFOG_CHAIN_CONTINUE);
//---CXTPResponser.SendNext(**,OUTFOG_CHAIN_LAST);

class CServiceResponser
{
public:
	CServiceResponser(COutFogServiceCallback *pServiceCallback)
	{
		m_pServiceCallback = pServiceCallback;
		memset(&m_OutFogMetaRef, 0, sizeof(TOutFogMetaRef));
		m_OutFogMetaRef.Chain = OUTFOG_CHAIN_SINGLE;
	}

	//�ظ������û��Ի��Ͳ�ѯ
	virtual bool SendResponse(int nMessageNo, void *pField, CFieldDescribe *pFieldDescribe, int nFrontNo, int nSessionNo, int nRequestNo, 
		char cChain = OUTFOG_CHAIN_SINGLE, int nErrorNo = 0, char* sErrorMsg = "Success")
	{
		//û��ָ��ǰ�ã��޷��ظ���Ϣ
		if (nFrontNo == INT_32_MAX)
		{
			m_OutFogMetaRef.RequestNo = -1; //�����ı���ͬ���޷�����
			return false;
		}
			
		//û��׼�����ģ��޷����
		if (cChain == OUTFOG_CHAIN_LAST || cChain == OUTFOG_CHAIN_CONTINUE)
		{
			m_OutFogMetaRef.RequestNo = -1; //�����ı���ͬ���޷�����
			return false;
		}			
		//���nRequestNoС���㣬�Ͳ�Ҫ�ظ��ˣ������˻���������ʹ��
		if (nRequestNo < 0)
		{
			m_OutFogMetaRef.RequestNo = nRequestNo; //�����ı���ͬ���޷�����
			return false;
		}			

		m_OutFogMetaRef.FrontNo = nFrontNo;
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_RESPONSE;
		m_OutFogMetaRef.MessageNo = nMessageNo;
		m_OutFogMetaRef.SessionNo = nSessionNo;
		m_OutFogMetaRef.RequestNo = nRequestNo;
		m_OutFogMetaRef.Chain = cChain;

		m_OutFogMetaRef.SubjectNo = nErrorNo;
		//sprintf(m_OutFogMetaRef.SubIndexName, sErrorMsg);
		//���ܿ���
		//����ֻ�ܿ���124���ֽڣ�Ҫ��Ȼ��Ӱ�������ֶ�
		strncpy(m_OutFogMetaRef.Remark, sErrorMsg, 124);

		if (pField != NULL)
		{
			m_OutFogMetaRef.FieldNo = pFieldDescribe->m_FieldID;
			m_OutFogMetaRef.Length = pFieldDescribe->m_nStreamSize;
			pFieldDescribe->setSingleField(m_pStreamBuf, pField);
			m_pServiceCallback->RspField(&m_OutFogMetaRef, m_pStreamBuf);
		}
		else
		{
			m_pServiceCallback->RspField(&m_OutFogMetaRef, NULL);
		}
		return true;
	};

	//���������ⷢ������
	virtual void SendToSubjectID(int nMessageNo, void *pField, CFieldDescribe *pFieldDescribe, int dwSubjectID, UF_INT8 nBusinessNo = UF_INT8_MAX, char cChain = OUTFOG_CHAIN_SINGLE)
	{
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_PUBLISH;
		m_OutFogMetaRef.MessageNo = nMessageNo;
		m_OutFogMetaRef.SubjectNo = dwSubjectID;
		m_OutFogMetaRef.BusinessNo = nBusinessNo;
		m_OutFogMetaRef.Chain = cChain;

		if (pField == NULL)
		{
			m_pServiceCallback->RspField(&m_OutFogMetaRef, NULL);
		}
		else
		{
			m_OutFogMetaRef.FieldNo = pFieldDescribe->m_FieldID;
			m_OutFogMetaRef.Length = pFieldDescribe->m_nStreamSize;
			pFieldDescribe->setSingleField(m_pStreamBuf, pField);
			m_pServiceCallback->RspField(&m_OutFogMetaRef, m_pStreamBuf);
		}
	};

	virtual void PrepareResponse(int nMessageNo, int nErrorID, char *sErrorMsg, int nFrontNo, int nSessionNo, int nRequestNo)
	{
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_RESPONSE;
		m_OutFogMetaRef.MessageNo = nMessageNo;
		m_OutFogMetaRef.FrontNo = nFrontNo;
		m_OutFogMetaRef.SessionNo = nSessionNo;
		m_OutFogMetaRef.RequestNo = nRequestNo;
		m_OutFogMetaRef.SubjectNo = nErrorID;
		m_OutFogMetaRef.Chain = OUTFOG_CHAIN_FIRST;
		//sprintf(m_OutFogMetaRef.SubIndexName, sErrorMsg);
		strcpy(m_OutFogMetaRef.Remark, sErrorMsg);
		m_pServiceCallback->RspField(&m_OutFogMetaRef, NULL);
	};

	//����������һ��TID��Field,���cChain==OUTFOG_CHAIN_LAST,ȫ������
	virtual void SendNext(void *pField, CFieldDescribe *pFieldDescribe, char cChain = OUTFOG_CHAIN_CONTINUE)
	{
		if (m_OutFogMetaRef.RequestNo < 0)
			return;
		m_OutFogMetaRef.Chain = cChain;
		if (pField == NULL)
		{
			m_pServiceCallback->RspField(&m_OutFogMetaRef, NULL);
		}
		else
		{
			m_OutFogMetaRef.FieldNo = pFieldDescribe->m_FieldID;
			m_OutFogMetaRef.Length = pFieldDescribe->m_nStreamSize;
			pFieldDescribe->setSingleField(m_pStreamBuf, pField);
			m_pServiceCallback->RspField(&m_OutFogMetaRef, m_pStreamBuf);
		}
	};

	//����ָ������ˮ��ǰ������ע���(public��private��user����ע��)
	//virtual void SubSubjectID(int nLocalID, int dwSubjectID, UF_INT8 nSequenceNo, const char* SubIndexName, const char* UserID, int nFrontID, int nSessionID, UF_INT8 nBusinessNo = UF_INT8_MAX, int nRequestNo = 1)
	virtual void SubSubjectID(int nLocalID, int dwSubjectID, UF_INT8 nSequenceNo, const char* SubIndexName, int nFrontID, int nSessionID, UF_INT8 nBusinessNo = UF_INT8_MAX, int nRequestNo = 1)
	{
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_SUBTOPIC;
		m_OutFogMetaRef.SubjectNo = dwSubjectID;
		m_OutFogMetaRef.MessageNo = nLocalID;
		m_OutFogMetaRef.SequenceNo = nSequenceNo;
		m_OutFogMetaRef.BusinessNo = nBusinessNo;
		m_OutFogMetaRef.Chain = OUTFOG_CHAIN_SINGLE;
		m_OutFogMetaRef.FrontNo = nFrontID;
		m_OutFogMetaRef.SessionNo = nSessionID;
		m_OutFogMetaRef.RequestNo = nRequestNo;
		strcpy(m_OutFogMetaRef.SubIndexName, SubIndexName);
		//strcpy(m_OutFogMetaRef.UserID, UserID);
		m_pServiceCallback->RspField(&m_OutFogMetaRef, NULL);
	};

	virtual void AddApiKey(const char* sApiKey, const char* sSecretKey, const char* sUserID, const UF_INT8 iExpireTime, const int iAccessLimit)
	{
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_ADDAPIKEY;
		m_OutFogMetaRef.Chain = OUTFOG_CHAIN_SINGLE;
		strcpy(m_OutFogMetaRef.Remark, sApiKey);
		sprintf(m_pStreamBuf, "%s,%s", sUserID, sSecretKey);
		m_OutFogMetaRef.BusinessNo = iExpireTime;
		m_OutFogMetaRef.SessionNo = iAccessLimit;
		m_OutFogMetaRef.FieldNo = 0;
		m_OutFogMetaRef.Length = 512;
		m_pServiceCallback->RspField(&m_OutFogMetaRef, m_pStreamBuf);
	};

	virtual void DelSession(int nFrontID, int nSessionID)
	{
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_DELSESSION;
		m_OutFogMetaRef.Chain = OUTFOG_CHAIN_SINGLE;
		m_OutFogMetaRef.FrontNo = nFrontID;
		m_OutFogMetaRef.SessionNo = nSessionID;
		m_pServiceCallback->RspField(&m_OutFogMetaRef, NULL);
	};

	virtual void DelApiKey(const char* sApiKey)
	{
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_DELAPIKEY;
		m_OutFogMetaRef.Chain = OUTFOG_CHAIN_SINGLE;
		strcpy(m_OutFogMetaRef.Remark, sApiKey);
		m_pServiceCallback->RspField(&m_OutFogMetaRef, NULL);
	};

	virtual void AddApiKeyAllowIP(const char* sApiKey, const char* sAllowIP)
	{
		vector<string> IPs = Txtsplit(sAllowIP, ";");
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_ADDAPIKEYALLOWIP;
		m_OutFogMetaRef.Chain = OUTFOG_CHAIN_SINGLE;
		m_OutFogMetaRef.FieldNo = 0;
		m_OutFogMetaRef.Length = 512;
		for (vector<string>::iterator it = IPs.begin(); it != IPs.end(); it++)
		{
			strcpy(m_OutFogMetaRef.Remark, sApiKey);
			strcpy(m_pStreamBuf, (*it).c_str());
			m_pServiceCallback->RspField(&m_OutFogMetaRef, m_pStreamBuf);
		}
	};

	virtual void DelApiKeyAllowIP(const char* sApiKey, const char* sAllowIP)
	{
		vector<string> IPs = Txtsplit(sAllowIP, ";");
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_DELAPIKEYALLOWIP;
		m_OutFogMetaRef.Chain = OUTFOG_CHAIN_SINGLE;
		m_OutFogMetaRef.FieldNo = 0;
		m_OutFogMetaRef.Length = 512;
		for (vector<string>::iterator it = IPs.begin(); it != IPs.end(); it++)
		{
			strcpy(m_OutFogMetaRef.Remark, sApiKey);
			strcpy(m_pStreamBuf, (*it).c_str());
			m_pServiceCallback->RspField(&m_OutFogMetaRef, m_pStreamBuf);
		}
	};

	virtual void UnSubSubjectID(int nLocalID, int nFrontID, int nSessionID)
	{
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_UNSUBTOPIC;
		m_OutFogMetaRef.MessageNo = nLocalID;
		m_OutFogMetaRef.Chain = OUTFOG_CHAIN_SINGLE;
		m_OutFogMetaRef.FrontNo = nFrontID;
		m_OutFogMetaRef.SessionNo = nSessionID;
		m_pServiceCallback->RspField(&m_OutFogMetaRef, NULL);
	};

	virtual void UnSubSubjectIDAll(int nFrontID, int nSessionID)
	{
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_UNSUBTOPIC;
		sprintf(m_OutFogMetaRef.Remark, OUTFOG_REMARK_UNSUBTOPIC_ALL);
		m_OutFogMetaRef.Chain = OUTFOG_CHAIN_SINGLE;
		m_OutFogMetaRef.FrontNo = nFrontID;
		m_OutFogMetaRef.SessionNo = nSessionID;
		m_pServiceCallback->RspField(&m_OutFogMetaRef, NULL);
	};

	//���̻�����
	virtual void InitLink(const char* sLinkDll, int nLinkID, int nTID, void *pField, CFieldDescribe *pFieldDescribe, int dwSubjectID, char cChain = OUTFOG_CHAIN_SINGLE)
	{
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_OFFER;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_INITLINK;
		strncpy(m_OutFogMetaRef.SubIndexName, sLinkDll, sizeof(m_OutFogMetaRef.SubIndexName));
		m_OutFogMetaRef.BusinessNo = nLinkID;
		m_OutFogMetaRef.MessageNo = nTID;
		m_OutFogMetaRef.SubjectNo = dwSubjectID;
		m_OutFogMetaRef.FieldNo = pFieldDescribe->m_FieldID;
		m_OutFogMetaRef.Length = pFieldDescribe->m_nStreamSize;
		m_OutFogMetaRef.Chain = cChain;
		pFieldDescribe->setSingleField(m_pStreamBuf, pField);
		m_pServiceCallback->RspField(&m_OutFogMetaRef, m_pStreamBuf);
	};

	virtual void StartLink(int nLinkType, int nLinkID, int nTID, void *pField, CFieldDescribe *pFieldDescribe, int dwSubjectID, char cChain = OUTFOG_CHAIN_SINGLE)
	{
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_OFFER;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_START;
		m_OutFogMetaRef.BusinessNo = nLinkID;
		m_OutFogMetaRef.MessageNo = nTID;
		m_OutFogMetaRef.SubjectNo = dwSubjectID;
		m_OutFogMetaRef.FieldNo = pFieldDescribe->m_FieldID;
		m_OutFogMetaRef.Length = pFieldDescribe->m_nStreamSize;
		m_OutFogMetaRef.Chain = cChain;
		pFieldDescribe->setSingleField(m_pStreamBuf, pField);
		m_pServiceCallback->RspField(&m_OutFogMetaRef, m_pStreamBuf);
	};

	//���̻�����
	virtual void SendToLink(int nTID, void *pField, CFieldDescribe *pFieldDescribe, int dwSubjectID, char cChain = OUTFOG_CHAIN_SINGLE)
	{
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_OFFER;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_PUBLISH;
		m_OutFogMetaRef.MessageNo = nTID;
		m_OutFogMetaRef.SubjectNo = dwSubjectID;
		m_OutFogMetaRef.FieldNo = pFieldDescribe->m_FieldID;
		m_OutFogMetaRef.Length = pFieldDescribe->m_nStreamSize;
		m_OutFogMetaRef.Chain = cChain;
		pFieldDescribe->setSingleField(m_pStreamBuf, pField);
		m_pServiceCallback->RspField(&m_OutFogMetaRef, m_pStreamBuf);
	};

	//��֪���Ĵ����￪ʼ���ݣ�ֻ�е�������ӿ�֮�󣬺��Ĳſ�ʼ����
	virtual void InitKernel(UF_INT8 dwSequenceNo)
	{
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_NULL;
		//m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_KERNEL;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_INIT;
		m_OutFogMetaRef.SequenceNo = dwSequenceNo;
		m_pServiceCallback->RspField(&m_OutFogMetaRef, NULL);
	};

	//��ȡcallback
	virtual COutFogServiceCallback* GetCallback()
	{
		return m_pServiceCallback;
	};
	//��ȡMetaRef
	virtual TOutFogMetaRef* GetMetaRef()
	{
		return &m_OutFogMetaRef;
	};
	//��ȡStreamBuf
	virtual char* GetStreamBuf()
	{
		return m_pStreamBuf;
	};
	//׼��MetaRef
	virtual bool PrepareMetaRef(CFieldDescribe *pFieldDescribe, void* pData, void* pOldData, char Type)
	{
		memset(&m_OutFogMetaRef, 0, sizeof(TOutFogMetaRef));
		return true;
	};
	virtual bool setdumpNo(UF_INT8 nSequenceNo)
	{
		return true;
	}

protected:
	TOutFogMetaRef m_OutFogMetaRef;
	COutFogServiceCallback *m_pServiceCallback;
	char m_pStreamBuf[OUTFOG_MAX_PACKAGE_SIZE];
};

#endif

