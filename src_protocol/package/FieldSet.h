#ifndef _FIELDSET_H__INCLUDED_
#define _FIELDSET_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "FieldDescribe.h"
#include "Package.h"

struct TFieldHeader
{
	unsigned short FieldID;
	unsigned short Size;
};

class CFieldIteratorBase
{
public:
	CFieldIteratorBase();

	CFieldIteratorBase(char *pHead, char *pEnd, CFieldDescribe *pFieldDescribe);

	void Next();

	inline bool IsEnd();

	//ȡ����ǰ���ͷ
	//@param fieldHeader ȡ���ĵ�ǰ���ͷ
	inline void RetrieveHeader(TFieldHeader &fieldHeader);
	
	inline void GetHeader();
	inline int DataRemain();

protected:
	TFieldHeader m_fieldHeader;
	CFieldDescribe *m_pFieldDescribe;
	char *m_pEnd;
	char *m_pCurr;
	char *m_pData;
};

inline bool CFieldIteratorBase::IsEnd()
{
	return m_pData == NULL;
}

inline int CFieldIteratorBase::DataRemain()
{
	return (int)(m_pEnd - m_pCurr);
}

inline void CFieldIteratorBase::RetrieveHeader(TFieldHeader &fieldHeader)
{
	fieldHeader = m_fieldHeader;
}

inline void CFieldIteratorBase::GetHeader()
{
	memcpy(&m_fieldHeader, m_pCurr, sizeof(TFieldHeader));
	CHANGE_ENDIAN(m_fieldHeader.FieldID);
	CHANGE_ENDIAN(m_fieldHeader.Size);
}

class CFieldTypeIterator : public CFieldIteratorBase
{
public:
	//���캯��
	//@param pHead �������ݵ�ͷָ��
	//@param pEnd �������ݵ�βָ��
	CFieldTypeIterator();

	CFieldTypeIterator(char *pHead, char *pEnd);

	TFieldHeader Retrieve(void *pStruct);

	//�ӻ������ж���һ������
	//@param pFieldDescribe Ҫ��ȡ���������
	//@param field ����������
	//@return 0 û�м�¼
	//@return >0  ��ȡ��¼�ĳ���
	int Retrieve(CFieldDescribe *pFieldDescribe, void *pStruct);
};

//����ָ����FieldDescribe�õ��ĵ�����
class  CNamedFieldIterator : public CFieldIteratorBase
{
public:
	//���캯��
	//@param pHead �������ݵ�ͷָ��
	//@param pEnd �������ݵ�βָ��
	//@param pFieldDescribe Ҫ���������������
	CNamedFieldIterator(char *pHead, char *pEnd, CFieldDescribe *pFieldDescribe);
	
	//�ӻ������ж���һ������
	//@param field ����������
	//@return 0 û�м�¼
	//@return >0  ��ȡ��¼�ĳ���
	int Retrieve(void *pStruct);
};

class CFieldSet : public CPackage
{
public:
	CFieldSet();
	virtual ~CFieldSet();

	//��ȡһ����
	//@param field ����һ���������͵���
	//@return 0 û�м�¼
	//@return >0  ��ȡ��¼�ĳ���
	int GetSingleField(CFieldDescribe *pFieldDescribe, void *pField);
		
	int AddField(CFieldDescribe *pFieldDescribe, void *pField);
	
	bool AddStream(unsigned short nFieldID,const char *pField,int nLength);

	void SetSize(int nSize);
	int GetSize();

	//����ָ�����������Ϣ����ȡָ����ĵ�����
	//@param field ����������͵��������ָ��
	//@return ָ����ĵ����� 
	inline CNamedFieldIterator GetNamedFieldIterator(CFieldDescribe *pFieldDescribe);

	//��ȡ�������ĵ�����
	//@param field ����������͵��������ָ��
	//@return ָ����ĵ����� 
	inline CFieldTypeIterator GetFieldTypeIterator();

	//Ϊĳ���͵������ռ�
	//@param FieldID  ������id
	//@param Size ��ĳ���
	//@return ���ط������ĵ�ַ
	char * AllocField(unsigned short wFieldID,unsigned short wSize);

	int m_nSize;//�������Field�ĸ���
};

inline CNamedFieldIterator CFieldSet::GetNamedFieldIterator(CFieldDescribe *pFieldDescribe)
{
	return CNamedFieldIterator(m_pHead, m_pTail, pFieldDescribe);
}

inline CFieldTypeIterator CFieldSet::GetFieldTypeIterator()
{
	return CFieldTypeIterator(m_pHead, m_pTail);
}

#endif