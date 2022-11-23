# Microsoft Developer Studio Project File - Name="omqsapi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=omqsapi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "omqsapi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "omqsapi.mak" CFG="omqsapi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "omqsapi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "omqsapi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "omqsapi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP   -DDEBUG_NOT_LOG -DOUTFOGSERVICE_API_IS_LIB -DLIB_OUTFOGSERVICE_API_EXPORT -nologo -EHsc -MT -O2 -GF -FD   -W3 -DWINDOWS -DMSVC -DISLIB /c
# ADD CPP   -DDEBUG_NOT_LOG -DOUTFOGSERVICE_API_IS_LIB -DLIB_OUTFOGSERVICE_API_EXPORT -nologo -EHsc -MT -O2 -GF -FD   -W3 -DWINDOWS -DMSVC -DISLIB   -I..\..\src_base\function\. -I..\..\src_base\datastruct\. -I..\..\src_base\thread\. -I..\..\src_base\xml\. -I..\..\src_base\logger\. -I..\..\src_base\crypt\. -I..\..\src_base\mongoose\. -I..\..\libs\openssl\1.1.0h\include\. -I..\..\src_network\network\. -I..\..\src_network\network_tcp\. -I..\..\src_network\network_udp\. -I..\..\src_network\network_multicast\. -I..\..\src_network\reactor\. -I..\..\src_protocol\flow\. -I..\..\src_protocol\protocol\. -I..\..\src_protocol\protocol_channel\. -I..\..\src_protocol\protocol_subscribe\. -I..\..\src_protocol\protocol_link\. -I..\..\src_protocol\protocol_ofp\. -I..\..\src_protocol\connect_tcp\. -I..\..\src_protocol\protocol_compress\. -I..\..\src_protocol\protocol_sequence\. -I..\..\src_protocol\protocol_guard\. -I..\..\src_protocol\protocol_hotline\. -I..\..\src_protocol\protocol_ftd\. -I..\..\src_protocol\package\. -I..\..\src\sequence\. -I..\..\src\fronts\. -I..\..\src\linkmanager\. -I..\..\src\linkmanager\offerlink\. -I..\..\libs\ofserviceapi\1.0\include\. -I..\..\libs\ofserviceapi\ofapibase\. -I..\..\src\omqsapi\. /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib   /libpath:..\..\libs\openssl\1.1.0h\static\windows_x64 crypto.lib ssl.lib Crypt32.lib   -nologo /dll    
# ADD LINK32   -nologo /dll     kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib   /libpath:..\..\libs\openssl\1.1.0h\static\windows_x64 crypto.lib ssl.lib Crypt32.lib /out:".\.\omqsapi.dll"  /implib:".\.\omqsapi.lib"

!ELSEIF  "$(CFG)" == "omqsapi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP   -DDEBUG_NOT_LOG -DOUTFOGSERVICE_API_IS_LIB -DLIB_OUTFOGSERVICE_API_EXPORT -nologo -EHsc -MTd -Od -Gm -Zi -FD -Zm200   -W3 -DDEBUG -DDEBUG_LOG -DWINDOWS -DMSVC -DISLIB  /c
# ADD CPP   -DDEBUG_NOT_LOG -DOUTFOGSERVICE_API_IS_LIB -DLIB_OUTFOGSERVICE_API_EXPORT -nologo -EHsc -MTd -Od -Gm -Zi -FD -Zm200   -W3 -DDEBUG -DDEBUG_LOG -DWINDOWS -DMSVC -DISLIB   -I..\..\src_base\function\. -I..\..\src_base\datastruct\. -I..\..\src_base\thread\. -I..\..\src_base\xml\. -I..\..\src_base\logger\. -I..\..\src_base\crypt\. -I..\..\src_base\mongoose\. -I..\..\libs\openssl\1.1.0h\include\. -I..\..\src_network\network\. -I..\..\src_network\network_tcp\. -I..\..\src_network\network_udp\. -I..\..\src_network\network_multicast\. -I..\..\src_network\reactor\. -I..\..\src_protocol\flow\. -I..\..\src_protocol\protocol\. -I..\..\src_protocol\protocol_channel\. -I..\..\src_protocol\protocol_subscribe\. -I..\..\src_protocol\protocol_link\. -I..\..\src_protocol\protocol_ofp\. -I..\..\src_protocol\connect_tcp\. -I..\..\src_protocol\protocol_compress\. -I..\..\src_protocol\protocol_sequence\. -I..\..\src_protocol\protocol_guard\. -I..\..\src_protocol\protocol_hotline\. -I..\..\src_protocol\protocol_ftd\. -I..\..\src_protocol\package\. -I..\..\src\sequence\. -I..\..\src\fronts\. -I..\..\src\linkmanager\. -I..\..\src\linkmanager\offerlink\. -I..\..\libs\ofserviceapi\1.0\include\. -I..\..\libs\ofserviceapi\ofapibase\. -I..\..\src\omqsapi\. /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib   /libpath:..\..\libs\openssl\1.1.0h\static\windows_x64 crypto.lib ssl.lib Crypt32.lib   -nologo /dll   /DEBUG
# ADD LINK32   -nologo /dll   /DEBUG kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib   /libpath:..\..\libs\openssl\1.1.0h\static\windows_x64 crypto.lib ssl.lib Crypt32.lib /out:".\.\omqsapi.dll"  /implib:".\.\omqsapi.lib"

