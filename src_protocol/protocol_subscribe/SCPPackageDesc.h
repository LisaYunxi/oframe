#ifndef AFX_SCPPACKAGEDESC_H__
#define AFX_SCPPACKAGEDESC_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "FieldDescribe.h"

#define STID_LOGIN_REQ						0x0001			//系统同步请求
#define STID_LOGIN_RSP						0x0002			//系统同步响应
#define STID_LSA_REQ						0x0003			//LSA发送请求
#define STID_LSA_UPDATE						0x0004			//LSA信息
#define STID_LSA_COMPLETE					0x0005			//LSA发送完毕
#define STID_SUBSCRIBE						0x0006			//订阅主题
#define STID_UN_SUBSCRIBE					0x0007			//取消订阅
#define STID_PUBLISH						0x0008			//发布主题声明
#define STID_ACK							0x0009			//主动确认

class  CSCPLoginReqField
{
public:
	CUFIntType	NodeID;				/**<设备号*/
	CUFIntType	Metric;				/**<花费*/
	CUFCharType	Type;				/**<设备类型 'R'路由器 'T'终端*/
	/**产生各属性的描述，并填写描述对照表
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
	CUFIntType NodeID;				/**< 目标地址*/
	CUFIntType Metric;				/**< 花费 */
	CUFCharType Type;				/**< 类型 'T'终端, 'R'路由*/
	/**产生各属性的描述，并填写描述对照表
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
	CUFIntType NodeID;				/**<产生LSA的节点ID*/
	CUFCharType Type;				/**< 产生LSA的节点类型 'T'终端, 'R'路由*/
	CUFIntType TimeStamp;			/**<时间戳*/
	/**产生各属性的描述，并填写描述对照表
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
	CUFIntType SubscriberID;		/**<订阅者标识  */
	CUFIntType SubjectNo;			/**<主题标识*/
	CUFFloatType<24, 0> StartID;		/**<起始序号*/

	/**产生各属性的描述，并填写描述对照表
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
	CUFIntType SubjectNo;			/**<主题标识*/
	CUFFloatType<24, 0> SeqNo;				/**<主题最大序号*/

	void DescribeMembers()
	{
		m_Describe.SetupMember(SubjectNo, (char *)&SubjectNo - (char *)this, "SubjectNo", sizeof(SubjectNo), "", 0);
		m_Describe.SetupMember(SeqNo, (char *)&SeqNo - (char *)this, "SeqNo", sizeof(SeqNo), "", 0);
	}
	static CFieldDescribe m_Describe;
};
#endif
