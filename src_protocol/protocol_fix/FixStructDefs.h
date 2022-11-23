#ifndef _FIX_STRUCT_DEFINES_H
#define _FIX_STRUCT_DEFINES_H
namespace FIX
{

#define BEGIN_STRING                              "BeginString"
#define SENDER_COMPID                             "SenderCompID"
#define SENDER_SUBID                              "SenderSubID"
#define SENDER_LOCATIONID                         "SenderLocationID"
#define TARGET_COMPID                             "TargetCompID"
#define TARGET_SUBID                              "TargetSubID"
#define HEART_BTINT                               "HeartBtInt" 
#define PASS_WORD                                 "Password"

#define STRING_QUICKFIX_VERSION_40                "FIX.4.0"
#define STRING_QUICKFIX_VERSION_41                "FIX.4.1"
#define STRING_QUICKFIX_VERSION_42                "FIX.4.2"
#define STRING_QUICKFIX_VERSION_43                "FIX.4.3"
#define STRING_QUICKFIX_VERSION_44                "FIX.4.4"
#define STRING_QUICKFIX_VERSION_50                "FIX.5.0"
#define Head_BeginString                          head.BeginString
#define Head_SenderCompID                         head.SenderCompID
#define Head_TargetCompID                         head.TargetCompID

enum
{
	INT_QUICKFIX_VERSION_NONE=0,
		INT_QUICKFIX_VERSION_40,
		INT_QUICKFIX_VERSION_41,
		INT_QUICKFIX_VERSION_42,
		INT_QUICKFIX_VERSION_43,
		INT_QUICKFIX_VERSION_44,
		INT_QUICKFIX_VERSION_50,
		INT_QUICKFIX_VERSION_MAX
};



enum IntFIXMsgType{
		IntType_None = 0,
		//user massge
		IntType_SecurityDefinition_End_Event,//1
		IntType_RequestforPosition_End_Event,//2
		IntType_RequestforMarketData_End_Event,//3
		
		//req
		IntType_Req_Logon_A, //4
		IntType_Req_Logout_5,//5
		IntType_Req_Heartbeat_0, ///6
		IntType_Req_Reject_3,///7
		IntType_Req_ResendRequest_2,///8
		IntType_Req_SequenceReset_4,///9
		IntType_Req_TestRequest_1,///10
		IntType_NewOrderSingle_D,//11
		IntType_OrderCancelRequest_F,//12
		IntType_SecurityDefinitionRequest_c,//13
		IntType_RequestforPosition_UAN,//14
		IntType_AccountDataRequest_UAR,//15
		IntType_MarketDataRequest_V,//16
		IntType_MarketDataEnablementsRequest_UZZ,//17
		IntType_AuthorizedAccountsRequest_UZS,//18
		IntType_AllowedFeatureRequest_UZX,//19
		IntType_OrderMassStatusRequest_UAF,//20
		IntType_CollateralInquiry_UBB,//21
		IntType_UserRequest_UBE,//22
		IntType_AccountRiskParametersRequest_URR,//23
		IntType_FCMCurrencyRateRequest_UZU,//24
		IntType_SelfMessage_S2A,//25
		IntType_Socket_Connect_Msg_26,
		IntType_Socket_Disconnect_Msg_27,
		IntType_SecurityStatusRequest_e,
		IntType_GatewayStatusReq_UAR,
		IntType_SecurityListReq_x,
		IntType_OrderStatusRequest_H,

		//rsp
		IntType_Logon_A,
		IntType_Logout_5,
		IntType_Heartbeat_0,
		IntType_Reject_3,
		IntType_ResendRequest_2,
		IntType_SequenceReset_4,
		IntType_TestRequest_1,
		IntType_BusinessMessageReject_j,
		IntType_ExecutionReport_8,
		IntType_OrderCancelReject_9,
		IntType_SecurityDefinition_d,
		IntType_RequestforPositionsAck_UAO,
		IntType_PositionReport_UAP,
		IntType_AccountDataReport_UAD,
		IntType_Market_DataRefresh_W,
		IntType_Market_Data_Incremental_Refresh_X,
		IntType_MarketDataReject_Y,
		IntType_AllowedMarketDataReport_UZY,
		IntType_AuthorizedAccountsReport_UZR,
		IntType_AllowedFeatureReport_UZW,
		IntType_OrderMassStatusRequestAck_UBR,
		IntType_CollateralReport_UBA,
		IntType_UserResponse_UBF,
		IntType_AccountRiskParametersReport_URP,
		IntType_FCMCurrencyRateReport_UZT,
		IntType_SecurityStatusRsp_f,
		IntType_GatewayStatusRsp_UAT,
		IntType_SecurityList_y,
		//max
		IntType_Max
};

enum{
        FIX_ORDER_ORDER_STATUS_NEW             = '0',
		FIX_ORDER_ORDER_STATUS_PARTIAL_FILL    = '1',
		FIX_ORDER_ORDER_STATUS_ALL_FILL        = '2',
		FIX_ORDER_ORDER_STATUS_CANCELED        = '4',
		FIX_ORDER_ORDER_STATUS_REPLACED        = '5',
		FIX_ORDER_ORDER_STATUS_PENDING_CANCEL  = '6',
		FIX_ORDER_ORDER_STATUS_REJECT          = '8',
		FIX_ORDER_ORDER_STATUS_SUSPENDED       = '9',
		FIX_ORDER_ORDER_STATUS_PENDING_NEW     = 'A',
		FIX_ORDER_ORDER_STATUS_EXPIRED         = 'C',
		FIX_ORDER_ORDER_STATUS_PENDING_REPLACE = 'E',
		//TT特有
		TT_ORDER_ORDER_STATUS_UNKNOWN          = 'U',
		//CQG特有
		CQG_ORDER_ORDER_STATUS_ORDER_STATUS    = 'I',
		
		//tag=20字段
		FIX_ORDER_EXEC_TRANS_TYPE_NEW          = '0',  //订单返回  
		FIX_ORDER_EXEC_TRANS_TYPE_CANCEL       = '1',  //撤单返回
		FIX_ORDER_EXEC_TRANS_TYPE_CORRECT      = '2',  //
		FIX_ORDER_EXEC_TRANS_TYPE_STATUS       = '3',  //订单状态查询返回
};
}
#endif //End of "FixStructDefs.h"