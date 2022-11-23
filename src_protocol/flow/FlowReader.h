// FlowReader.h: interface for the CFlowReader class.
//////////////////////////////////////////////////////////////////////

#ifndef _FLOWREADER_H__
#define _FLOWREADER_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "ReadFlow.h"
#include "Package.h"
#include "public.h"

//CFlowReader是用于流的读取的迭代器
class CFlowReader
{
public:
	//SetID时用的计算相对位置的标识，类似于fseek中用的SEEK_XXX宏
	enum SeekFlag
	{
		FROM_HEAD,
		FROM_HERE,
		FROM_END
	};
public:
	CFlowReader();
	virtual ~CFlowReader();

	//挂接到一个流上
	// @param	pFlow	要读取的CReadFlow
	// @param	startId	开始读取的编号，这个编号可以大于目前pFlow中的最大编号
	// @return -1: attch失败
	int AttachFlow(CReadFlow *pFlow, UF_INT8 nStartId, UF_INT8 nBusinessNo=UF_INT8_MAX, int nTimes = 1);

	//卸下挂接的主题
	// @return pFlow	原来挂接的流
	CReadFlow *DetachFlow();

	//设置下一个要读取的编号
	// @param	id	下一个要读取的编号，这个编号可以大于目前pFlow中的最大编号
	// @param	from	编号的计算方法，可以是
	// FROM_HEAD	从头计算
	// FROM_HERE	从当前位置计算
	// FROM_END	从尾计算
	void SetId(UF_INT8 id, SeekFlag from = FROM_HEAD);

	//从CReadFlow中是否还有东西可读
	// @return	true 表示还有，false 表示没有
	inline bool Available();

	//获取该CReadFlow中已经有的对象个数
	// @return	对象个数
	inline UF_INT8 GetCount();

	//获取该reader所关联的subject
	// @return	返回该Reader所操作的Subject
	inline CReadFlow *GetFlow();

	//获取下一个要读取的编号
	// @return  下一个要读的对象的编号
	inline UF_INT8 GetId();
	inline UF_INT8* GetIdPtr();
	inline UF_INT8 GetBusinessNo();
	inline void SetBusinessNo(UF_INT8 nBusinessNo);
	inline UF_INT8 GetUnRead();

	//将挂接的流中的下一个包取到指定的包对象中
	// @param pPackage 用于存贮取出数据的包对象
	// @return true 获取成功，false 获取失败
	// @remark 包必须有足够的空间
	bool GetNext(CPackage *pPackage);
	//获取基础的包，不进行Valid处理
	int GetNextBase(CPackage *pPackage);

	//正常流水获取数据
	int GetNext(void *pObject, int length);
	//OneTimeFlow获取数据
	void* GetNext(int& length);

protected:
	CReadFlow *m_pFlow;			///存储所关联的流
	UF_INT8 m_nNextId;				///一个要读的对象的编号
	int m_nTimes;				//每次读多少个，取最后一个
	UF_INT8 m_nBusinessNo;		//读取哪个业务号开始
};

inline bool CFlowReader::Available()
{
	return GetCount() > m_nNextId;
}

inline UF_INT8 CFlowReader::GetCount()
{
	return m_pFlow->GetCount();
}

inline UF_INT8 CFlowReader::GetId()
{
	return m_nNextId;
}
inline UF_INT8*  CFlowReader::GetIdPtr()
{
	return &m_nNextId;
}
inline CReadFlow* CFlowReader::GetFlow()
{
	return m_pFlow;
}
inline UF_INT8 CFlowReader::GetBusinessNo()
{
	return m_nBusinessNo;
}
inline void CFlowReader::SetBusinessNo(UF_INT8 nBusinessNo)
{
	m_nBusinessNo = nBusinessNo;
}
inline UF_INT8 CFlowReader::GetUnRead()
{
	return m_pFlow->GetCount() - m_nNextId;
}

#endif
