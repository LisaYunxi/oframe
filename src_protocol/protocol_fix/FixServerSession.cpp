#include "FixServerSession.h"
#include "FileStore.h"



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CFixServerSession::CFixServerSession(const char *pFixServerCfgFile)
:m_iMaxSsID(0), m_FixServerStarted(false)
{
	std::string file = pFixServerCfgFile;
	FIX::SessionSettings  settings( file );
	InitSession(settings);
}

CFixServerSession::CFixServerSession(FIX::SessionSettings sSettings)
:m_iMaxSsID(0), m_FixServerStarted(false)
{
	InitSession(sSettings);
}

void CFixServerSession::InitSession(FIX::SessionSettings &sSettings)
{
	m_CFixApplication.RegisterCB(this);
	FIX::FileStoreFactory storeFactory( sSettings );
	if (sSettings.get().has("LogType"))
	{
		try
		{
			if (sSettings.get().getLong("LogType") == 1)
			{//printf
				FIX::ScreenLogFactory logFactory( sSettings );		
				m_CSocketAcceptor  = new FIX::SocketAcceptor(m_CFixApplication, storeFactory, sSettings, logFactory);
			}
			else if (sSettings.get().getLong("LogType") == 2)
			{
				FIX::FileLogFactory fileLogFactory( sSettings );
				m_CSocketAcceptor  = new FIX::SocketAcceptor(m_CFixApplication, storeFactory, sSettings, fileLogFactory);
			}
			else
			{
				m_CSocketAcceptor  = new FIX::SocketAcceptor(m_CFixApplication, storeFactory, sSettings);
			}
		}
		catch (FIX::ConfigError& e)
		{
			printf("SocketAcceptor error:%s\n", e.what());
		}
		catch ( ... )
		{
			printf("SocketAcceptor error");
		}

	}
	else
	{
		m_CSocketAcceptor  = new FIX::SocketAcceptor(m_CFixApplication, storeFactory, sSettings);
	}

	m_CSocketAcceptor->RegisterCB(this);
	m_CSessionMap.clear();
	
	m_ClOrdIDSessionIDMap = new ClOrdIDSessionIDMap();
	m_FixEngineErrMsgMap  = new FixEngineErrMsgMap();
	InitErrMsgMap(m_FixEngineErrMsgMap);
}

CFixServerSession::~CFixServerSession()
{

	if (NULL != m_CSocketAcceptor)
	{
		delete m_CSocketAcceptor;
		m_CSocketAcceptor = NULL;
	}

	if (NULL != m_ClOrdIDSessionIDMap)
	{
		delete m_ClOrdIDSessionIDMap;
		m_ClOrdIDSessionIDMap = NULL;
	}

	if (NULL != m_FixEngineErrMsgMap)
	{
		delete m_FixEngineErrMsgMap;
		m_FixEngineErrMsgMap = NULL;
	}
}

bool CFixServerSession::IsFixServerStarted()
{
	return m_FixServerStarted;
}
//处理从下游上发的消息
void CFixServerSession::HandleMessage( FIX::IntFIXMsgType type, FIX::Message msg, FIX::SessionNo sessionID)
{
	//如果是报单，保存ClOrdID与sessionID的关系
	std::string msgType = msg.getHeader().getField(FIX::FIELD::MsgType);
	if (msgType == "D")
	{
		std::string ClOrdID = msg.getField(FIX::FIELD::ClOrdID);
		if (!ClOrdID.empty())
		{
			if (!SetOrderInfo(ClOrdID.c_str(), sessionID))
			{
				//fixme:: rturn duplicate order
				printf("收到重复的报单：%s\n", ClOrdID.c_str());
				SendBusinessRejectMessage(&msg, sessionID, ERR_FIX_ENGINE_DUPLICATE_ORDER, RefMsgType_D, BusinessRejectReason_0);
				return;
			}
		}
	}

	if (m_CFixServerSessionCB)
	{
		m_CFixServerSessionCB->HandlePackage(type, msg, sessionID);
	}
}

bool CFixServerSession::SetOrderInfo(const char *pClOrdID, FIX::SessionNo sessionID)
{
	ClOrdIDSessionIDMapIter iter = m_ClOrdIDSessionIDMap->find(std::string(pClOrdID));
	if (iter == m_ClOrdIDSessionIDMap->end())
	{
		m_ClOrdIDSessionIDMap->insert(make_pair(std::string(pClOrdID), sessionID));
		return true;
	}
	return false;
}
//错误的登录
void CFixServerSession::OnErrLogonRsp(const char *pUser, int iErrCode)
{
	FIX::Message rsp;
	rsp.getHeader().setField(FIX::BeginString(STRING_QUICKFIX_VERSION_42));
	rsp.getHeader().setField(FIX::MsgType(FIX::MsgType_Logout));
	rsp.getHeader().setField(FIX::SenderCompID(QDAM_TARGET_COMP_ID));
	rsp.getHeader().setField(FIX::TargetCompID(pUser));
	rsp.setField(FIX::Text(GetErrMsg(iErrCode).c_str()));

	SendMsg2FixClient(rsp);
}

