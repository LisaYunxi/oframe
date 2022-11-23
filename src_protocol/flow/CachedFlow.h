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

/**�����������Ľڵ�
*/
struct TCachedFlowNode
{
	void * address;				//�����ڻ������е�ƫ����(�����ڻ����У�����������)
	int size;					//����ĳ���(�����ڻ����У�����������)
};

/**CCachedFlowNodeVector�����ڹ�����������Ķ��С�����Ϊ����������
* һ������ΪCFNV_BUCKET_SIZE�������������Ĵ�СΪCFNV_BLOCK_SIZE��
* ���ɴ����İ������ڵ�ΪCFNV_MAX_NODE��
*/
class CCachedFlowNodeVector
{
public:
	/**���캯��
	*/
	CCachedFlowNodeVector()
	{
		m_nPopCount = 0;
		m_nNodeCount = 0;
		memset(m_pBuckets, 0, CFNV_BUCKET_SIZE*sizeof(TCachedFlowNode *));
	}
	
	/**��������
	*/
	~CCachedFlowNodeVector()
	{
		clear();
	}

	/**���±�ȡ������һ���ڵ�
	*@param n �ڵ���±�
	*@return ȡ���Ľڵ������
	*/
	inline TCachedFlowNode & operator[](size_t n)
	{
		n += m_nPopCount;
		return m_pBuckets[n/CFNV_BLOCK_SIZE][n&(CFNV_BLOCK_SIZE-1)];
	}

	/**�����β�����һ�������Ľڵ�
	*@param node Ҫ��ӵĽڵ�
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

	/**ɾ�������ڵ�
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

	/**��ȡ�����нڵ�ĸ���
	*@return �����нڵ�ĸ���
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
	/*volatile*/ int m_nPopCount;							/**< ���ж���ɾ���ĸ���*/
	/*volatile*/ int m_nNodeCount;							/**< �����нڵ�ĸ���*/
	TCachedFlowNode *m_pBuckets[CFNV_BUCKET_SIZE];		/**< һ������*/
};

/**CCachedFlow�Ǵ��̶���С���������������߶�д�ٶȡ�
* ���ǲ����̰߳�ȫ�������ڻ�����δ����֮ǰ��������һ��д���ߣ�ͬʱ�ж����ȡ��
* �����Թҽ�����һ���������ڳ־û�������������޷����ɵİ���
* ���ڹ���ʱҪָ������Ĵ�С������ʹ���Ƚ��ȳ��ķ�������
* �ڶ�ȡ��ʱ�����������û�У������Ŵӹҽӵ����ж�ȡ��
*/
class CCachedFlow : public CFlow
{
public:
	/**���캯��
	* @param bSyncFlag �ڼ���һ�����ǣ��Ƿ�����ͬ�����ҽӵ�����
	*		 true����ͬ�����ҽӵ�����false��ͬ�����ҽӵ���
	* @param nMaxObjects ��������������,���Ϊ0�����ư��ĸ���
	* @param nDataBlockSize CacheListʹ�õ����������С����ҪС��һ�����ݰ��ĳ���
	*/
	CCachedFlow(bool bSyncFlag, const char* pName = "", int nMaxObjects = CFNV_MAX_OBJECTS, int nDataBlockSize = PACKAGE_MAX_SIZE * 100);
	
	/**��������
	*/
	virtual ~CCachedFlow();
	
	/**�ҽ���һ����
	* @param pFlow ��Ҫ�ҽӵ���
	* @return 
	*/
	virtual bool AttachUnderFlow(CFlow *pFlow);

	/**��ջ���
	*/
	void Clear();

	/**�������еİ�ͬ�����ҽӵ�����
	* @return 0 �ɹ� <0 ʧ��
	*/
	bool SyncUnderFlow(UF_INT8 id);

	/**����β���İ��ص�
	*@param nCount �����İ��ĸ���
	*@return true �����ɹ��� false ����ʧ��
	*/
	virtual bool Truncate(UF_INT8 nCount); 

	/**����������һ������
	*@param	pObject Ҫ���ӵĶ���
	*@param length �������ݵĳ���
	*@return ���������е���ţ�С��0��ʾ����ʧ��
	*/
	virtual UF_INT8 Append(void *pObject, int length);

	/**��ȡָ����ŵĶ���
	*@param	id	�������
	*@param pObject ��ȡ����Ļ�����
	*@param length ��������С
	*@return ȡ������ĳ���
	*/
 	virtual int Get(UF_INT8 id, void *pObject, int length);

	/**������ĵ�һ�����ӻ���������ȥ
	*@return true ��ȥ��һ�����ɹ�
	*@return false ��ȥ��һ����ʧ��
	*/
	bool PopFront();
	
	virtual int GetFlowType();
	
protected:
	int m_nMaxObjects;				/**<��������������*/
	int m_nDataBlockSize;			/**<CacheListʹ�õ����������С*/
	CCacheList m_CacheList;			/**<�������ݵĻ���*/
	
	typedef CCachedFlowNodeVector CNodeQueue;
	CNodeQueue m_NodeQueue;			/**<��������*/

	//CRITICAL_VAR m_criticalVar;		/**< �ٽ������� */
	CRWLock m_RWLock;
};

#endif
