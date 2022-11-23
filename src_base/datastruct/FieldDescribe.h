#ifndef _FIELDDESCRIBE_H__
#define _FIELDDESCRIBE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "UFDataType.h"
#define MAX_MEMB				80
#define MAX_FIELD_NAME_LEN		100
#define MAX_FIELD_SIZE			4096 

//ÿ���������͵ı�ʾ��
enum MemberType
{
	FT_BYTE,
	FT_DWORD,
	FT_REAL8,
	FT_LONG
};

//���Ա���Խṹ
struct TMemberDesc
{
	MemberType nType;					//��Ա����
	size_t nStructOffset;				//��Ա�����е�ƫ����
	size_t nStreamOffset;				//��Ա���ֽ����е�ƫ����
	int nSize;							//��Ա����
	char szName[MAX_FIELD_NAME_LEN];	//��Ա����
	char sShortName[MAX_FIELD_NAME_LEN];//��Ա������
	int nPrecision;						//����
};
//#define DEF_SETUP_MEMBER(type,id) 	void SetupMember(type &,int offset,const char *name,int length){	SetupMember(id,offset,name,length); }

class  CFieldDescribe
{
public:
	typedef void(*describeFunc)(); /**< ����һ��ָ�뺯������ */

	///���캯������ִ�д�����ָ�뺯��
	///FieldID ��ID
	///nStructSize ���С
	///pszFieldName ����
	///pszComment ��˵��
	///func    ָ��һ��������ָ��
	CFieldDescribe(int FieldID, int nStructSize, const char *pszFieldName, const char *pszComment, describeFunc func);

	/**��������
	*/
	~CFieldDescribe();
public:
	void SetupMember(CUFBaseCharType&, size_t offset, const char *name, int length, const char *shortname, int nPrecision)
	{
		SetupMember(FT_BYTE, offset, name, length, shortname, nPrecision);
	}
	void SetupMember(CUFBaseIntType&, size_t offset, const char *name, int length, const char *shortname, int nPrecision)
	{
		SetupMember(FT_DWORD, offset, name, length, shortname, nPrecision);
	}
	void SetupMember(CUFBaseLongType&, size_t offset, const char *name, int length, const char *shortname, int nPrecision)
	{
		SetupMember(FT_LONG, offset, name, length, shortname, nPrecision);
	}
	void SetupMember(CUFBaseFloatType&, size_t offset, const char *name, int length, const char *shortname, int nPrecision)
	{
		SetupMember(FT_REAL8, offset, name, length, shortname, nPrecision);
	}
	void SetupMember(CUFBaseStringType&, size_t offset, const char *name, int length, const char *shortname, int nPrecision)
	{
		SetupMember(FT_BYTE, offset, name, length, shortname, nPrecision);
	}

	///������ת��Ϊ�ֽ���
	/// @pStruct Ҫת���Ķ���
	/// @pStream ת�������ֽ���
	/// �ֽ����еĳ�Ա�����Ǹ�λ��ǰ
	void StructToStream(char *pStream, const char *pStruct);

	///���ֽ���ת��Ϊ����
	///@param pStruct ת���ĳ�����
	///@param pStream Ҫת�����ֽ���
	///@remark �ֽ����еĳ�Ա���������Ǹ�λ��ǰ
	void StreamToStruct(char *pStruct, const char *pStream);

	bool StructToOutput(char* pOutput, const char *pStruct);
	bool StreamToOutput(char* pOutput, const char *pStream);
	bool StructToCSV(char* pCSV, const char *pStruct);
	bool StreamToCSV(char* pCSV, const char *pStream);
	bool StructToSingleCSV(char* pCSV, const char *pStruct);
	bool StreamToSingleCSV(char* pCSV, const char *pStream);
	bool StructToJson(char* pJson, const char *pStruct);
	bool StructToShortJson(char* pJson, const char *pStruct);
	bool StreamToJson(char* pJson, const char *pStream);

	///��������ˮ��ȡ���ڲ�����
	// @param pField ת�����Ķ���
	// @param pStream Ҫת�����ֽ���
	int getSingleField(void *pField, const void *pStream);

	//���ڲ�����ת��Ϊ������ˮ
	// @param pStream ת�������ֽ���
	// @param pField Ҫת���Ķ���
	int setSingleField(void *pStream, const void *pField);

	inline int GetStructSize();

	//��ȡ��ת���ɵ��ֽ�������
	//return ������ת���ɵ��ֽ�������	
	inline int GetStreamSize();

	///���Ա����
	///return �������Ա����
	inline int GetMemberCount();

	///��ȡ����ÿ����Ա������
	/// @param index ��Ա���������ձ��е�λ��
	/// return ���س�Ա����
	inline TMemberDesc *GetMemberDesc(int index);

	inline const char *GetFieldName();
private:

	///������ͺ�����Id���ձ�
	/// @param nType ��Ա����
	/// @param nStructOffset ��Ա�����е�ƫ����
	/// @param nSize ��Ա����
	/// @param szName ��Ա����
	inline void SetupMember(MemberType nType, size_t nStructOffset, const char *szName, int nSize, const char *shortname, int nPrecision = 0);
public:
	int m_FieldID;			/**<��ID */
	//char m_szComment[MAX_FIELD_NAME_LEN];		/**<��ע�� */
	int m_nStructSize;			/**<���С*/
	int m_nStreamSize;			/**<��ת���ɵ��ֽ�������*/
private:
	int m_nTotalMember;			/**<���Ա����*/
	char m_szFieldName[MAX_FIELD_NAME_LEN];
	char m_szFieldTmp[MAX_FIELD_SIZE * 2];
	TMemberDesc m_MemberDesc[MAX_MEMB];	/**<���Ա��������*/

};

inline void CFieldDescribe::SetupMember(MemberType nType, size_t nStructOffset, const char *szName, int nSize, const char *shortname, int nPrecision)
{
	TMemberDesc *p = &m_MemberDesc[m_nTotalMember];
	p->nType = nType;
	p->nStructOffset = nStructOffset;
	p->nStreamOffset = m_nStreamSize;
	p->nSize = nSize;
	p->nPrecision = nPrecision;
	strcpy(p->szName, szName);
	if (shortname != NULL)
		strcpy(p->sShortName, shortname);
	else
		p->sShortName[0] = '\0';

	m_nStreamSize += nSize;
	m_nTotalMember++;
}

inline int CFieldDescribe::GetStructSize()
{
	return m_nStructSize;
}

inline int CFieldDescribe::GetStreamSize()
{
	return m_nStreamSize;
}

inline int CFieldDescribe::GetMemberCount()
{
	return m_nTotalMember;
}

inline TMemberDesc * CFieldDescribe::GetMemberDesc(int index)
{
	return &m_MemberDesc[index];
}

inline const char *CFieldDescribe::GetFieldName()
{
	return m_szFieldName;
}

#endif
