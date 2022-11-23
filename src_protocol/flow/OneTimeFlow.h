#ifndef AFX_EXCACHEDFLOW_H__
#define AFX_EXCACHEDFLOW_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CachedFlow.h"

//ֻ��һ�ζ�ȡ���ڴ���
//�����ȡ����ˣ��ڴ��оͲ����м�¼����ʡ�ڴ�
//ͬʱ��ȡ���ڴ�ĵ�ַ��������ˮ�ڴ浽���ڴ棬��ʡ�ڴ�ͼ���
//��һ�����Ч����ˮʹ�÷�ʽ������ֻ��������һ�ζ�ȡ�������õĳ���

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
