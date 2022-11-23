#ifndef AFX_EXCACHEDFLOW_H__
#define AFX_EXCACHEDFLOW_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CachedFlow.h"

//只有一次读取的内存流
//如果读取完成了，内存中就不会有记录，节省内存
//同时获取的内存的地址不考虑流水内存到包内存，节省内存和计算
//是一种最高效的流水使用方式，但是只能适用于一次读取永不叙用的场景

class COneTimeFlow : public CCachedFlow
{
public:
	COneTimeFlow(const char * pName = "");
	virtual ~COneTimeFlow();

	virtual UF_INT8 Append(void *pObject, int length);
	virtual void* GetPointer(UF_INT8 id, int& length);
	virtual int GetFlowType();

protected:
	volatile UF_INT8 m_iHaveGetID;
};

#endif
