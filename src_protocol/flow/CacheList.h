#ifndef AFX_CACHELIST_H__
#define AFX_CACHELIST_H__

#if _MSC_VER > 1000
#pragma once
#endif

class CCacheNode;

/**定义了一个数据块队列
*/
class  CCacheList  
{
public:
	/**构造函数
	* @param nDataBlockSize 每个结点可存贮数据的长度，缺省值为20K
	*/
	CCacheList(int nDataBlockSize = 4096*100);
	
	/**析构造函数
	*/
	virtual ~CCacheList();

	/**取出队列头上的一块数据
	* @param len 输入：希望取出的数据长度；输出：实际取到的长度，不超过输入参数
	* @return 取到的数据。若未取到数据则为空
	*/
	void *GetData(int &len);

	/**将队列最前面的数据移丢弃
	* @param len 丢弃数据的长度
	* @return 实际丢弃数据的长度
	*/
	int PopFront(int len);
	
	/**向队列尾部加入一块数据
	* @param pData 加入数据的指针
	* @param nDataLen 加入数据的长度
	* @return 加入缓冲区的地址
	*/
	void *PushBack(void *pData, int nDataLen);
	
	/**检查队列是否为空
	* @return true 队列中无数据 false 队列中有数据
	*/
	bool IsEmpty();
	
	/**清空所有的数据
	*/
	void Clear();
protected:
	CCacheNode *m_pCacheHead;	/**< 头结点 */

	CCacheNode *m_pCacheTail;	/**< 尾结点 */
	
	int m_nDataBlockSize;		/**< 每个结点可存贮数据的长度*/
	
	//增加内存节点池
	CCacheNode *m_pCachePoolHead;	/**< 内存结点池头 */
	int m_iCachePoolCount;
};

#endif
