#ifndef _FTD_FRONT_H_
#define _FTD_FRONT_H_

#include "CachedFlow.h"
#include "SelectReactor.h"
#include "Config.h"
#include "LinkManager.h"
#include "printinfo.h"
#include "OutFogServiceApi.h"
#include "Channel.h"
#include "SessionFactory.h"
#include "Flow.h"
#include "FlowManager.h"
#include "FTDCPackage.h"
#include "FTDCSession.h"
#include "ServiceResponser.h"

//typedef CUFIntType CInnerSequenceSeriesType;
//typedef CUFFloatType<16, 0> CInnerSequenceNoType;

//const unsigned int FTD_TID_IntlRtnDissemination = 0x00005901;

//class CFTDDisseminationField
//{
//public:
//	///序列系列号
//	CInnerSequenceSeriesType	SubjectNo;
//	///序列号
//	CInnerSequenceNoType	SequenceNo;
//
//	void DescribeMembers()
//	{
//		m_Describe.SetupMember(SubjectNo, (int)((char *)&SubjectNo - (char *)this), "SubjectNo", sizeof(SubjectNo));
//		m_Describe.SetupMember(SequenceNo,(int)((char *)&SequenceNo - (char *)this), "SequenceNo", sizeof(SequenceNo));
//	}
//	static CFieldDescribe m_Describe;
//};
//const unsigned int FTD_FID_Dissemination = 0x0001;


struct TFrontSessionInfo
{
	unsigned int nSessionID;			//会话号
	unsigned int nConnectTime;			//连接建立时间
	UF_INT8 nDialogFlowSeqNo;			//对话流当前序号
	UF_INT8 nQueryFlowSeqNo;			//查询流当前序号
	bool bIsLogin;						//是否已经登录
	char UserID[16];					//会话所属用户代码
	int nCommFlux;						//单位时间的通讯流量
	int nMaxCommFlux;					//单位时间的通讯流量上限
};

class CFrontWriteLogReactor : public CThread
{
public:
	CFrontWriteLogReactor(CFlow *pSlogResult,const char* szFlowPath, int nFrontID, int nGap)
	{
		m_dwFrontNo = nFrontID;
		m_pSlogResult = pSlogResult;
		char szBuffer[300];
		strcpy(szBuffer, szFlowPath);
		strcat(szBuffer, "front");
		m_strFlowPathName = szBuffer;
		m_fpChannelLog = NULL;
		OpenChannelLogFile();
		m_reader.AttachFlow(m_pSlogResult, 0);
		m_nCount = 0;
		m_nGap = nGap;
	}
	virtual void Run();
	bool OpenChannelLogFile();

private:
	CFlow *m_pSlogResult;
	FILE *m_fpChannelLog;
	CFlowReader m_reader;
	string m_strFlowPathName;
	int m_nCount;
	int m_nGap;
	char m_fDate[13];				//日志文件日期
	int m_dwFrontNo;
};

class CSlogFlow :public CChannelLog
{
public:
	CSlogFlow(CFlow *pFlow)
	{
		m_pFlow = pFlow;
	}
	virtual ~CSlogFlow() {};

	virtual UF_INT8 Append(void *pObject, int length)
	{
		if (m_pFlow != NULL)
			return m_pFlow->Append(pObject, length);
		return -1;
	}
protected:
	CFlow *m_pFlow;
};

class CFtdSessionFactory :public CSessionFactory, public CFTDCSessionCallback
{
public:
	//pSeriesFlow是前置的输入
	CFtdSessionFactory(CReactor *pReactor, CFlow* pSeriesFlow, CFlowManager *pFlowManager, const char* pFrontAddress, int nFrontid);
	virtual ~CFtdSessionFactory();

	virtual void OnTimer(int nIDEvent);

	virtual int HandlePackage(CFTDCPackage *pFTDCPackage, CFTDCSession *pSession);
	bool HandleResponse(CXTPPackage *pMessage);

	virtual CSession *CreateSession(CChannel *pChannel, unsigned int dwMark);
	virtual void OnSessionConnected(CSession *pSession);
	virtual void OnSessionDisconnected(CSession *pSession, int nReason);
	void CheckSession(CSession *pSession, TFrontSessionInfo *pSessionInfo);
	void CalcFlux(CSession *pSession, const int nFlux);

	//发送到对话流
	void SendToDialogFlow(CFTDCSession *pSession, CFTDCPackage *pPackage);
	//按照业务端制定的SubjectNo发送
	void SendRsponser(CFTDCSession *pSession, CFTDCPackage *pPackage);

	void DeleteSession(CXTPPackage *pMessage);
	void SubTopic(CXTPPackage *pMessage);
	void UnSubTopic(CXTPPackage *pMessage);

private:

	CReactor *m_pReactor;
	CFlow* m_pSeriesFlow;

	CFTDCPackage m_rspPackage;

	CXTPPackage m_reqPackage;
	//CXTPPackage m_rspPackage;

