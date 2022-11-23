#ifndef AFX_READFLOW_H_
#define AFX_READFLOW_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "platform.h"

//定义流的最大大小，关系清晰

//内存流里面每个索引的大小，临时申请的大小，必须是2的n次方
#define CFNV_BLOCK_SIZE		0x100000
//内存流里面一共有多少个索引块，这个是一次性申请的
#define CFNV_BUCKET_SIZE	0x100
//内存流里面的一共索引的大小
//#define CFNV_MAX_NODE		0x7fffffff
//#define CFNV_MAX_NODE		0x10000000
#define CFNV_MAX_NODE		(CFNV_BLOCK_SIZE*CFNV_BUCKET_SIZE)

//内存里面的最大个数只能是最大索引的一半
#define CFNV_MAX_OBJECTS	(CFNV_MAX_NODE/2)

//长文件流的每个流的报文个数
#define LONGFILEFLOWGAP 100000

//定义流水的种类

#define FLOW_TYPE_FILEFLOW			1
#define FLOW_TYPE_CACHEDFLOW		2
#define FLOW_TYPE_LOCKFLOW			3
#define FLOW_TYPE_PUBFLOW			4
#define FLOW_TYPE_DIALOGFLOW		5
#define FLOW_TYPE_ONETIMEFLOW		6
#define FLOW_TYPE_INDEXFLOW			7

//CReadFlow是一种对象容器（流）,流中的对象可以按序号读出。
//CReadFlow只定义的操作接口，需要派生类中实现具体的方法
class CReadFlow
{
public:
	CReadFlow() {};
	virtual ~CReadFlow() {};

	//获取流中已经有的对象的个数
	// @return 对象的个数
	virtual UF_INT8 GetCount() = 0;

	//获取指定编号的对象
	// @param	id	对象序号
	// @param pObject 获取对象的缓冲区
	// @param length 缓冲区大小
	// @return 取出对象的长度
	virtual int Get(UF_INT8 id, void *pObject, int length) = 0;

	virtual void* GetPointer(UF_INT8 id, int& length) = 0;

	//是否一次性读取有效
	virtual int GetFlowType() = 0;

	virtual UF_INT8 GetFirstID() = 0;

	virtual const char* GetName() = 0;
};

#endif
