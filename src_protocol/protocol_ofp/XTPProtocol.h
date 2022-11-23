#ifndef _XTPPROTOCOL_H__
#define _XTPPROTOCOL_H__

#if _MSC_VER > 1000
#pragma once
#endif 


#include "Protocol.h"
#include "XTPPackage.h"
#include "HashMap.h"
#include "XTPPubEndPoint.h"
#include "XTPSubEndPoint.h"

#define MSG_XTPERR_BADPACKAGE		0x3001

/**Э��ջ�����ĵڵ�����Э�飬��Ҫ��XTP���Ĵ���
*/
class  CXTPProtocol : public CProtocol  
{
public:
	/**���캯��
	* @param pReactor ָ�����������ָ��
	* @param pPubHelper    �²�Э���ָ��
	* @param nActiveId ���²�Э����ԣ�����Э���Э��Id
	*/
	CXTPProtocol(CReactor *pReactor, CXTPPubHelper *pPubHelper, CXTPSubHelper *pSubHelper);
	
	/**��������
	*/
	virtual ~CXTPProtocol();

	/**�������͵����ʵ��ϲ�Э��
	* @param pPackage ��Ҫ�ϴ��İ���ָ��
	* @return 0   �ɹ�
	* @return <>0 ʧ��
	*/
	virtual int Pop(CPackage *pPackage);
	
	/**����ʱ��������CReactor����
	* @param nIDEvent ��ʱ���¼���ʶ   
	*/
	virtual void OnTimer(int nIDEvent);

	/**�յ�һ����������ݰ�ʱ����
	* @param pPackage ��������ݰ�
	* @return 0   �ɹ�
	* @return <>0 ʧ��
	*/
	virtual int OnRecvErrorPackage(CPackage *pPackage);

	void OnHeartBeat(unsigned int nSubjectNo, UF_INT8 nSeqNo);
	
	void Publish(CReadFlow *pFlow, unsigned int nSubjectNo, unsigned int dwSubscriberID, UF_INT8 nStartId);
	void UnPublish(unsigned int nSubjectNo);
	void RegisterSubscriber(CSubscriber *pSubscriber, unsigned int dwSubscriberID);
	void UnRegisterSubscriber(CSubscriber *pSubscriber);
	void OnAck(unsigned int dwSubscriberID, unsigned int dwSubjectNo, UF_INT8 nStartId);
	
	/**�˴���Ҫ�������ɺ����̴߳���һ�η�������
	* @param pReadId   ������������ض���ʹ�õ�socket��Ϊ0��ʾ����Ҫ��
	* @param pWriteId  �������������д��ʹ�õ�socket��Ϊ0��ʾ����Ҫд 
	*/
	virtual void GetIds(int *pReadId, int *pWriteId);
protected:
	
private:
	CXTPSubEndPoint *GetSubEndPoint(unsigned int nSubjectNo);
	CXTPPubEndPoint *GetPubEndPoint(unsigned int nSubjectNo);

private:
	typedef CHashMap<unsigned int, CXTPSubEndPoint *, HashInt> CSubEndPointMap;
	CSubEndPointMap m_mapSubEndPoint;

	typedef CHashMap<unsigned int, CXTPPubEndPoint *, HashInt> CPubEndPointMap;
	CPubEndPointMap m_mapPubEndPoint;

	CXTPPubHelper *m_pPubHelper;
	CXTPSubHelper *m_pSubHelper;
};

#endif