	int m_dwFrontNo;
	char m_FrontAddress[512];

	CFrontWriteLogReactor *m_pFrontWriteLogReactor;
	CFlow	*m_pSlogResult;
	CFlowManager *m_pFlowManager;

	typedef CHashMap<unsigned int, TFrontSessionInfo, HashInt> CSessionInfoMap;
	CSessionInfoMap m_mapSessionInfo;

	CSlogFlow *m_pChannelLogFlow;

	//int m_nMaxSpeed;

	int m_nFlowCacheSize;
};

class CFtdFront :public CSelectReactor, public COutFogServiceApi,public COutFogServiceCallback
{
public:
	CFtdFront(CReadFlow* pFlow, CFlow* pSeriesFlow, CFlowManager *pFlowManager, const char* pFrontAddress, int nFrontid, int nPortNumber) :COutFogServiceApi()
	{
//		m_pOtherTransfer = NULL;
		m_rspPackage.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
		m_Package.ConstructAllocate(PACKAGE_MAX_SIZE, PACKAGE_RESERVE_SIZE);
		m_ResultReader.AttachFlow(pFlow, 0);
		m_pSeriesFlow = pSeriesFlow;
		m_nFrontID = nFrontid;

		//tcp://127.0.0.1:12001
		vector<string> sSplitThird = Txtsplit(pFrontAddress, ":");
		int nPort = atoi(sSplitThird[2].c_str());
		char sFrontAddress[256];
		sprintf(sFrontAddress, "%s:%s:%d", sSplitThird[0].c_str(), sSplitThird[1].c_str(), nPort + nPortNumber - 1);

		PRINT_TO_STD("CFront: CFront's IPArddress:%s", sFrontAddress);
		m_pSessionFactory = new CFtdSessionFactory(this, pSeriesFlow, pFlowManager, sFrontAddress, nFrontid);
	}
	virtual ~CFtdFront()
	{
	};

	virtual const char* getVersion() { return "Front"; };
	virtual bool ReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pStream)
	{
		if (pOutFogReqRef->Type == OUTFOG_TYPE_START)
		{
			Create();
			m_pSessionFactory->Start();
			m_pSessionFactory->EnableListener(true);
			PRINT_TO_STD("FrontStartWork: Open Front Port!");
		}
		return true;
	}
	virtual bool SyncReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pReqStream, TOutFogMetaRef* pOutFogRspRef, void *pRspStream)
	{
		return true;
	}
	virtual bool RspField(const TOutFogMetaRef* pRspRef, const void *pRspBuffer)
	{
		CopyHeadMeta(m_Package.GetXTPHeader(), pRspRef);
		m_Package.GetXTPHeader()->FrontNo = m_nFrontID;
		m_Package.AddStream(pRspRef->FieldNo, (const char *)pRspBuffer, pRspRef->Length);
		m_Package.MakePackage();
		m_pSeriesFlow->Append(m_Package.Address(), m_Package.Length());
		return true;
	}
	virtual bool HandleOtherTask()
	{
		for (int i = 0; i < 100; i++)
		{
			if (m_ResultReader.GetNext(&m_Package))
			{
				HandleMessage(&m_Package);
			}
			else
			{
				break;
			}
		}
		return CSelectReactor::HandleOtherTask();
	}

	void HandleMessage(CXTPPackage *pMessage)
	{
		switch (pMessage->GetXTPHeader()->Type)
		{
		case OUTFOG_TYPE_DELSESSION:
		{
			m_pSessionFactory->DeleteSession(pMessage);
			break;
		}
		case OUTFOG_TYPE_SUBTOPIC:
		{
			m_pSessionFactory->SubTopic(pMessage);
			break;
		}
		case OUTFOG_TYPE_UNSUBTOPIC:
		{
			m_pSessionFactory->UnSubTopic(pMessage);
			break;
		}
		case OUTFOG_TYPE_RESPONSE:
		{
			m_pSessionFactory->HandleResponse(pMessage);
			break;
		}
		default:
		{
			REPORT_EVENT(LOG_CRITICAL, "Front", "HandleMessage Error: NO XTPHeader Type");
			break;
		}
		}
	}

private:

	int m_nFrontID;
	CXTPPackage m_Package;
	CFTDCPackage m_rspPackage;

	CFlow* m_pSeriesFlow;
	CFlowReader m_ResultReader;
	CFlowManager *m_pFlowManager;
	CFtdSessionFactory* m_pSessionFactory;

	//为OtherTransfer解析的时候需要
	//COutFogServiceApi* m_pOtherTransfer;
	char m_fieldStruct[OUTFOG_MAX_PACKAGE_SIZE];
	char m_fieldStream[OUTFOG_MAX_PACKAGE_SIZE];
	TOutFogMetaRef m_FieldMetaInput;
	TOutFogMetaRef m_FieldMetaOutput;
};


#endif
