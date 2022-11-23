// SCPPackageDesc.cpp: implementation of the CSCPPackageDesc class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "SCPPackageDesc.h"



// REGISTER_FIELD(0x0001,CSCPLoginReqField, "CSCPLoginReqField");
// REGISTER_FIELD(0x0003,CSCPLinkStateField, "CSCPLinkStateField");
// REGISTER_FIELD(0x0004,CSCPLsaHeadField, "CSCPLsaHeadField");
// REGISTER_FIELD(0x0006,CSCPSubscribeField, "CSCPSubscribeField");
// REGISTER_FIELD(0x0008,CSCPPublishField, "CSCPPublishField");

static void DescribeMemberOfCSCPLoginReqField() 
{
	CSCPLoginReqField f;
	f.DescribeMembers();
}
CFieldDescribe CSCPLoginReqField::m_Describe(0x0001,sizeof(CSCPLoginReqField),"CSCPLoginReqField","CSCPLoginReqField",&DescribeMemberOfCSCPLoginReqField);

static void DescribeMemberOfCSCPLinkStateField() 
{
	CSCPLinkStateField f;
	f.DescribeMembers();
}
CFieldDescribe CSCPLinkStateField::m_Describe(0x0003,sizeof(CSCPLinkStateField),"CSCPLinkStateField","CSCPLinkStateField",&DescribeMemberOfCSCPLinkStateField);

static void DescribeMemberOfCSCPLsaHeadField() 
{
	CSCPLsaHeadField f;
	f.DescribeMembers();
}
CFieldDescribe CSCPLsaHeadField::m_Describe(0x0004,sizeof(CSCPLsaHeadField),"CSCPLsaHeadField","CSCPLsaHeadField",&DescribeMemberOfCSCPLsaHeadField);

static void DescribeMemberOfCSCPSubscribeField() 
{
	CSCPSubscribeField f;
	f.DescribeMembers();
}
CFieldDescribe CSCPSubscribeField::m_Describe(0x0006,sizeof(CSCPSubscribeField),"CSCPSubscribeField","CSCPSubscribeField",&DescribeMemberOfCSCPSubscribeField);

static void DescribeMemberOfCSCPPublishField() 
{
	CSCPPublishField f;
	f.DescribeMembers();
}
CFieldDescribe CSCPPublishField::m_Describe(0x0008,sizeof(CSCPPublishField),"CSCPPublishField","CSCPPublishField",&DescribeMemberOfCSCPPublishField);

