#ifndef AFX_FILEFLOW_H__
#define AFX_FILEFLOW_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "platform.h"
#include "Flow.h"
#include "Mutex.h"

struct SFlowFileInfo
{
	FILE *pContentFile;		//文件指针
	string FileName;		//文件名
	UF_INT8 BeginID;
};

typedef map<int,SFlowFileInfo*> VFileInfoMap;

//CLongFileFlow是文件流类，它使用两个文件来存贮包。一个文件保存包的偏移量，
// 另一个文件保存包的内容。为了提高速度，每BLOCK_SIZE个包保存一个偏移量。
// 文件名和文件所存贮的目录在构造时指定。

class CFileFlow : public CFlow  
{
public:
	//构造函数
	// @param pszFlowName 流文件的名称（真正的文件有两个，会自动加上"flow"的扩展名
	// @param pszPath 流文件存贮的目录
	// @异常  CRuntimeError	如果文件名为空或文件读写错误，则抛出此异常
	// @remark 如果重用文件，则构造时将检查文件的合法性。
	
	CFileFlow(const char *pszFlowName, const char *pszPath, bool bReUse = false,int nFlowGap=LONGFILEFLOWGAP);
	
	virtual ~CFileFlow();

	//将流尾部的包截掉
	// @param nCount 保留的包的个数
	virtual bool Truncate(UF_INT8 nCount); 
	
	//向流中增加一个对象
	// @param	pObject 要增加的对象
	// @param length 增加数据的长度
	// @return 对象在流中的序号，小于0表示增加失败
	virtual UF_INT8 Append(void *pObject, int length);

	//获取指定编号的对象
	// @param	id	对象序号
	// @param pObject 获取对象的缓冲区
	// @param length 缓冲区大小
	// @return 取出对象的长度
 	virtual int Get(UF_INT8 id, void *pObject, int length);

protected:	
	//初始化存贮包的文件
	// @return true 初始化成功  false 初始化失败
	bool InitFile(UF_INT8 nCount);
	
	//重新打开流文件
	// @param	szIdFilename	存储地址的文件名
	// @param	szContentFilename	存储内容的文件名
	// @param	bReuse	是否重用此文件
	// @异常	CRuntimeError	如果文件名为空或文件读写错误，则抛出此异常
	FILE* OpenFile(int nFileIndex);

	//获取写位置
	fpos_t GetPos(int nFileIndex);

	fpos_t GetPos(int nFileIndex, UF_INT8 id);

	//获取一个index里面最大的序列号
	UF_INT8 GetIndexMaxCount(int nFileIndex);

	//关闭存贮包的文件
	void CloseFile(SFlowFileInfo* pFlowFileInfo);
	virtual int GetFlowType();

private:
	VFileInfoMap m_flowIndex;		// 已经读到的文件的索引
	int m_nFirstIndex;				// 第一个位置
	fpos_t m_nCurrOffset;			// 最新存储文件的内容长度
	int m_nCurrIndex;				// 写入的位置

	//读文件使用
	fpos_t m_nReadOffset;			// 最近一次读取的包在文件中的偏移量
	UF_INT8 m_nReadId;				// 最近一次读取的包的序号
	int m_nReadIndex;				// 最近一次读取的文件的位置

	//CRITICAL_VAR m_criticalVar;		// 临界区变量
	CRWLock m_RWLock;
	char m_sPreFileName[128];		// 文件名前缀
	int m_nLongFileFlowGap;			//多少gap一个文件
	char m_sError[512];				//错误消息临时变量
};

#endif
