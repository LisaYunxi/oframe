#include "OfKernelApiImp.h"
//#include "strapi.h"

COfKernelApiImp::COfKernelApiImp(COutFogServiceCallback *pCallback, int nServiceID) :COutFogServiceApi(pCallback)
{
	m_nKernelID = nServiceID;
	m_nReliableMode = GetConfigInt(INI_FILE_NAME, "ReliableMode");
	m_pKernelapp = new CKernelApp(m_nKernelID, pCallback);
	if (GetConfigString(INI_FILE_NAME, "InputToOutput") == "yes")
		m_pKernelapp->SetKernelAppCallback(this);

	memset(&m_OutFogMetaRef, 0, sizeof(TOutFogMetaRef));
}

COfKernelApiImp::~COfKernelApiImp()
{
}

bool COfKernelApiImp::RspPackage(CXTPPackage *pMessage)
{
	switch (pMessage->GetXTPHeader()->Direction)
	{
	case OUTFOG_DIRECTION_TO_FRONT:
	{
		m_pKernelapp->RspField(pMessage);
		break;
	}
	case OUTFOG_DIRECTION_TO_KERNEL:
	{
		pMessage->GetXTPHeader()->Direction = OUTFOG_DIRECTION_TO_OFFER;
		m_pKernelapp->RspField(pMessage);
		break;
	}
	default:
	{
	}
	}
	return true;
}

bool COfKernelApiImp::ReqField(const TOutFogMetaRef* pOutFogReqRef, const void *pStream)
{
	if (pOutFogReqRef->Type == OUTFOG_TYPE_START)
	{
		Start();
		return true;
	}
	return m_pKernelapp->RspField(pOutFogReqRef, pStream);
}

const char* COfKernelApiImp::getVersion()
{
	static char version[256];
	sprintf(version, "Service version: %s", "trade");
	return version;
}

bool COfKernelApiImp::Start()
{
	m_pKernelapp->Create();
	PRINT_TO_STD("Kernel ID is %d", m_nKernelID);
	while (!m_pKernelapp->m_bFinishInitInstance)
	{
		SLEEP_SECONDS(1);
	}

	CCompReactor *pCompReactor = NULL;
	if (m_nReliableMode == ReliableMode_HotStandby)
	{
		CCompFlowFactory* pFlowFactory = new CCompFlowFactory(m_pKernelapp->m_pPreSeries);
		pCompReactor = new CCompReactor(m_nKernelID, m_pKernelapp, pFlowFactory, GetConfigString(INI_FILE_NAME,"AppName").c_str());
		pCompReactor->Create();
		//有排队机的情况下一直睡眠等待状态切换到单点或者主模式才允许程序往下走
		while (m_pKernelapp->GetCompStateID() != CS_SINGLE && m_pKernelapp->GetCompStateID() != CS_PRIMARY)
		{
			SLEEP_SECONDS(1);
			m_OutFogMetaRef.Type = OUTFOG_TYPE_LOOP;
			m_OutFogMetaRef.IsReplay = true;
			m_pServiceCallback->RspField(&m_OutFogMetaRef, NULL);
		}
	}
	return true;
}

extern "C" COutFogServiceApi* CreateOutFogServiceApi(COutFogServiceCallback *pCallback, int nServiceID)
{
	return new COfKernelApiImp(pCallback, nServiceID);
}
