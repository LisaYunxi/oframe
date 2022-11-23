#ifndef AFX_SCPPACKAGEDESC_H__
#define AFX_SCPPACKAGEDESC_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "FieldDescribe.h"

#define STID_LOGIN_REQ						0x0001			//ϵͳͬ������
#define STID_LOGIN_RSP						0x0002			//ϵͳͬ����Ӧ
#define STID_LSA_REQ						0x0003			//LSA��������
#define STID_LSA_UPDATE						0x0004			//LSA��Ϣ
#define STID_LSA_COMPLETE					0x0005			//LSA�������
#define STID_SUBSCRIBE						0x0006			//��������
#define STID_UN_SUBSCRIBE					0x0007			//ȡ������
#define STID_PUBLISH						0x0008			//������������
#define STID_ACK							0x0009			//����ȷ��

class  CSCPLoginReqField
{
public:
	CUFIntType	NodeID;				/**<�豸��*/
	CUFIntType	Metric;				/**<����*/
	CUFCharType	Type;				/**<�豸���� 'R'·���� 'T'�ն�*/
	/**���������Ե�����������д�������ձ�
	*/
	void DescribeMembers()
	{
		m_Describe.SetupMember(NodeID, (char *)&NodeID - (char *)this, "NodeID", sizeof(NodeID), "", 0);
		m_Describe.SetupMember(Metric, (char *)&Metric - (char *)this, "Metric", sizeof(Metric), "", 0);
		m_Describe.SetupMember(Type, (char *)&Type - (char *)this, "Type", sizeof(Type), "", 0);
	}
	static CFieldDescribe m_Describe;
};

class CSCPLinkStateField
{
public:
	CUFIntType NodeID;				/**< Ŀ���ַ*/
	CUFIntType Metric;				/**< ���� */
	CUFCharType Type;				/**< ���� 'T'�ն�, 'R'·��*/
	/**���������Ե�����������д�������ձ�
	*/
	void DescribeMembers()
	{
		m_Describe.SetupMember(NodeID, (char *)&NodeID - (char *)this, "NodeID", sizeof(NodeID), "", 0);
		m_Describe.SetupMember(Metric, (char *)&Metric - (char *)this, "Metric", sizeof(Metric), "", 0);
		m_Describe.SetupMember(Type, (char *)&Type - (char *)this, "Type", sizeof(Type), "", 0);
	}
	static CFieldDescribe m_Describe;
};

class CSCPLsaHeadField
{
public:
	CUFIntType NodeID;				/**<����LSA�Ľڵ�ID*/
	CUFCharType Type;				/**< ����LSA�Ľڵ����� 'T'�ն�, 'R'·��*/
	CUFIntType TimeStamp;			/**<ʱ���*/
	/**���������Ե�����������д�������ձ�
	*/
	void DescribeMembers()
	{
		m_Describe.SetupMember(NodeID, (char *)&NodeID - (char *)this, "NodeID", sizeof(NodeID), "", 0);
		m_Describe.SetupMember(Type, (char *)&Type - (char *)this, "Type", sizeof(Type), "", 0);
		m_Describe.SetupMember(TimeStamp, (char *)&TimeStamp - (char *)this, "TimeStamp", sizeof(TimeStamp), "", 0);
	}
	static CFieldDescribe m_Describe;
};

class CSCPSubscribeField
{
public:
	CUFIntType SubscriberID;		/**<�����߱�ʶ  */
	CUFIntType SubjectNo;			/**<�����ʶ*/
	CUFFloatType<24, 0> StartID;		/**<��ʼ���*/

	/**���������Ե�����������д�������ձ�
	*/
	void DescribeMembers()
	{
		m_Describe.SetupMember(SubscriberID, (char *)&SubscriberID - (char *)this, "SubscriberID", sizeof(SubscriberID), "", 0);
		m_Describe.SetupMember(SubjectNo, (char *)&SubjectNo - (char *)this, "SubjectNo", sizeof(SubjectNo), "", 0);
		m_Describe.SetupMember(StartID, (char *)&StartID - (char *)this, "StartID", sizeof(StartID), "", 0);
	}
	static CFieldDescribe m_Describe;
};

class CSCPPublishField
{
public:
	CUFIntType SubjectNo;			/**<�����ʶ*/
	CUFFloatType<24, 0> SeqNo;				/**<����������*/

	void DescribeMembers()
	{
		m_Describe.SetupMember(SubjectNo, (char *)&SubjectNo - (char *)this, "SubjectNo", sizeof(SubjectNo), "", 0);
		m_Describe.SetupMember(SeqNo, (char *)&SeqNo - (char *)this, "SeqNo", sizeof(SeqNo), "", 0);
	}
	static CFieldDescribe m_Describe;
};
#endif
