// PublishPort.h: interface for the CPublishPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBLISHPORT_H__88F9C025_0AE8_4FBE_AAFD_56BAE65FECFD__INCLUDED_)
#define AFX_PUBLISHPORT_H__88F9C025_0AE8_4FBE_AAFD_56BAE65FECFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "FlowReader.h"
#include "Protocol.h"
#include "XTPPackage.h"

/**��¼һ�������ߵ���Ϣ
*/
struct TSubscirberInfo
{
	unsigned int dwSubscriberID;				/**< �����ߴ��� */
	unsigned int dwLastActiveTime;				/**< �ϴη���ACKʱ�� */
};

/**CPublishCompactPackage�ǿ������ɶ�����İ������ڷ�����Ϣʱ���������
*  ��װ��һ���������������Ϸ���
*/
class CPublishCompactPackage : public CPackage
{
public:
	/**���캯��
	*@param nMaxLength ����������С�������˿������ɵİ����ۼƳ���
	*@remark ���캯���и��������������Ļ���������Ҫ�����������ҽӡ�
	*/
	CPublishCompactPackage(int nMaxLength);

	/**��������һ������������
	*@param pPackage Ҫ��ӵİ�
	*@return true ��ӳɹ���false ���ʧ�ܣ����ڵĿ��пռ䲻��
	*@remark
	*/
	bool AppendPackage(CPackage *pPackage);

	/**��հ��е�����
	*/
	void Reset();
private:
	char *m_pBufferEnd;			/**< ���л�����ָ�� */
};

/**CPublishPort����Ϣ���еķ����˿ڡ�һ��CPublishPort��������һ�鶩����
*  ������Ϣ��
*/
class CPublishPort  
{
public:
	/**���캯��
	*@param pSendProtocol ������Ϣʹ�õ�Э�����
	*@param pSendPackage ������Ϣʹ�õİ�
	*@param nSubjectNo ������Ϣ������
	*/
	CPublishPort(CProtocol *pSendProtocol, CXTPPackage *pSendPackage, unsigned int nSubjectNo);

	/**��������
	*/
	virtual ~CPublishPort();

	/**�ҽ�һ������׼�������еİ����ͳ�ȥ
	*@param pFlow Ҫ��������
	*@param nStartId ��ǰҪ���͵���Ϣ�����
	*@return 0 �ҽӳɹ��� ���� �ҽ�ʧ��
	*/
	int AttachFlow(CReadFlow *pFlow, UF_INT8 nStartId);

	/**�ı䵱ǰ������Ϣ�����
	*@param nOffset ��Ϣ��ŵ�ƫ����
	*@param nSeekFlag ��ʼλ�ã�
	*       FROM_HEAD ����ͷ�����㣻FROM_HERE �ӵ�ǰλ�ü��㣻FROM_END ��β������
	*@remark �����ŷ����˻��ˣ��������ʻ��½�
	*/
	void MoveTo(UF_INT8 nOffset, CFlowReader::SeekFlag nSeekFlag = CFlowReader::FROM_HEAD);

	/**��ȡ��һ��Ҫ���͵İ�
	*@return NULL δȡ��Ҫ���͵İ���!=NULL ȡ����Ҫ���͵İ�
	*/
	CPackage *GetNextDataPackage();
	
	/**����һ����Ϣ
	*@param nClockLapse ʱ��Ƭ����
	*@return ʵ�ʷ��͵���Ϣ�ĸ���
	*@remark ����ʱ��Ƭ�ĳ��Ⱥͷ������ʣ��ӵ�ǰ��ſ�ʼ�������е���Ϣ���ͳ�ȥ
	*/
	int Publish(unsigned int nClockLapse,int DriveFlag=0);

	/**�ӹ���һ�������˿�������Ķ�����
	*@param pPublishPort ���ӹܵķ����˿�
	*/
	void Merge(CPublishPort *pPublishPort);

	/**���һ���������Ƿ��ɱ�CPublishPort�����ṩ���ͷ���
	*@param dwSubscriberID ������ID
	*@return true �ṩ��false ���ṩ
	*/
	inline bool IsServeFor(unsigned int dwSubscriberID);

	/**��ȡ��ǰ������Ϣ�����
	*@return ��ǰ������Ϣ�����
	*/
	inline UF_INT8 GetNextId();

	/**��ȡ�ṩ����Ķ����ߵĸ���
	*@return �����ߵĸ���
	*/
	inline int GetSubscirberCount();

	/**���Ӷ�һ�������ߵķ�������
	*@param dwSubscriberID ������ID
	*@param dwLastActiveTime �ṩ�������ʼʱ��
	*/
	inline void AddSubscriber(unsigned int dwSubscriberID, unsigned int dwLastActiveTime);

