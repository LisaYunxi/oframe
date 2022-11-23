#ifndef AFX_CACHEDFLOW_H__
#define AFX_CACHEDFLOW_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Flow.h"
#include "public.h"
#include "CacheList.h"
#include "Package.h"
#include "Mutex.h"

/**缓存中索引的节点
*/
struct TCachedFlowNode
{
	void * address;				//对象在缓冲区中的偏移量(当包在缓存中，此项有意义)
	int size;					//对象的长度(当包在缓存中，此项有意义)
};

/**CCachedFlowNodeVector是用于管理包的索引的队列。它分为两级存贮。
* 一级存贮为CFNV_BUCKET_SIZE个，二级存贮的大小为CFNV_BLOCK_SIZE。
* 最多可存贮的包索引节点为CFNV_MAX_NODE个
*/
class CCachedFlowNodeVector
{
public:
	/**构造函数
	*/
	CCachedFlowNodeVector()
	{
		m_nPopCount = 0;
		m_nNodeCount = 0;
		memset(m_pBuckets, 0, CFNV_BUCKET_SIZE*sizeof(TCachedFlowNode *));
	}
	
	/**析构函数
	*/
	~CCachedFlowNodeVector()
	{
		clear();
	}

	/**按下标取索引的一个节点
	*@param n 节点的下标
	*@return 取出的节点的引用
	*/
	inline TCachedFlowNode & operator[](size_t n)
	{
		n += m_nPopCount;
		return m_pBuckets[n/CFNV_BLOCK_SIZE][n&(CFNV_BLOCK_SIZE-1)];
	}

	/**向队列尾部添加一个索引的节点
	*@param node 要添加的节点
	*/
	void push_back(TCachedFlowNode &node)
	{
		int n = m_nNodeCount+m_nPopCount;
		if (n % CFNV_BLOCK_SIZE == 0)
		{
 			if(n>CFNV_MAX_NODE)
 			{
 				EMERGENCY_EXIT("push_back is larger than CFNV_MAX_NODE");
 			}
			m_pBuckets[n/CFNV_BLOCK_SIZE] = new TCachedFlowNode[CFNV_BLOCK_SIZE];
		}
		(*this)[m_nNodeCount] = node;
		//printf("push:%d,%d \n",m_nPopCount,m_nNodeCount);
		//TCachedFlowNode x = (*this)[m_nNodeCount];
		//printf("index:%d,size:%d \n",m_nNodeCount,x.size);
		m_nNodeCount++;
	}

	/**删除顶部节点
	*/
	void pop_front()
	{
		m_nPopCount++;
		if (m_nPopCount>=CFNV_BLOCK_SIZE)
		{
			delete m_pBuckets[0];
			memmove(m_pBuckets, &m_pBuckets[1], sizeof(m_pBuckets)-sizeof(TCachedFlowNode*));
			m_pBuckets[CFNV_BUCKET_SIZE-1] = NULL;
			m_nPopCount = 0;
		}
		m_nNodeCount--;
		//printf("pop:%d,%d \n",m_nPopCount,m_nNodeCount);
	}

	/**获取队列中节点的个数
	*@return 队列中节点的个数
	*/
	inline size_t size()
	{
		return m_nNodeCount;
	}

	void clear()
	{
		m_nPopCount = 0;
		m_nNodeCount = 0;
		for (int i=0; i<CFNV_BUCKET_SIZE; i++)
		{
			if (m_pBuckets[i] == NULL)
			{
				break;
			}
			delete [] m_pBuckets[i];
		}
		memset(m_pBuckets, 0, CFNV_BUCKET_SIZE*sizeof(TCachedFlowNode *));
	}
	
private:
	/*volatile*/ int m_nPopCount;							/**< 队列顶部删除的个数*/
	/*volatile*/ int m_nNodeCount;							/**< 队列中节点的个数*/
	TCachedFlowNode *m_pBuckets[CFNV_BUCKET_SIZE];		/**< 一级缓存*/
};

/**CCachedFlow是带固定大小缓存的流，用于提高读写速度。
* 它是部分线程安全的流，在缓冲区未用完之前，可以有一个写入者，同时有多个读取者
* 它可以挂接另外一个流，用于持久化或存贮缓冲区无法容纳的包。
* 它在构造时要指定缓存的大小。缓存使用先进先出的方法管理。
* 在读取包时，如果缓存中没有，则试着从挂接的流中读取。
*/
class CCachedFlow : public CFlow
{
public:
	/**构造函数
	* @param bSyncFlag 在加入一个包是，是否立刻同步到挂接的流。
	*		 true立刻同步到挂接的流，false不同步到挂接的流
	* @param nMaxObjects 缓存包的最大数量,如果为0则不限制包的个数
	* @param nDataBlockSize CacheList使用的数据区块大小，不要小于一个数据包的长度
	*/
	CCachedFlow(bool bSyncFlag, const char* pName = "", int nMaxObjects = CFNV_MAX_OBJECTS, int nDataBlockSize = PACKAGE_MAX_SIZE * 100);
	
	/**析构函数
	*/
	virtual ~CCachedFlow();
	
	/**挂接另一个流
	* @param pFlow 所要挂接的流
	* @return 
	*/
	virtual bool AttachUnderFlow(CFlow *pFlow);

	/**清空缓存
	*/
	void Clear();

	/**将缓存中的包同步到挂接的流中
	* @return 0 成功 <0 失败
	*/
	bool SyncUnderFlow(UF_INT8 id);

	/**将流尾部的包截掉
	*@param nCount 保留的包的个数
	*@return true 操作成功； false 操作失败
	*/
	virtual bool Truncate(UF_INT8 nCount); 

	/**向流中增加一个对象
	*@param	pObject 要增加的对象
	*@param length 增加数据的长度
	*@return 对象在流中的序号，小于0表示增加失败
	*/
	virtual UF_INT8 Append(void *pObject, int length);

	/**获取指定编号的对象
	*@param	id	对象序号
	*@param pObject 获取对象的缓冲区
	*@param length 缓冲区大小
	*@return 取出对象的长度
	*/
 	virtual int Get(UF_INT8 id, void *pObject, int length);

	/**将缓冲的第一个包从缓冲区中移去
	*@return true 移去第一个包成功
	*@return false 移去第一个包失败
	*/
	bool PopFront();
	
	virtual int GetFlowType();
	
protected:
	int m_nMaxObjects;				/**<缓存包的最大数量*/
	int m_nDataBlockSize;			/**<CacheList使用的数据区块大小*/
	CCacheList m_CacheList;			/**<包中数据的缓存*/
	
	typedef CCachedFlowNodeVector CNodeQueue;
	CNodeQueue m_NodeQueue;			/**<包的索引*/

	//CRITICAL_VAR m_criticalVar;		/**< 临界区变量 */
	CRWLock m_RWLock;
};

#endif
