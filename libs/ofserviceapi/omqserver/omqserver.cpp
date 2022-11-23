/////////////////////////////////////////////////////////////////////////
///@brief OFrame��main�ļ�
///@history 
///20190909	���һ�		�������ļ�
///////////////////////////////////////////////////////////////////////

#include "OutFogServiceApi.h"
#include "OutFogService.h"
#include "utility.h"
#include "version.h"
#include "ServiceResponser.h"

///////////////////////////////����������///////////////////////////////////////////

void CmdLineArgError()
{
	printf("Useage: omqserver ([0/1/.../n]/[-v] \n");
	printf("        omqserver         :������������Ĭ��Ϊ0,ֻ��֧�ֵ�������\n");
	printf("        omqserver 0...n   :����������\n");
	printf("        omqserver -v      :��ʾ�汾��Ϣ\n");
	exit(0);
}

/////////////////////////////////////////////////////////////////////////////////////
// �������

class CKernel:public COutFogServiceCallback
{
public:
	CKernel()
	{
	}

	void setServiceApi(COutFogServiceApi* pServiceApi)
	{
		m_pServiceApi = pServiceApi;

		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_REGTOPIC;
		m_OutFogMetaRef.SubjectNo = 2;
		m_OutFogMetaRef.Chain = OUTFOG_CHAIN_SINGLE;
		m_pServiceApi->ReqField(&m_OutFogMetaRef,NULL);		
	}

	virtual bool RspField(const TOutFogMetaRef* pOutFogRspRef,const void *pField)
	{	
		CopyHeadMeta(&m_OutFogMetaRef,pOutFogRspRef);
		//��ʼ����һЩ��ˮ
		m_OutFogMetaRef.Direction = OUTFOG_DIRECTION_TO_FRONT;
		m_OutFogMetaRef.Chain = OUTFOG_CHAIN_SINGLE;
		m_OutFogMetaRef.Type = OUTFOG_TYPE_PUBLISH;
		sprintf(m_OutFogMetaRef.SubIndexName, "x");
		if (fmod(m_OutFogMetaRef.SequenceNo,2) == 0)
		{
			sprintf(m_OutFogMetaRef.UserID, "y");
		}
		else
		{
			memset(m_OutFogMetaRef.UserID, 0, sizeof(m_OutFogMetaRef.UserID));
		}
		m_OutFogMetaRef.SubjectNo = 2;
		m_pServiceApi->ReqField(&m_OutFogMetaRef,pField);

		//����Ӧ��
		m_OutFogMetaRef.Type = OUTFOG_TYPE_RESPONSE;
		m_pServiceApi->ReqField(&m_OutFogMetaRef,pField);		
		return true;
	}
	
private:
	COutFogServiceApi* m_pServiceApi;
	TOutFogMetaRef m_OutFogMetaRef;
};


int main(int argc, char *argv[])
{

	showVersion(argc,argv);

#ifdef UNIX
	signal(SIGABRT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, SIG_IGN);
#endif

	if (argc >3)
	{
		CmdLineArgError();
	}

	int nKernelid = 0;
	if(argc > 1)
	{
		if(isValidNumber(argv[1]))
		{
			nKernelid = atoi(argv[1]);
		}
	}	

	CKernel* pKernel = new CKernel();
	COutFogService*	m_pService = new COutFogService();
	COutFogServiceApi *pOutFogServiceApi = m_pService->LoadOutFogApi("omqsapi",pKernel,nKernelid);
	pKernel->setServiceApi(pOutFogServiceApi);

	TOutFogMetaRef OutFogMetaRef;
	OutFogMetaRef.Type = OUTFOG_TYPE_START;
	pOutFogServiceApi->ReqField(&OutFogMetaRef,NULL);

	while(true)
	{
		SLEEP_SECONDS(1000);
	}

	return 0;
}