	/**ȡ����һ�������ߵķ�������
	*@param dwSubscriberID ������ID
	*@param dwLastActiveTime �ṩ�������ʼʱ��
	*/
	inline void DelSubscriber(unsigned int dwSubscriberID);

	/**����һ�������ߵ�ACKʱ��
	*@param dwSubscriberID ������ID
	*@param dwLastActiveTime �յ�ACK��ʱ��
	*@return
	*@remark
	*/
	inline void UpdSubscriber(unsigned int dwSubscriberID, unsigned int dwLastActiveTime);

	/**����ʱ�Ķ����ߴӷ����б���ɾ��
	*@param dwCurrTime����ǰʱ��
	*@return true �ж����߳�ʱ��false û�ж����߳�ʱ
	*@remark ����ʱ��δ�յ�ĳ�������ߵ�ACKʱ�����ö����ߴӷ����б���ɾ��
	*/
	bool RemoveTimeoutSubscriber(unsigned int dwCurrTime);

	/**�ı䷢������
	*@param nDeltaTransRate �������ʵı仯����>0���٣�<0����
	*/
	inline void ChangeTransRate(int nDeltaTransRate);

	/**��ȡ��ǰ��������
	*@return ��ǰ��������
	*/
	inline int GetTransRate();

private:
	/**�������ж������ϲ��İ����͵�������
	*@return true ���ͳɹ���false ����ʧ��
	*/
	bool Flush();

private:
	unsigned int m_nSubjectNo;							/**< ��������ID */
	CFlowReader m_flowReader;					/**< ��ȡҪ����������Reader */
	CXTPPackage *m_pSendPackage;				/**< ��ȡҪ���������İ� */
	
	CPublishCompactPackage *m_pCompactPackage;	/**< �ϲ����İ�����int m_nRandCostBandWidth;  //����������Ѵ����ۼƷ����� */

	int m_nTransRate;							/**< ��������(byte/ms) */
	int m_nBandWidth;							/**< ���ʹ���(���ʡ�ʱ��Ƭ) */
	CProtocol *m_pSendProtocol;					/**< ������Ϣʹ�õ�Э����� */
	int m_nTransRateIncrease;					/**< ���������ٶ� */
	int m_nDesiredTransRate;					/**< ����������뷢������ */

	vector<TSubscirberInfo> m_SubscirberInfos;	/**< �ṩ����Ķ������б� */
//	unsigned int m_dwGroupID;							/**< PublishPort�ı�ʶ */
	
	int m_nRandCostBandWidth;  //����������Ѵ����ۼ�
	int m_nRandBandWidth;
	int m_nCurrTimerClockLapse; //��ǰ��ʱ������ʱ����
};

inline UF_INT8 CPublishPort::GetNextId()
{
	return m_flowReader.GetId();
}

inline bool CPublishPort::IsServeFor(unsigned int dwSubscriberID)
{
	for (int i=0; i<m_SubscirberInfos.size(); i++)
	{
		if (m_SubscirberInfos[i].dwSubscriberID == dwSubscriberID)
		{
			return true;
		}
	}
	return false;
}

inline int CPublishPort::GetSubscirberCount()
{
	return m_SubscirberInfos.size();
}

inline void CPublishPort::AddSubscriber(unsigned int dwSubscriberID, unsigned int dwLastActiveTime)
{
	TSubscirberInfo SubscirberInfo;
	SubscirberInfo.dwSubscriberID = dwSubscriberID;
	SubscirberInfo.dwLastActiveTime = dwLastActiveTime;
	m_SubscirberInfos.push_back(SubscirberInfo);
}

inline void CPublishPort::UpdSubscriber(unsigned int dwSubscriberID, unsigned int dwLastActiveTime)
{
	vector<TSubscirberInfo>::iterator i= m_SubscirberInfos.begin();
	for (; i != m_SubscirberInfos.end(); i++)
	{
		if ((*i).dwSubscriberID == dwSubscriberID)
		{
			(*i).dwLastActiveTime = dwLastActiveTime;
			break;
		}
	}
}

inline void CPublishPort::DelSubscriber(unsigned int dwSubscriberID)
{
	vector<TSubscirberInfo>::iterator i= m_SubscirberInfos.begin();
	for (; i != m_SubscirberInfos.end(); i++)
	{
		if ((*i).dwSubscriberID == dwSubscriberID)
		{
			m_SubscirberInfos.erase(i);
			break;
		}
	}
}

inline void CPublishPort::ChangeTransRate(int nDeltaTransRate)
{
	m_nTransRate += nDeltaTransRate;
}

inline int CPublishPort::GetTransRate()
{
	return m_nTransRate;
}

#endif // !defined(AFX_PUBLISHPORT_H__88F9C025_0AE8_4FBE_AAFD_56BAE65FECFD__INCLUDED_)
