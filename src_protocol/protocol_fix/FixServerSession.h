#ifndef _FIX_SESSION_H
#define _FIX_SESSION_H
#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 4290 )
#endif


#include "FixStructDefs.h"
#include "FixApplication.h"
//#include "FixSocketAcceptor.h"
#include "SocketAcceptor.h"
#include "SocketAcceptor.h"
#include "errorDefine.h"

///////////////////////////////////////////////////////////////////////////////
#define QDAM_TARGET_COMP_ID                        "QDAM_FRONT"

//ErrCOde
#define ERR_FIX_ENGINE_OTHER_ORDER                 90000
#define ERR_FIX_ENGINE_DUPLICATE_ORDER             90001
#define ERR_FIX_ENGINE_UNSUPPORT_ORDER_TYPE        90002
#define ERR_FIX_ENGINE_INVAILD_USER_ID             90003
#define ERR_FIX_ENGINE_INVAILD_SECURITYID_ID       90004
#define ERR_FIX_ENGINE_ORDER_CANCEL_FAIL           90005
///////////////////////////////////////////////////////////////////////////////


typedef std::map<std::string, FIX::SessionNo> ClOrdIDSessionIDMap; //key=ClOrdID
typedef ClOrdIDSessionIDMap::iterator ClOrdIDSessionIDMapIter;

//错误码:key=sErrCode, value=sErrMsg
typedef std::map<int, std::string> FixEngineErrMsgMap;
typedef FixEngineErrMsgMap::iterator FixEngineErrMsgMapIter;

class CFixServerSessionCB
{
public:
	virtual int HandlePackage(FIX::IntFIXMsgType type, FIX::Message msg, FIX::SessionNo sessionID)=0;
	
};

class CFixServerSession: public FIX::CFixApplicationCB, public FIX::CSocketAcceptorCB
{
public:
	typedef std::map<std::string, int> CSessionIdMap;
	typedef CSessionIdMap::iterator CSessionIdMapIter;
public:
	CFixServerSession(const char *pFixServerCfgFile);
	CFixServerSession(FIX::SessionSettings sSettings);
	void RegisterCB(CFixServerSessionCB *pCFixServerSessionCB)
	{
		m_CFixServerSessionCB = pCFixServerSessionCB;
	}
	~CFixServerSession();

	void StartServer();
	void OnRspLogon( const char *pBrokerID, const char *pUser, const char *pPwd, int HeartBtInt );
	void OnErrLogonRsp( const char *pUser, int iErrCode );
	void OnErrLogonRsp( FIX::Message msg, FIX::SessionNo sessionID );
	void SendExecutionReportMessage(FIX::Message* pMsg, const char *pClOrdID);
	void SendBusinessRejectMessage( FIX::Message *pMsg, FIX::SessionNo sessionID, int iErrCode, char* pRefMsgType, int iBusinessRejectReason);
	void SendOrderCancelReject( FIX::Message* pMsg );
	bool IsFixServerStarted();
	bool SetOrderInfo(const char *pClOrdID, FIX::SessionNo sessionID);
	std::string GetErrMsg(int iErrCode);
private:
	//CFixApplicationCB
	void HandleMessage( FIX::IntFIXMsgType type, FIX::Message msg, FIX::SessionNo sessionID);
	int  GetSSID( std::string sessionID );
	void InitErrMsgMap(FixEngineErrMsgMap *pErrMsgMap);
	
	
	
	//CSocketAcceptorCB
// 	void onStart();
// 	void onStop();
	void onConnect(  );
	void onDisconnect( FIX::Session* pSession );
	void SendMsg2FixClient( FIX::Message &rsp );
	void InitSession(FIX::SessionSettings &sSettings);
private:
	int                  m_iSessionNum;
	CSessionIdMap        m_CSessionMap;
	FIX::CFixApplication m_CFixApplication;
	FIX::SocketAcceptor  *m_CSocketAcceptor;
	CFixServerSessionCB  *m_CFixServerSessionCB;
	ClOrdIDSessionIDMap  *m_ClOrdIDSessionIDMap; //用来保存ClordID与sessionID之间的关系
	FixEngineErrMsgMap   *m_FixEngineErrMsgMap;
	int                  m_iMaxSsID;
	bool                 m_FixServerStarted;
};


#endif
