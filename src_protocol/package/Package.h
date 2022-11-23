#ifndef _PACKAGE_H__
#define _PACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif

//实际使用包的最大报文容量
#define PACKAGE_MAX_SIZE		3560
#define PACKAGE_RESERVE_SIZE	512

//CPackageBuffer定义CPackage中数据真正的存放地方，并记录
//了CPackage对象，对数据缓冲取的引用个数

class  CPackageBuffer
{
public:
	//构造函数
	// @param length定义了数据缓冲区的大小 
	// @remark 使用该构造函数，缓冲区的生命周期由本对象控制
	CPackageBuffer(int length);
	
	//构造函数
	// @param pData 已分配的缓冲区地址
	// @param length 定义了数据缓冲区的大小 
	// @remark 使用该构造函数，本对象不管理缓冲区的生命周期
	CPackageBuffer(char *pData, int length);

	virtual ~CPackageBuffer();
	
	//返回缓冲区的首地址
	// @return  缓冲区的地址
	inline char *Data();
	
	//返回缓冲区的长度
	// @return  缓冲区的长度
	inline int Length();
	
	//增加对这个缓冲区的引用
	void AddRef();
	
	//释放这个缓冲区
	void Release();

protected:
	char *m_pData;			//指向缓冲区数据的指针
	int m_nLength;			//缓冲区长度
	int m_nRefCount;		//缓冲区的引用个数
	bool m_bReleaseData;	//是否管理缓冲区的生命周期
};

inline char *CPackageBuffer::Data()
{
	return m_pData;
}

inline int CPackageBuffer::Length()
{
	return m_nLength;
}

class  CPackage  
{
public:
	CPackage();
	virtual ~CPackage();
	
	//清空数据包
	void Clear();

	//数据包上传时，上层协议的编号，子类必须实现此函数
	// @return 上层协议的编号
	virtual unsigned int GetActiveID();

	//检查包内数据是否构成一个完整的包。若有多余的数据，则截掉。
	// @return >=0 是一个完整的包 
	// @return -1 数据包不完整
	// @return <-1 数据包有错误
	// @remark 派生类一般要重载该函数，实现自已对数据的校验
	virtual int ValidPackage();

	//组装数据包
	// @return 0 组装成功
	// @return <0 组装失败
	// @remark 派生类一般要重载该函数，完成安装包头等工作
	virtual int MakePackage();

	//指向PackageBuffer缓冲区的指针m_pHead前移size长度
	// @param size 指针移动的大小
	// @return 返回移动m_pHead后的地址
	char *Push(int size);
	
	//指向PackageBuffer缓冲区的指针m_pHead后移size长度
	// @param size 指针移动的大小
	// @return 返回移动m_pHead后的地址
	char *Pop(int size);
	
	//缓冲区中存放数据的长度
	// @return 存放数据的长度
	inline int Length();
	
	//缓冲区中存放数据的地址
	// @return 存放数据的地址
	inline char *Address();
	
	//将另一个包的缓冲区关联到本包中
	// @param pPackage 指向源包的指针
	void BufAddRef(CPackage *pPackage);
	
	//脱离本包对Buff的引用
	// @return 返回Buff的地址
	CPackageBuffer *BufRelease();
	
	//实际为package分配空间
	// @param nCapacity package的可用空间大小
	// @param nReserve package保留空间大小
	void ConstructAllocate(int nCapacity=PACKAGE_MAX_SIZE, int nReserve=PACKAGE_RESERVE_SIZE);

	//得到package的所有头的长度，如果有
	int GetHeaderLength();

	//从关联的PackagBuffer上分配一块空间。
	// 制定长度分配，用于从流水里面读取制定类别的package的报文之后还需要添加报头的行为
	// @param length 分配空间的大小
	// @return 分配空间的首地址。若未关联PackgeBuffer或PackageBuffer空间不足，则返回NULL
	char* Allocate(int length);

	//需要添加数据到package里面之前的初始化，Head需要留有空间给报文头
	char* AllocateToAdd();

	//从流水里面读取到Package里面的初始化，Head需要留一部分头部空间
	char* AllocateToRead();

	//缓冲区尾部截去一段
	// @param newLength 缓冲区尾部截去的长度
	void Truncate(int newLength);
	
	//减少对本包的引用，当引用为0时，析构本对象
	void Release();
	
	//增加对本包的引用
	void AddRef();
	
	//将源包的数据拷贝到本包中
	// @param pSrcPackage 源包
	// @remark 本包必须已经关联了足够的空间
	void DupPackage(CPackage *pSrcPackage);

	void DupData(CPackage *pSrcPackage);

	//因为pHead已经放到了数据区的最前面
	//attch之后不能makepackage，只能vilidpackage
	//这个报文不能发送
	void AttachBuffer(CPackageBuffer *pPackageBuffer);

	//设置包中的数据
	// @param pData 指向数据的指针
	// @param nDataLen 数据的长度
	// @remark 该函数不会产生PackageBuffer，只是设置了头、尾指针，
	// @remark 因此，本包只能用于读取，不能用于写入数据
	void SetData(char *pData, int nDataLen);

	char* OutputHeader(char* header);

	CPackageBuffer* GetPackageBuffer();

protected:
	CPackageBuffer *m_pPackageBuffer;   /// 指向数据缓冲区的指针
	char *m_pHead;						/// 指向数据缓冲区的头指针
	char *m_pTail;						/// 指向数据缓冲区的尾指针
	int m_nRefCount;					/// 对本包的引用个数
	int m_nReserve;						/// 保留空间大小，供报文头使用
	int m_nHeadLength;					/// 报文头的长度
	int m_nCapacity;					/// 包的容量
};

inline int CPackage::Length()
{
	return (int)(m_pTail-m_pHead);
}

inline char *CPackage::Address()
{
	return m_pHead;
}

#endif
