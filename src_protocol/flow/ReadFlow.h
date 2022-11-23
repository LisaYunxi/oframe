#ifndef AFX_READFLOW_H_
#define AFX_READFLOW_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "platform.h"

//������������С����ϵ����

//�ڴ�������ÿ�������Ĵ�С����ʱ����Ĵ�С��������2��n�η�
#define CFNV_BLOCK_SIZE		0x100000
//�ڴ�������һ���ж��ٸ������飬�����һ���������
#define CFNV_BUCKET_SIZE	0x100
//�ڴ��������һ�������Ĵ�С
//#define CFNV_MAX_NODE		0x7fffffff
//#define CFNV_MAX_NODE		0x10000000
#define CFNV_MAX_NODE		(CFNV_BLOCK_SIZE*CFNV_BUCKET_SIZE)

//�ڴ������������ֻ�������������һ��
#define CFNV_MAX_OBJECTS	(CFNV_MAX_NODE/2)

//���ļ�����ÿ�����ı��ĸ���
#define LONGFILEFLOWGAP 100000

//������ˮ������

#define FLOW_TYPE_FILEFLOW			1
#define FLOW_TYPE_CACHEDFLOW		2
#define FLOW_TYPE_LOCKFLOW			3
#define FLOW_TYPE_PUBFLOW			4
#define FLOW_TYPE_DIALOGFLOW		5
#define FLOW_TYPE_ONETIMEFLOW		6
#define FLOW_TYPE_INDEXFLOW			7

//CReadFlow��һ�ֶ�������������,���еĶ�����԰���Ŷ�����
//CReadFlowֻ����Ĳ����ӿڣ���Ҫ��������ʵ�־���ķ���
class CReadFlow
{
public:
	CReadFlow() {};
	virtual ~CReadFlow() {};

	//��ȡ�����Ѿ��еĶ���ĸ���
	// @return ����ĸ���
	virtual UF_INT8 GetCount() = 0;

	//��ȡָ����ŵĶ���
	// @param	id	�������
	// @param pObject ��ȡ����Ļ�����
	// @param length ��������С
	// @return ȡ������ĳ���
	virtual int Get(UF_INT8 id, void *pObject, int length) = 0;

	virtual void* GetPointer(UF_INT8 id, int& length) = 0;

	//�Ƿ�һ���Զ�ȡ��Ч
	virtual int GetFlowType() = 0;

	virtual UF_INT8 GetFirstID() = 0;

	virtual const char* GetName() = 0;
};

#endif