!ENDIF 

# Begin Target

# Name "omqsapi - Win32 Release"
# Name "omqsapi - Win32 Debug"


# Begin Group "function"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_base\function\BaseFunction.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\Config.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\platform.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\printinfo.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\profile.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\utility.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\utils.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\version.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\BaseFunction.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\Config.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\ExpEvaluate.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\platform.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\printinfo.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\profile.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\public.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\utility.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\utils.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\version.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\function\versionValue.h
# End Source File
# End Group

# Begin Group "datastruct"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_base\datastruct\BigDigit.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\CDate.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\CErrorEngine.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\CSequence.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\CSVParser.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\CSVToField.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\FieldDescribe.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\BigDigit.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\CDate.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\CErrorEngine.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\CSequence.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\CSVParser.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\CSVToField.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\FieldDescribe.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\HashMap.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\datastruct\UFDataType.h
# End Source File
# End Group

# Begin Group "thread"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_base\thread\Semaphore.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\thread\Thread.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\thread\Mutex.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\thread\Semaphore.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\thread\Thread.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\thread\ThreadSafeMap.h
# End Source File
# End Group

# Begin Group "xml"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_base\xml\CXML.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\xml\XMLAction.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\xml\CXML.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\xml\XMLAction.h
# End Source File
# End Group

# Begin Group "logger"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_base\logger\CFilelogService.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\logger\CLogger.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\logger\CProbeLogger.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\logger\CSyslogService.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\logger\monitorIndex.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\logger\CFilelogService.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\logger\CLogger.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\logger\CProbeLogger.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\logger\CSyslogService.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\logger\monitorIndex.h
# End Source File
# End Group

# Begin Group "crypt"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_base\crypt\cryptUtility.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\crypt\Des.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\crypt\hexEncode.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\crypt\md5Handler.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_base\crypt\cryptAll.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\crypt\cryptUtility.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\crypt\Des.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\crypt\DesEncryptAlgorithm.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\crypt\hexEncode.h
# End Source File
# Begin Source File
SOURCE=..\..\src_base\crypt\md5Handler.h
# End Source File
# End Group

# Begin Group "mongoose"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_base\mongoose\mongoose.c
# End Source File
# Begin Source File
SOURCE=..\..\src_base\mongoose\mongoose.h
# End Source File
# End Group

# Begin Group "openssl_include"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\libs\openssl\1.1.0h\include\ApiKeyUtil.cpp
# End Source File
# Begin Source File
SOURCE=..\..\libs\openssl\1.1.0h\include\ApiKeyUtil.h
# End Source File
# End Group

# Begin Group "network"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_network\network\Channel.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\Client.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\EpollReactor.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\ListenCtrl.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\NetworkFactory.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\PipeEventHandler.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\SelectReactor.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\Server.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\ServiceName.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\SocketInit.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\Channel.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\Client.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\EpollReactor.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\ListenCtrl.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\NetworkFactory.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\PipeEventHandler.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\SelectReactor.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\Server.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\ServiceName.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network\SocketInit.h
# End Source File
# End Group

# Begin Group "network_tcp"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_network\network_tcp\TcpChannel.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_tcp\TcpClient.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_tcp\TcpNetworkFactory.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_tcp\TcpServer.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_tcp\TcpChannel.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_tcp\TcpClient.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_tcp\TcpNetworkFactory.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_tcp\TcpServer.h
# End Source File
# End Group

