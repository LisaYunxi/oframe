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

	//取出当前域的头
	//@param fieldHeader 取出的当前域的头
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
	//构造函数
	//@param pHead 包内数据的头指针
	//@param pEnd 包内数据的尾指针
	CFieldTypeIterator();

	CFieldTypeIterator(char *pHead, char *pEnd);

	TFieldHeader Retrieve(void *pStruct);

	//从缓冲区中读到一个域中
	//@param pFieldDescribe 要获取域的描述字
	//@param field 这个域的引用
	//@return 0 没有记录
	//@return >0  读取记录的长度
	int Retrieve(CFieldDescribe *pFieldDescribe, void *pStruct);
};

//根据指定的FieldDescribe得到的迭代器
class  CNamedFieldIterator : public CFieldIteratorBase
{
public:
	//构造函数
	//@param pHead 包内数据的头指针
	//@param pEnd 包内数据的尾指针
	//@param pFieldDescribe 要遍历的域的描述字
	CNamedFieldIterator(char *pHead, char *pEnd, CFieldDescribe *pFieldDescribe);
	
	//从缓冲区中读到一个域中
	//@param field 这个域的引用
	//@return 0 没有记录
	//@return >0  读取记录的长度
	int Retrieve(void *pStruct);
};

class CFieldSet : public CPackage
{
public:
	CFieldSet();
	virtual ~CFieldSet();

	//获取一个域
	//@param field 传入一个具体类型的域
	//@return 0 没有记录
	//@return >0  读取记录的长度
	int GetSingleField(CFieldDescribe *pFieldDescribe, void *pField);
		
	int AddField(CFieldDescribe *pFieldDescribe, void *pField);
	
	bool AddStream(unsigned short nFieldID,const char *pField,int nLength);

	void SetSize(int nSize);
	int GetSize();

	//根据指定域的描述信息，获取指定域的迭代器
	//@param field 传入具体类型的域的类型指针
	//@return 指定域的迭代器 
	inline CNamedFieldIterator GetNamedFieldIterator(CFieldDescribe *pFieldDescribe);

	//获取整个包的迭代器
	//@param field 传入具体类型的域的类型指针
	//@return 指定域的迭代器 
	inline CFieldTypeIterator GetFieldTypeIterator();

	//为某类型的域分配空间
	//@param FieldID  域类型id
	//@param Size 域的长度
	//@return 返回分配后域的地址
	char * AllocField(unsigned short wFieldID,unsigned short wSize);

	int m_nSize;//报里面的Field的个数
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