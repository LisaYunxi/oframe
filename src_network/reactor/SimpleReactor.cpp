/////////////////////////////////////////////////////////////////////
// SimpleReactor.cpp: implementation of the CSimpleReactor class.
// 20140625  chenchao
// �򻯺��Reactor��������������ֱ��HandleOtherTask�������uskernel�����ٶ�
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

