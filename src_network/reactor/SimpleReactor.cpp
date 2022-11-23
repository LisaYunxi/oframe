/////////////////////////////////////////////////////////////////////
// SimpleReactor.cpp: implementation of the CSimpleReactor class.
// 20140625  chenchao
// 简化后的Reactor，不做其他处理，直接HandleOtherTask，以提高uskernel处理速度
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "SimpleReactor.h"

CSimpleReactor::CSimpleReactor()
{
}

CSimpleReactor::~CSimpleReactor()
{

}

void CSimpleReactor::DispatchIOs()
{
	HandleOtherTask();
}

