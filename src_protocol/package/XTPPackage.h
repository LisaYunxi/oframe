#ifndef _XTPPACKAGE_H__
#define _XTPPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 
#include "FieldSet.h"
#include "Package.h"

//#define XTP_FIELDTYPE_SIZE			10

//const unsigned char CHAIN_CONTINUE	= 'C';		//还有后续的XTP报文
//const unsigned char CHAIN_LAST		= 'L';		//最后一个XTP报文

const unsigned char XTP_Type_REQUEST		= '1';		//请求
const unsigned char XTP_Type_RESPONSE	= '2';		//应答
const unsigned char XTP_Type_PUBLISH		= '3';		//发布
const unsigned char XTP_Type_LOGIN		= '4';		//登录

//系统默认是8字节对齐，如果内部代码改为4字节对齐则出现问题
//这里把按照字节从大到小排列，不管是4字节还是8字节，都能处理
//double:8,int:4,bool:4,char:1 数组的大小为2的n次方

struct	TXTPHeader
{
	//这样的安排前面的字符串不需要 CHANGE_ENDIAN
	char	Remark[24];			// 自定义字段
	char	SubIndexName[48];	// 关联投资者
	char	UserID[48];			// 自定义字段
	char	Reserve[5];			// 预留字段
	char	Direction;			// 报文方向
	char	Type;				// 包类型
	char	Chain;				// XTP报文的连续标志
	UF_INT8	SequenceNo;			// XTP报文的序号
	UF_INT8	GlobalNo;			// 输入流的报文的序号
	UF_INT8	BusinessNo;			// 全局业务序号
	int	FrontNo;			// 交易前置的标识
	int	RequestNo;			// 请求ID	
	int	SessionNo;			// 请求发起者在前置的会话ID
	int	MessageNo;			// 报文的id
	int	Length;				// 除报头之外，各field长度和
	int	SubjectNo;			// 主题
//	unsigned int   DealUsec;			// 记录报文处理时间
//	unsigned int	GroupID;			// 订阅组标识
//	unsigned int   ServiceNo;			// 服务的序号

	inline void Init();
	inline void ChangeEndian();

	//将本对象的内容放入pStream，并改变字节序
	inline void ToStream(char *pStream);

	//将pStream的数据放入本对象，并改变字节序
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

/**定义了XTPProtocol协议层所识别的Package
*/
class  CXTPPackage : public CFieldSet

{
public:
	/**构造函数。出始化成员变量。
	*/
	CXTPPackage();
		
	/**析构函数
	*/
	virtual ~CXTPPackage();
	
	/**获得包的上层协议ID，用来路由
	* @return 返回包的上层协议ID
	*/
	//virtual unsigned int GetActiveID();
	
	/**解析出XTP报头，检查是否XTP报文，并解析每个域的偏移量
	* @return PROTERR_XTP_BADPACKAGAE 一个坏的XTP包文
	* @return >0 完整的XTP包
	*/
	virtual int ValidPackage();
	
	/**组装数据包
	* @return 0 组装成功
	* @return <0 组装失败
	*/
	virtual int MakePackage();

	/**组装没有自己buf的数据包
	* @return 0 组装成功
	* @return <0 组装失败
	*/
//	int MakeNoBufPackage();
	/**获取XTP报文头指针
	*@return XTP报文头指针
	*/
	inline TXTPHeader *GetXTPHeader();
		
	/**初始化成请求报文
	*@param tid XTP报文的id
	*@param chain XTP报文的连续标志
	*@remark 调用该方法后，即可向包中增加Field
	*/
	void PrepareRequest(unsigned int tid, unsigned char chain);
	
	/**根据请求报文初始化成应答报文
	*@param pRequest 请求XTP包
	*@param tid XTP报文的id
	*@param chain XTP报文的连续标志
	*@remark 调用该方法后，即可向包中增加Field
	*/
	void PrepareResponse(CXTPPackage *pRequest, unsigned int tid, unsigned char chain);
	
	void PrepareResponse(unsigned int SessionNo,unsigned int FrontNo,unsigned int RequestID,unsigned int tid, unsigned char chain);

	/**初始化成发布报文
	*@param tid XTP报文的id
	*@param chain XTP报文的连续标志
	*@remark 调用该方法后，即可向包中增加Field
	*/
	void PreparePublish(unsigned int tid, unsigned char chain);

	/**调试输出报文头
	*/
	char* OutputHeader(char* header);

	/**创建一个XTP包，并分配空间。空间大小为保留空间＋XTP最大报文件长度
	* @param nReserve package保留空间大小
	* @retrun 创建出的XTP包
	*/
	static CXTPPackage *CreatePackage(int nHeaderReserve);

protected:
	TXTPHeader m_XTPHeader;	  /**< XTP报头*/
};

inline TXTPHeader *CXTPPackage::GetXTPHeader()
{ 
	return &m_XTPHeader;
}

#endif
