#ifndef _XTPPACKAGE_H__
#define _XTPPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 
#include "FieldSet.h"
#include "Package.h"

//#define XTP_FIELDTYPE_SIZE			10

//const unsigned char CHAIN_CONTINUE	= 'C';		//���к�����XTP����
//const unsigned char CHAIN_LAST		= 'L';		//���һ��XTP����

const unsigned char XTP_Type_REQUEST		= '1';		//����
const unsigned char XTP_Type_RESPONSE	= '2';		//Ӧ��
const unsigned char XTP_Type_PUBLISH		= '3';		//����
const unsigned char XTP_Type_LOGIN		= '4';		//��¼

//ϵͳĬ����8�ֽڶ��룬����ڲ������Ϊ4�ֽڶ������������
//����Ѱ����ֽڴӴ�С���У�������4�ֽڻ���8�ֽڣ����ܴ���
//double:8,int:4,bool:4,char:1 ����Ĵ�СΪ2��n�η�

struct	TXTPHeader
{
	//�����İ���ǰ����ַ�������Ҫ CHANGE_ENDIAN
	char	Remark[24];			// �Զ����ֶ�
	char	SubIndexName[48];	// ����Ͷ����
	char	UserID[48];			// �Զ����ֶ�
	char	Reserve[5];			// Ԥ���ֶ�
	char	Direction;			// ���ķ���
	char	Type;				// ������
	char	Chain;				// XTP���ĵ�������־
	UF_INT8	SequenceNo;			// XTP���ĵ����
	UF_INT8	GlobalNo;			// �������ı��ĵ����
	UF_INT8	BusinessNo;			// ȫ��ҵ�����
	int	FrontNo;			// ����ǰ�õı�ʶ
	int	RequestNo;			// ����ID	
	int	SessionNo;			// ����������ǰ�õĻỰID
	int	MessageNo;			// ���ĵ�id
	int	Length;				// ����ͷ֮�⣬��field���Ⱥ�
	int	SubjectNo;			// ����
//	unsigned int   DealUsec;			// ��¼���Ĵ���ʱ��
//	unsigned int	GroupID;			// �������ʶ
//	unsigned int   ServiceNo;			// ��������

	inline void Init();
	inline void ChangeEndian();

	//������������ݷ���pStream�����ı��ֽ���
	inline void ToStream(char *pStream);

	//��pStream�����ݷ��뱾���󣬲��ı��ֽ���
	inline void FromStream(char *pStream);
};

inline void TXTPHeader::Init()
{
	memset(this, 0 ,sizeof(TXTPHeader));
}

inline void TXTPHeader::ChangeEndian()
{
	CHANGE_ENDIAN(SequenceNo);
	CHANGE_ENDIAN(GlobalNo);
	CHANGE_ENDIAN(BusinessNo);
	CHANGE_ENDIAN(RequestNo);
	CHANGE_ENDIAN(SessionNo);
	CHANGE_ENDIAN(FrontNo);
//	CHANGE_ENDIAN(ServiceNo);
	CHANGE_ENDIAN(SubjectNo);
	CHANGE_ENDIAN(MessageNo);
//	CHANGE_ENDIAN(DealUsec);
	CHANGE_ENDIAN(Length);

//	CHANGE_ENDIAN(GroupID);
}

inline void TXTPHeader::ToStream(char *pStream)
{
	memcpy(pStream, this, sizeof(TXTPHeader));
	((TXTPHeader *)pStream)->ChangeEndian();
}

inline void TXTPHeader::FromStream(char *pStream)
{
	memcpy(this, pStream, sizeof(TXTPHeader));
	this->ChangeEndian();
}

const int XTPHLEN = sizeof(TXTPHeader);

/**������XTPProtocolЭ�����ʶ���Package
*/
class  CXTPPackage : public CFieldSet

{
public:
	/**���캯������ʼ����Ա������
	*/
	CXTPPackage();
		
	/**��������
	*/
	virtual ~CXTPPackage();
	
	/**��ð����ϲ�Э��ID������·��
	* @return ���ذ����ϲ�Э��ID
	*/
	//virtual unsigned int GetActiveID();
	
	/**������XTP��ͷ������Ƿ�XTP���ģ�������ÿ�����ƫ����
	* @return PROTERR_XTP_BADPACKAGAE һ������XTP����
	* @return >0 ������XTP��
	*/
	virtual int ValidPackage();
	
	/**��װ���ݰ�
	* @return 0 ��װ�ɹ�
	* @return <0 ��װʧ��
	*/
	virtual int MakePackage();

	/**��װû���Լ�buf�����ݰ�
	* @return 0 ��װ�ɹ�
	* @return <0 ��װʧ��
	*/
//	int MakeNoBufPackage();
	/**��ȡXTP����ͷָ��
	*@return XTP����ͷָ��
	*/
	inline TXTPHeader *GetXTPHeader();
		
	/**��ʼ����������
	*@param tid XTP���ĵ�id
	*@param chain XTP���ĵ�������־
	*@remark ���ø÷����󣬼������������Field
	*/
	void PrepareRequest(unsigned int tid, unsigned char chain);
	
	/**���������ĳ�ʼ����Ӧ����
	*@param pRequest ����XTP��
	*@param tid XTP���ĵ�id
	*@param chain XTP���ĵ�������־
	*@remark ���ø÷����󣬼������������Field
	*/
	void PrepareResponse(CXTPPackage *pRequest, unsigned int tid, unsigned char chain);
	
	void PrepareResponse(unsigned int SessionNo,unsigned int FrontNo,unsigned int RequestID,unsigned int tid, unsigned char chain);

	/**��ʼ���ɷ�������
	*@param tid XTP���ĵ�id
	*@param chain XTP���ĵ�������־
	*@remark ���ø÷����󣬼������������Field
	*/
	void PreparePublish(unsigned int tid, unsigned char chain);

	/**�����������ͷ
	*/
	char* OutputHeader(char* header);

	/**����һ��XTP����������ռ䡣�ռ��СΪ�����ռ䣫XTP����ļ�����
	* @param nReserve package�����ռ��С
	* @retrun ��������XTP��
	*/
	static CXTPPackage *CreatePackage(int nHeaderReserve);

protected:
	TXTPHeader m_XTPHeader;	  /**< XTP��ͷ*/
};

inline TXTPHeader *CXTPPackage::GetXTPHeader()
{ 
	return &m_XTPHeader;
}

#endif
