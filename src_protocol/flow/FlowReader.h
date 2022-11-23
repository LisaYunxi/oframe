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

//CFlowReader���������Ķ�ȡ�ĵ�����
class CFlowReader
{
public:
	//SetIDʱ�õļ������λ�õı�ʶ��������fseek���õ�SEEK_XXX��
	enum SeekFlag
	{
		FROM_HEAD,
		FROM_HERE,
		FROM_END
	};
public:
	CFlowReader();
	virtual ~CFlowReader();

	//�ҽӵ�һ������
	// @param	pFlow	Ҫ��ȡ��CReadFlow
	// @param	startId	��ʼ��ȡ�ı�ţ������ſ��Դ���ĿǰpFlow�е������
	// @return -1: attchʧ��
	int AttachFlow(CReadFlow *pFlow, UF_INT8 nStartId, UF_INT8 nBusinessNo=UF_INT8_MAX, int nTimes = 1);

	//ж�¹ҽӵ�����
	// @return pFlow	ԭ���ҽӵ���
	CReadFlow *DetachFlow();

	//������һ��Ҫ��ȡ�ı��
	// @param	id	��һ��Ҫ��ȡ�ı�ţ������ſ��Դ���ĿǰpFlow�е������
	// @param	from	��ŵļ��㷽����������
	// FROM_HEAD	��ͷ����
	// FROM_HERE	�ӵ�ǰλ�ü���
	// FROM_END	��β����
	void SetId(UF_INT8 id, SeekFlag from = FROM_HEAD);

	//��CReadFlow���Ƿ��ж����ɶ�
	// @return	true ��ʾ���У�false ��ʾû��
	inline bool Available();

	//��ȡ��CReadFlow���Ѿ��еĶ������
	// @return	�������
	inline UF_INT8 GetCount();

	//��ȡ��reader��������subject
	// @return	���ظ�Reader��������Subject
	inline CReadFlow *GetFlow();

	//��ȡ��һ��Ҫ��ȡ�ı��
	// @return  ��һ��Ҫ���Ķ���ı��
	inline UF_INT8 GetId();
	inline UF_INT8* GetIdPtr();
	inline UF_INT8 GetBusinessNo();
	inline void SetBusinessNo(UF_INT8 nBusinessNo);
	inline UF_INT8 GetUnRead();

	//���ҽӵ����е���һ����ȡ��ָ���İ�������
	// @param pPackage ���ڴ���ȡ�����ݵİ�����
	// @return true ��ȡ�ɹ���false ��ȡʧ��
	// @remark ���������㹻�Ŀռ�
	bool GetNext(CPackage *pPackage);
	//��ȡ�����İ���������Valid����
	int GetNextBase(CPackage *pPackage);

	//������ˮ��ȡ����
	int GetNext(void *pObject, int length);
	//OneTimeFlow��ȡ����
	void* GetNext(int& length);

protected:
	CReadFlow *m_pFlow;			///�洢����������
	UF_INT8 m_nNextId;				///һ��Ҫ���Ķ���ı��
	int m_nTimes;				//ÿ�ζ����ٸ���ȡ���һ��
	UF_INT8 m_nBusinessNo;		//��ȡ�ĸ�ҵ��ſ�ʼ
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