# Begin Group "network_udp"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_network\network_udp\UdpChannel.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_udp\UdpClient.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_udp\UdpNetworkFactory.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_udp\UdpServer.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_udp\UdpChannel.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_udp\UdpClient.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_udp\UdpNetworkFactory.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_udp\UdpServer.h
# End Source File
# End Group

# Begin Group "network_multicast"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_network\network_multicast\MultiChannel.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_multicast\MultiClient.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_multicast\MultiNetworkFactory.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_multicast\MultiServer.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_multicast\MultiChannel.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_multicast\MultiClient.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_multicast\MultiNetworkFactory.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\network_multicast\MultiServer.h
# End Source File
# End Group

# Begin Group "reactor"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_network\reactor\EventDispatcher.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\reactor\EventHandler.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\reactor\EventQueue.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\reactor\Reactor.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\reactor\SimpleReactor.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\reactor\TimerHeap.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_network\reactor\EventDispatcher.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\reactor\EventHandler.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\reactor\EventQueue.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\reactor\Reactor.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\reactor\SimpleReactor.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\reactor\TimerHeap.h
# End Source File
# Begin Source File
SOURCE=..\..\src_network\reactor\TimerQueue.h
# End Source File
# End Group

# Begin Group "flow"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\flow\CachedFlow.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\CacheList.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\FileFlow.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\FlowReader.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\IndexFlow.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\OneTimeFlow.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\PubFlow.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\CachedFlow.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\CacheList.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\DialogFlow.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\FileFlow.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\Flow.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\FlowReader.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\IndexFlow.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\LockFlow.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\OneTimeFlow.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\PubFlow.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\flow\ReadFlow.h
# End Source File
# End Group

# Begin Group "protocol"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\protocol\Protocol.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol\Protocol.h
# End Source File
# End Group

# Begin Group "protocol_channel"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\protocol_channel\ChannelPackage.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_channel\ChannelProtocol.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_channel\Session.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_channel\SessionFactory.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_channel\ChannelPackage.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_channel\ChannelProtocol.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_channel\Session.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_channel\SessionFactory.h
# End Source File
# End Group

# Begin Group "protocol_subscribe"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\protocol_subscribe\SCPPackage.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_subscribe\SCPPackageDesc.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_subscribe\SCPProtocol.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_subscribe\SCPPackage.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_subscribe\SCPPackageDesc.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_subscribe\SCPProtocol.h
# End Source File
# End Group

# Begin Group "protocol_link"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\protocol_link\XMPPackage.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_link\XMPProtocol.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_link\XMPSession.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_link\XMPPackage.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_link\XMPProtocol.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_link\XMPSession.h
# End Source File
# End Group

# Begin Group "protocol_ofp"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\OrderingQ.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\PublishPort.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\XTPProtocol.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\XTPPubEndPoint.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\XTPPubSession.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\XTPSession.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\XTPSubEndPoint.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\FlowSubscriber.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\OrderingQ.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\PublishPort.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\Subscriber.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\SynFlowSubscriber.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\XTPProtocol.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\XTPPubEndPoint.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\XTPPubSession.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\XTPSession.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ofp\XTPSubEndPoint.h
# End Source File
# End Group

# Begin Group "connect_tcp"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\connect_tcp\TcpXtpProtocol.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\connect_tcp\TcpXtpSession.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\connect_tcp\XtpClientStub.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\connect_tcp\XtpServerStub.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\connect_tcp\TcpXtpProtocol.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\connect_tcp\TcpXtpSession.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\connect_tcp\XtpClient.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\connect_tcp\XtpClientStub.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\connect_tcp\XtpServer.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\connect_tcp\XtpServerStub.h
# End Source File
# End Group

# Begin Group "protocol_compress"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\protocol_compress\CompressPackage.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_compress\CompressProtocol.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_compress\CompressUtil.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_compress\CompressPackage.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_compress\CompressProtocol.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_compress\CompressUtil.h
# End Source File
# End Group

# Begin Group "protocol_sequence"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\protocol_sequence\AggrPackage.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_sequence\AggrProtocol.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_sequence\AggrSession.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_sequence\AggrSessionFactory.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_sequence\SessionAggregation.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_sequence\AggrPackage.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_sequence\AggrProtocol.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_sequence\AggrSession.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_sequence\AggrSessionFactory.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_sequence\SessionAggregation.h
# End Source File
# End Group

