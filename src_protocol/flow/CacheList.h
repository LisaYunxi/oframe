#ifndef AFX_CACHELIST_H__
#define AFX_CACHELIST_H__

#if _MSC_VER > 1000
#pragma once
#endif

class CCacheNode;

/**������һ�����ݿ����
*/
class  CCacheList  
{
public:
	/**���캯��
	* @param nDataBlockSize ÿ�����ɴ������ݵĳ��ȣ�ȱʡֵΪ20K
	*/
	CCacheList(int nDataBlockSize = 4096*100);
	
	/**�����캯��
	*/
	virtual ~CCacheList();

	/**ȡ������ͷ�ϵ�һ������
	* @param len ���룺ϣ��ȡ�������ݳ��ȣ������ʵ��ȡ���ĳ��ȣ��������������
	* @return ȡ�������ݡ���δȡ��������Ϊ��
	*/
	void *GetData(int &len);

	/**��������ǰ��������ƶ���
	* @param len �������ݵĳ���
	* @return ʵ�ʶ������ݵĳ���
	*/
	int PopFront(int len);
	
	/**�����β������һ������
	* @param pData �������ݵ�ָ��
	* @param nDataLen �������ݵĳ���
	* @return ���뻺�����ĵ�ַ
	*/
	void *PushBack(void *pData, int nDataLen);
	
	/**�������Ƿ�Ϊ��
	* @return true ������������ false ������������
	*/
	bool IsEmpty();
	
	/**������е�����
	*/
	void Clear();
protected:
	CCacheNode *m_pCacheHead;	/**< ͷ��� */

	CCacheNode *m_pCacheTail;	/**< β��� */
	
	int m_nDataBlockSize;		/**< ÿ�����ɴ������ݵĳ���*/
	
	//�����ڴ�ڵ��
	CCacheNode *m_pCachePoolHead;	/**< �ڴ����ͷ */
	int m_iCachePoolCount;
};

#endif
