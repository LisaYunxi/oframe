/////////////////////////////////////////////////////////////////////////
///@system OutFogServiceApiWrapper
///OutFogServiceApiWrapper.h
///������C++ʵ��C��api��һ���򵥵Ľ��ּ�
///@history 
///20190909	���һ�	�������ļ�
/////////////////////////////////////////////////////////////////////////

#include "OutFogServiceApiWrapper.h"

void _OutFogRspField(void* pInstance,TOutFogMetaRef* pOutFogRspRef,void *pStream)
{
	((COutFogServiceApi*)pInstance)->getServiceCallback()->RspField(pOutFogRspRef,pStream);
}

