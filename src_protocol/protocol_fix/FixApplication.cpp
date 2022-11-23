/* -*- C++ -*- */

/****************************************************************************
** Copyright (c) 2001-2014
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#else
#include "config.h"
#include "SessionNo.h"
#endif

#include "FixApplication.h"
#include "Session.h"
#include <iostream>
namespace FIX
{
/******************************************************************************
*CallBack
******************************************************************************/

void CFixApplication::onCreate( const FIX::SessionNo& sessionID)
{

}
void CFixApplication::onLogon( const Message&logon, const FIX::SessionNo& sessionID )
{
	m_pCFixApplicationCB->HandleMessage( IntType_Req_Logon_A, logon, sessionID); 
}

void CFixApplication::onLogout( const Message&logout, const FIX::SessionNo& sessionID )
{

}

void CFixApplication::fromApp( const FIX::Message& message, const FIX::SessionNo& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
  //crack( message, sessionID );
	m_pCFixApplicationCB->HandleMessage( GetMgsIntType(message.getHeader().getField( FIX::FIELD::MsgType )), message, sessionID); 
}

void CFixApplication::toApp( FIX::Message& message, const FIX::SessionNo& sessionID )
throw( FIX::DoNotSend )
{
  try
  {
    FIX::PossDupFlag possDupFlag;
    message.getHeader().getField( possDupFlag );
    if ( possDupFlag ) throw FIX::DoNotSend();
  }
  catch ( FIX::FieldNotFound& ) {}
}

void CFixApplication::fromAppSLC(const int& , const std::string& )
{

}
void CFixApplication::onLogonSLC( const SessionNo& )
{
}
void CFixApplication::onEvent( const std::string& )
{

}

FIX::IntFIXMsgType CFixApplication::GetMgsIntType( std::string msgTypeValue )
{
	FIX::IntFIXMsgType intType;
	if( msgTypeValue == "0" ) //Heartbeat
      intType = IntType_Heartbeat_0;
    else
    if( msgTypeValue == "1" ) //TestRequest
      intType = IntType_TestRequest_1;
    else
    if( msgTypeValue == "2" ) //ResendRequest
      intType = IntType_ResendRequest_2;
    else
    if( msgTypeValue == "3" ) //Reject
      intType = IntType_Reject_3;
    else
    if( msgTypeValue == "4" ) //SequenceReset
      intType = IntType_SequenceReset_4;
    else
    if( msgTypeValue == "5" ) //Logout
      intType = IntType_Logout_5;
    else
    if( msgTypeValue == "8" ) //ExecutionReport
      intType = IntType_ExecutionReport_8;
    else
    if( msgTypeValue == "9" ) //OrderCancelReject
      intType = IntType_OrderCancelReject_9;
    else
    if( msgTypeValue == "A" ) //Logon
      intType = IntType_Logon_A;
    else
    if( msgTypeValue == "D" ) //NewOrderSingle
      intType = IntType_NewOrderSingle_D;
    else
    if( msgTypeValue == "F" ) //OrderCancelRequest
      intType = IntType_OrderCancelRequest_F;
    else
    if( msgTypeValue == "H" ) //OrderStatusRequest
      intType = IntType_OrderStatusRequest_H;
    else
    if( msgTypeValue == "V" ) //MarketDataRequest
      intType = IntType_MarketDataRequest_V;
    else
    if( msgTypeValue == "W" ) //MarketDataSnapshotFullRefresh
      intType = IntType_Market_DataRefresh_W;
    else
    if( msgTypeValue == "X" ) //MarketDataIncrementalRefresh
      intType = IntType_Market_Data_Incremental_Refresh_X;
    else
    if( msgTypeValue == "Y" ) //MarketDataRequestReject
      intType = IntType_MarketDataReject_Y;
    else
    if( msgTypeValue == "c" ) //SecurityDefinitionRequest
      intType = IntType_SecurityDefinitionRequest_c;
    else
    if( msgTypeValue == "d" ) //SecurityDefinition
      intType = IntType_SecurityDefinition_d;
    else
    if( msgTypeValue == "e" ) //SecurityStatusRequest
      intType = IntType_SecurityStatusRequest_e;
    else
    if( msgTypeValue == "f" ) //SecurityStatus
      intType = IntType_SecurityStatusRsp_f;
    else
    if( msgTypeValue == "j" ) //BusinessMessageReject
      intType = IntType_BusinessMessageReject_j;

	return intType;
}

}