void CFixServerSession::OnErrLogonRsp( FIX::Message msg, FIX::SessionNo sessionID )
{

}

void CFixServerSession::StartServer()
{
	if (m_FixServerStarted) return;
	m_CSocketAcceptor->start();
	m_FixServerStarted = true;
}

void CFixServerSession::OnRspLogon( const char *pBrokerID, const char *pUser, const char *pPwd, int HeartBtInt )
{
	FIX::Message rsp;
	rsp.getHeader().setField(FIX::BeginString(STRING_QUICKFIX_VERSION_42));
	rsp.getHeader().setField(FIX::MsgType(FIX::MsgType_Logon));
	rsp.getHeader().setField(FIX::SenderCompID(QDAM_TARGET_COMP_ID));
	rsp.getHeader().setField(FIX::TargetCompID(pUser));
	rsp.setField(FIX::EncyptMethod('0'));
	rsp.setField(FIX::HeartBtInt(HeartBtInt));

	SendMsg2FixClient(rsp);
}

//维护一个FIX::SessionNo 和 int sessionID的关系
int CFixServerSession::GetSSID( std::string sessionID )
{
	int ssID;
	
	CSessionIdMapIter iter = m_CSessionMap.find(sessionID);
	if (iter == m_CSessionMap.end())
	{
		ssID = m_iMaxSsID++;
		m_CSessionMap.insert(make_pair(sessionID, ssID));
	}
	else
	{
		ssID = iter->second;
	}

	return ssID;
}

void CFixServerSession::onConnect(  )
{
	printf("test\n");
}
void CFixServerSession::onDisconnect( FIX::Session* pSession )
{
	if (NULL == pSession) return;
	printf("CFixServerSession::onDisconnect:%s\n", pSession->getSessionID().toString().c_str());

	//发送logout给qtrade
	FIX::Message rsp;
	rsp.getHeader().setField(FIX::BeginString(STRING_QUICKFIX_VERSION_42));
	rsp.getHeader().setField(FIX::MsgType(FIX::MsgType_Logout));
	rsp.getHeader().setField(FIX::SenderCompID(pSession->getSessionID().getTargetCompID().getValue().c_str()));
	rsp.getHeader().setField(FIX::TargetCompID(pSession->getSessionID().getSenderCompID().getValue().c_str()));
	rsp.getHeader().setField(FIX::OnBehalfOfCompID(pSession->getBrokerID().c_str()));
	rsp.setField(FIX::Text("Session disconnect"));

	if (m_CFixServerSessionCB)
	{
		m_CFixServerSessionCB->HandlePackage(FIX::IntType_Req_Logout_5, rsp, pSession->getSessionID());
	}
}

//msgtype=8
void CFixServerSession::SendExecutionReportMessage(FIX::Message* pMsg, const char *pClOrdID)
{
	if (NULL == pMsg) return;
	//先利用ClOrdID查询对应的报单，需要找到对应的sessionID
	ClOrdIDSessionIDMapIter iter = m_ClOrdIDSessionIDMap->find(std::string(pClOrdID));
	if (iter ==  m_ClOrdIDSessionIDMap->end())
	{//没有找到对应的报单
		m_ClOrdIDSessionIDMap->insert(make_pair(std::string(pClOrdID), FIX::SessionNo(STRING_QUICKFIX_VERSION_42, QDAM_TARGET_COMP_ID, 
																					pMsg->getHeader().getField(FIX::FIELD::TargetCompID))));
	}
	pMsg->getHeader().setField(FIX::MsgType(FIX::MsgType_ExecutionReport));
	SendMsg2FixClient(*pMsg);
}

