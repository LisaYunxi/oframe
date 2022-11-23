// FIBSession.h: interface for the CFIBSession class .
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIBSESSION_H__88022E6A_66E9_46E0_B66F_8F73B23CDEFA__INCLUDED_)
#define AFX_FIBSESSION_H__88022E6A_66E9_46E0_B66F_8F73B23CDEFA__INCLUDED_

#include "EventHandler.h"
#include "XMPSession.h"
#include "XMPProtocol.h"
#include "CompressProtocol.h"
#include "FIBProtocol.h"
#include "FlowReader.h"

class CFIBSession;

class CFIBSessionCallback
{
public:
	virtual int HandlePackage(CXTPPackage *pFIBPackage, CFIBSession *pSession) = 0;
};

/**��һ�����Э��ջ���������ǻ���IO�����EventHandler�࣬һ������ListenCtrl
*������������ע�ᵽһ��Reactor�������ĵײ�������һ��TcpChannel�ϣ������ڵ�
*Э��ֻ��ͨ�������Ĳ��ܵõ���д���� */
class CFIBSession : public CProtocolCallback, public CXMPSession
{
public:
	/**���캯�������Э��ջ�Ĺ���
	* @param pReactor   ��������
	* @param pChannel   һ��TcpChannel
	*/ 	
	CFIBSession(CReactor *pReactor,CChannel *pChannel);
	
	/**�����������ͷſɿ���Դ
	*/
	virtual ~CFIBSession();
	
	/**����һ����Ϣ
	* @param nEventID ��ϢID
	* @param dwParam ��һ������
	* @param pParam �ڶ�������
	* @return ������
	*/
	virtual int HandleEvent(int nEventID, DWORD dwParam, void *pParam);

	virtual int HandlePackage(CPackage *pPackage,CProtocol *pProtocol);

	/**��ȡ����дsocket����selectʹ��
	* @param pReadId   ������������ض���ʹ�õ�socket��Ϊ0��ʾ����Ҫ��
	* @param pWriteId  �������������д��ʹ�õ�socket��Ϊ0��ʾ����Ҫд
	* @remark �����ｫ��������������ݰ����ݵ����²�Э��
	*/
	virtual void GetIds(int *pReadId, int *pWriteId);
	
	void RegisterPackageHandler(CFIBSessionCallback *pPackageHandler);

	void Publish(CReadOnlyFlow *pFlow, WORD wSequenceSeries, int nStartId);
	void UnPublish(WORD wSequenceSeries);
	void RegisterSubscriber(CFIBSubscriber *pSubscriber);
	void UnRegisterSubscriber(CFIBSubscriber *pSubscriber);
	
	CXTPPackage *CreatePackage();
	
	void SendRequestPackage(CXTPPackage *pPackage);
	
	void SetCommPhaseNo(DWORD wCommPhaseNo);	

	virtual void Disconnect(int nErrorCode);

	/**�������FIB��ѹ���㷨
	*@param chCompressMethod ѹ���㷨���룬0��ʾ��ѹ��
	*/
	void SetCompressMethod(BYTE chCompressMethod);
	
	/**���ñ��Ự�İ汾
	*@param	version	�汾��
	*/
	void SetVersion(BYTE version);
	
	/**��ȡ���Ự�İ汾
	*@return	���Ự�İ汾
	*/
	BYTE GetVersion(void);
	
protected:
	CFIBProtocol *m_pFIBProtocol;		/**< FIBЭ�����*/
	CCompressProtocol *m_pCRPProtocol;	/**< CRPЭ�����*/
	CFIBSessionCallback *m_pPackageHandler;
	///-----
	///	xuzh��20060223�������˶�FTDЭ��İ汾����
	///-----
	BYTE m_version;
};

#endif // !defined(AFX_FIBSESSION_H__88022E6A_66E9_46E0_B66F_8F73B23CDEFA__INCLUDED_)
