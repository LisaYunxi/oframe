#include "public.h"
#include "CacheList.h"
 
#define CACHELIST_POOL_SIZE		5
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/**发送链表的结点
* 每个结点可以缓存多个数据块
*/
class  CCacheNode
{
public:
	CCacheNode(int nBufSize)
	{
		m_nBufSize = nBufSize;
		m_pBuffer = new char [m_nBufSize];
		m_nLength = 0;
		m_pData = m_pBuffer;
		m_pNext = NULL;
	}

	~CCacheNode()
	{
		delete m_pBuffer;
	}
	//由于要把不用结点放入内存节点池复用，放入池前需要恢复结点状态
	void ResetNode()
	{
		m_nLength = 0;
		m_pData = m_pBuffer;
		m_pNext = NULL;
	}
	/**取出缓冲区头上的一块数据
	* @param len 输入：希望取出的数据长度；输出：实际取到的长度，不超过输入参数
	* @return 取到的数据。若未取到数据则为空
	*/
	void *GetData(int &len)
	{
		if(m_nLength == 0){
			return NULL;
		}
		len = (m_nLength<len)?m_nLength:len;
		return m_pData;
	}

	/**将缓冲区最前面的数据移丢弃
	* @param len 丢弃数据的长度
	* @return 实际丢弃数据的长度
	*/
	int PopFront(int len)
	{
		if(len >m_nLength) 
			len = m_nLength;

		m_pData += len;
		m_nLength -= len;
		return len;
	}
	
	/**向缓冲区尾部加入一块数据
	* @param pData 加入数据的指针
	* @param nDataLen 加入数据的长度
	* @return 加入缓冲区的地址
	*/
	void *PushBack(void *pData, int nDataLen)
	{
		char *pTarget = m_pData+m_nLength;
		if(pTarget+nDataLen > m_pBuffer+m_nBufSize)
		{			
			if (m_nLength == 0) 
			{
				m_pData = m_pBuffer;
				pTarget = m_pData;
				//修改之前胡国和发现的会导致内存越界的bug
				if(nDataLen > m_nBufSize)
				{
					//缓冲区放不下数据
					return NULL;
				}
			}
			else
			{
				//缓冲区已满
				return NULL;
			}
		}
		memcpy(pTarget, pData, nDataLen);
		m_nLength += nDataLen;
		return pTarget;
	}

	void SetNext(CCacheNode *pNext)
	{
		m_pNext = pNext;
	}

	CCacheNode *GetNext()
	{
		return m_pNext;
	}

	int GetLength()
	{
		return m_nLength;
	}
private:
	int m_nBufSize;			/**< 缓冲区长度 */
	char *m_pBuffer;		/**< 缓冲区指针 */
	int m_nLength;			/**< 本结点的数据长度 */
	char *m_pData;			/**< 针向实际的数据块 */
	CCacheNode *m_pNext;	/**< 下一个结点的指针 */
};




CCacheList::CCacheList(int nDataBlockSize)
{
	m_nDataBlockSize = nDataBlockSize;
	m_pCacheHead = new CCacheNode(nDataBlockSize);
	m_pCacheTail = m_pCacheHead;
	//内存节点池头初始为空
	m_pCachePoolHead = NULL;	
	m_iCachePoolCount = 0;
//	m_pCachePoolHead = new CCacheNode(m_nDataBlockSize);
	// 默认初始化一些内存，解决频繁申请问题，提高运行中的性能
//	CCacheNode *pNode = m_pCachePoolHead;
//	int i=0;
	//加上之后延时变大 具体原因待查
//	while (i < 102400)
//	{
//		CCacheNode *pNext = new CCacheNode(m_nDataBlockSize);
//		pNode->SetNext(pNext);
//		pNode = pNext;
//		i++;
//	}
}

CCacheList::~CCacheList()
{
	CCacheNode *pNode = m_pCacheHead;
	CCacheNode *pNext = NULL;
	while (pNode != NULL)
	{
		pNext = pNode->GetNext();
		delete pNode;
		pNode = pNext;
	}
	
	//释放内存节点池
	pNode = m_pCachePoolHead;	
	while (pNode != NULL)
	{
		pNext = pNode->GetNext();
		delete pNode;
		pNode = pNext;
	}
}

void *CCacheList::PushBack(void *pData,int nDataLen)
{
	void *pTarget = m_pCacheTail->PushBack(pData, nDataLen);
	if(pTarget != NULL)
	{
		return pTarget;
	}

	//先检查内存节点池是否有空，否则分配一个新的节点
	CCacheNode *pNode = m_pCachePoolHead;
	if(NULL != pNode)
	{
		m_pCachePoolHead = pNode->GetNext();	
		pNode->ResetNode();	
		m_iCachePoolCount--;
	}
	else
	{
		//最后的结点空间已经用完，分配一个新的结点
		pNode = new CCacheNode(m_nDataBlockSize);
	}	
	m_pCacheTail->SetNext(pNode);
	m_pCacheTail = pNode;

	//直接将数据复制进最后的结点
	pTarget = m_pCacheTail->PushBack(pData, nDataLen);
	return pTarget;
}

int CCacheList::PopFront(int len)
{
	int poplen = m_pCacheHead->PopFront(len);	
	if(m_pCacheHead->GetLength() <= 0)
	{
		CCacheNode *pNext = m_pCacheHead->GetNext();
		if(pNext != NULL)
		{
			if (m_iCachePoolCount >= CACHELIST_POOL_SIZE)
			{
				m_pCacheHead->SetNext(NULL);
				delete m_pCacheHead;
				m_pCacheHead = pNext;
			}
			else
			{
				//把要删除的节点放入内存节点池头部			
				m_pCacheHead->ResetNode();//恢复要被删除的结点状态
				m_pCacheHead->SetNext(m_pCachePoolHead);
				m_pCachePoolHead = m_pCacheHead;	
				m_pCacheHead = pNext;
				m_iCachePoolCount++;
			}		
		}
		else
		{
			m_pCacheTail = m_pCacheHead;
		}
	}
	return poplen;
}

void *CCacheList::GetData(int &nLength)
{
	return (char *)m_pCacheHead->GetData(nLength);
}

bool CCacheList::IsEmpty()
{
	int len = 1;
	return (GetData(len)==NULL);
}

void CCacheList::Clear()
{
	while(m_pCacheHead->GetLength() > 0)
	{
		PopFront(m_pCacheHead->GetLength());
	}
}