//发送错误信息
void CFixServerSession::SendBusinessRejectMessage( FIX::Message *pMsg, FIX::SessionNo sessionID, int iErrCode, char* pRefMsgType, int iBusinessRejectReason)
{
	if ( NULL == pMsg ) return;
	FIX::Message msg;
	char tmpbuf[128]={0};
	//header
	msg.getHeader().setField(sessionID.getBeginString());
	msg.getHeader().setField(FIX::MsgType(FIX::MsgType_BusinessMessageReject));
	msg.getHeader().setField(sessionID.getSenderCompID());
	msg.getHeader().setField(sessionID.getTargetCompID());

	//body
	msg.setField(FIX::Account(pMsg->getField(FIX::FIELD::Account).c_str()));
	msg.setField(FIX::ClOrdID(pMsg->getField(FIX::FIELD::ClOrdID).c_str()));
	msg.setField(FIX::RefSeqNum(atoi(pMsg->getHeader().getField(FIX::FIELD::MsgSeqNum).c_str())));
	msg.setField(FIX::Text(GetErrMsg(iErrCode).c_str()));
	msg.setField(FIX::RefMsgType(pRefMsgType));
	sprintf(tmpbuf, "%d", iErrCode);
	msg.setField(FIX::BusinessRejectRefID(tmpbuf));
	msg.setField(FIX::BusinessRejectReason(iBusinessRejectReason));
	SendMsg2FixClient(msg);
}

void CFixServerSession::SendMsg2FixClient( FIX::Message &rsp )
{
	try
	{
		//判断是否已经登录
		FIX::Session::sendToTarget(rsp);
	}
	catch (FIX::SessionNotFound& e)
	{
		printf("订单发送失败:%s\n", e.what());
	}
	catch (...)
	{
		printf("订单发送失败:其他错误\n");
	}

}

void CFixServerSession::InitErrMsgMap(FixEngineErrMsgMap *pErrMsgMap)
{
	pErrMsgMap->clear();
	pErrMsgMap->insert(make_pair(ERROR_DUPLICATE_ORDER, std::string("Duplicate order")));
	pErrMsgMap->insert(make_pair(ERR_FIX_ENGINE_DUPLICATE_ORDER, std::string("Duplicate order")));
	pErrMsgMap->insert(make_pair(ERR_FIX_ENGINE_UNSUPPORT_ORDER_TYPE, std::string("Unsupport order type")));
	pErrMsgMap->insert(make_pair(ERR_FIX_ENGINE_INVAILD_USER_ID, std::string("User or passwd invaild")));
	pErrMsgMap->insert(make_pair(ERROR_INSTRUMENT_NOT_FOUND, std::string("SecurityID invaild")));
	pErrMsgMap->insert(make_pair(ERR_FIX_ENGINE_ORDER_CANCEL_FAIL, std::string("Order cancel fail")));
	pErrMsgMap->insert(make_pair(ERR_FIX_ENGINE_OTHER_ORDER, std::string("Other error")));
	
	pErrMsgMap->insert(make_pair(ERROR_USER_MISMATCH, std::string("UserID not found")));
	pErrMsgMap->insert(make_pair(ERROR_USER_NOT_LOGIN, std::string("The user is not login")));
	pErrMsgMap->insert(make_pair(ERROR_ADUSER_NOT_RELATED, std::string("Congenial relationship does not exist")));
	pErrMsgMap->insert(make_pair(ERROR_USER_INVESTOR_NOT_RELATED, std::string("User investor relationship does not exist")));
	pErrMsgMap->insert(make_pair(ERROR_DEFULT_TRADER_NOT, std::string("Default trader does not exist")));
	pErrMsgMap->insert(make_pair(ERROR_ADUSER_NOT_RIGHT, std::string("The investment manager is not to place an order")));
	pErrMsgMap->insert(make_pair(ERROR_ADVICEORDER_ALREADY_EXITE, std::string("Duplicate order")));
	pErrMsgMap->insert(make_pair(ERROR_ADVICEORDER_NOT_EXITE, std::string("The order does not exist")));
	pErrMsgMap->insert(make_pair(ERROR_ADVICEORDER_NOT_ALLOW, std::string("The order status is not the investment manager through")));
	pErrMsgMap->insert(make_pair(ERROR_ORDER_COUNT_OVER_MAX, std::string("Quantity in excess of order number")));
	pErrMsgMap->insert(make_pair(ERROR_CAN_NOT_FIND_ACCOUNT, std::string("Can not found account")));
	pErrMsgMap->insert(make_pair(ERROR_PRICE_OUT_OF_LOWER_BOUND, std::string("Price out of lower bound")));
	pErrMsgMap->insert(make_pair(ERROR_PRICE_OUT_OF_UPPER_BOUND, std::string("Price out of upper bound")));

	
}

std::string CFixServerSession::GetErrMsg(int iErrCode)
{
	FixEngineErrMsgMapIter iter = m_FixEngineErrMsgMap->find(iErrCode);
	if (iter == m_FixEngineErrMsgMap->end())
	{
		return std::string("Other error");
	}
	return iter->second;
}

void CFixServerSession::SendOrderCancelReject( FIX::Message* pMsg )
{
	if (NULL == pMsg) return;
	pMsg->getHeader().setField(FIX::MsgType(FIX::MsgType_OrderCancelReject));
	SendMsg2FixClient(*pMsg);
}