# Begin Group "protocol_guard"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\protocol_guard\ArbPackage.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_guard\ArbProtocol.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_guard\ArbSession.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_guard\ArbPackage.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_guard\ArbProtocol.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_guard\ArbSession.h
# End Source File
# End Group

# Begin Group "protocol_hotline"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\protocol_hotline\HotlinePackage.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_hotline\HotlineProtocol.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_hotline\HotlineSession.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_hotline\HotlinePackage.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_hotline\HotlineProtocol.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_hotline\HotlineSession.h
# End Source File
# End Group

# Begin Group "protocol_ftd"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ftd\FTDCProtocol.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ftd\FTDCPubEndPoint.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ftd\FTDCSession.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ftd\FTDCSubEndPoint.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ftd\FTDCProtocol.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ftd\FTDCPubEndPoint.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ftd\FTDCSession.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ftd\FTDCSubEndPoint.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\protocol_ftd\FTDCSubscriber.h
# End Source File
# End Group

# Begin Group "package"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src_protocol\package\FieldSet.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\package\FTDCPackage.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\package\Package.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\package\XTPPackage.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\package\FieldSet.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\package\FTDCPackage.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\package\Package.h
# End Source File
# Begin Source File
SOURCE=..\..\src_protocol\package\XTPPackage.h
# End Source File
# End Group

# Begin Group "sequence"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src\sequence\CompFlowFactory.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompInitState.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompositorApp.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompPrepareState.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompPrimaryState.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompSecondaryState.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompServer.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompSingleState.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompState.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompFlowFactory.h
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompInitState.h
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompositorApp.h
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompPrepareState.h
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompPrimaryState.h
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompSecondaryState.h
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompServer.h
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompSingleState.h
# End Source File
# Begin Source File
SOURCE=..\..\src\sequence\CompState.h
# End Source File
# End Group

# Begin Group "fronts"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src\fronts\FrontLogger.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\FtdFront.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\GatewayFront.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\MultiFront.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\ShmFront.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\WSFront.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\XHttpFront.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\FrontLogger.h
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\FtdFront.h
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\GatewayFront.h
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\MultiFront.h
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\ShmFront.h
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\WSFront.h
# End Source File
# Begin Source File
SOURCE=..\..\src\fronts\XHttpFront.h
# End Source File
# End Group

# Begin Group "linkmanager"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src\linkmanager\LinkManager.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\linkmanager\LinkManager.h
# End Source File
# End Group

# Begin Group "offerlink"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src\linkmanager\offerlink\OfferLink.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\linkmanager\offerlink\OfferLink.h
# End Source File
# End Group

# Begin Group "ofserviceapi_include"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\libs\ofserviceapi\1.0\include\OutFogServiceApi.h
# End Source File
# Begin Source File
SOURCE=..\..\libs\ofserviceapi\1.0\include\OutFogServiceApiStruct.h
# End Source File
# Begin Source File
SOURCE=..\..\libs\ofserviceapi\1.0\include\OutFogServiceCApi.h
# End Source File
# Begin Source File
SOURCE=..\..\libs\ofserviceapi\1.0\include\ServiceResponser.h
# End Source File
# End Group

# Begin Group "ofapibase"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\libs\ofserviceapi\ofapibase\OutFogServiceApiWrapper.cpp
# End Source File
# Begin Source File
SOURCE=..\..\libs\ofserviceapi\ofapibase\OutFogService.h
# End Source File
# Begin Source File
SOURCE=..\..\libs\ofserviceapi\ofapibase\OutFogServiceApiWrapper.h
# End Source File
# End Group

# Begin Group "omqsapi"
# PROP Default_Filter ""
# Begin Source File
SOURCE=..\..\src\omqsapi\FlowDispatcher.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\omqsapi\FlowManager.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\omqsapi\KernelApp.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\omqsapi\OfKernelApiImp.cpp
# End Source File
# Begin Source File
SOURCE=..\..\src\omqsapi\FlowDispatcher.h
# End Source File
# Begin Source File
SOURCE=..\..\src\omqsapi\FlowManager.h
# End Source File
# Begin Source File
SOURCE=..\..\src\omqsapi\KernelApp.h
# End Source File
# Begin Source File
SOURCE=..\..\src\omqsapi\OfKernelApiImp.h
# End Source File
# End Group

# End Target
# End Project


