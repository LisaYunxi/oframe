// FIBProtocl.h: interface for the CFIBProtocl class .
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBPROTOCOL_H__C5C04921_6D0B_11D7_BB64_444553540000__INCLUDED_)
#define AFX_FIBPROTOCOL_H__C5C04921_6D0B_11D7_BB64_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Protocol.h"
#include "XTPPackage.h"
#include "HashMap.h"
#include "FIBPubEndPoint.h"
#include "FIBSubEndPoint.h"

#define MSG_FIBERR_BADPACKAGE		0xA001

/**Э��ջ�����ĵڵ�����Э�飬��Ҫ��FIB���Ĵ���
*/
class  CFIBProtocol : public CProtocol  
{
public:
	/**���캯��
	* @param pReactor ָ�����������ָ��
	* @param pBelow    �²�Э���ָ��
	* @param nActiveId ���²�Э����ԣ�����Э���Э��Id
	*/
	CFIBProtocol(CReactor *pReactor);
	
	/**��������
	*/
	virtual ~CFIBProtocol();

	virtual int Pop(CPackage *pPackage);
	
	/**��������ı��ķ��ͳ�ȥ
	*/
	void PublishSend();

	/**�յ�һ����������ݰ�ʱ����
	* @param pPackage ��������ݰ�
	* @return 0   �ɹ�
	* @return <>0 ʧ��
	*/
	virtual int OnRecvErrorPackage(CPackage *pPackage);

	void Publish(CReadOnlyFlow *pFlow, WORD nSequenceSeries, int nStartId);
	void UnPublish(WORD nSequenceSeries);
	void RegisterSubscriber(CFIBSubscriber *pSubscriber);
	void UnRegisterSubscriber(CFIBSubscriber *pSubscriber);
	void SetCommPhaseNo(DWORD wCommPhaseNo);

	void Clear();
	
private:
	CFIBSubEndPoint *GetSubEndPoint(WORD nSequenceSeries);
	CFIBPubEndPoint *GetPubEndPoint(WORD nSequenceSeries);

private:
	typedef CHashMap<WORD, CFIBSubEndPoint *, HashInt> CSubEndPointMap;
	CSubEndPointMap m_mapSubEndPoint;

	typedef CHashMap<WORD, CFIBPubEndPoint *, HashInt> CPubEndPointMap;
	CPubEndPointMap m_mapPubEndPoint;
};

#endif
