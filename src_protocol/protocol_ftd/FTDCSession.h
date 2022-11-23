#ifndef _FTDCSESSION_H__
#define _FTDCSESSION_H__

#include "EventHandler.h"
#include "XMPSession.h"
#include "XMPProtocol.h"
#include "CompressProtocol.h"
#include "FTDCProtocol.h"
#include "FlowReader.h"

class CFTDCSession;

class CFTDCSessionCallback
{
public:
	virtual int HandlePackage(CFTDCPackage *pFTDCPackage, CFTDCSession *pSession) = 0;
};

//��һ�����Э��ջ���������ǻ���IO�����EventHandler�࣬һ������ListenCtrl
//������������ע�ᵽһ��Reactor�������ĵײ�������һ��TcpChannel�ϣ������ڵ�
//Э��ֻ��ͨ�������Ĳ��ܵõ���д���� 
class CFTDCSession : public CProtocolCallback, public CXMPSession
{
public:
	CFTDCSession(CReactor *pReactor, CChannel *pChannel);
	virtual ~CFTDCSession();
	virtual int HandleEvent(int nEventID, unsigned int dwParam, void *pParam);
	virtual int HandlePackage(CPackage *pPackage, CProtocol *pProtocol);

	//��ȡ����дsocket����selectʹ��
	// @param pReadId   ������������ض���ʹ�õ�socket��Ϊ0��ʾ����Ҫ��
	// @param pWriteId  �������������д��ʹ�õ�socket��Ϊ0��ʾ����Ҫд
	// @remark �����ｫ��������������ݰ����ݵ����²�Э��
	virtual void GetIds(int *pReadId, int *pWriteId);

	void RegisterPackageHandler(CFTDCSessionCallback *pPackageHandler);

	void Publish(CReadFlow *pFlow, unsigned short wSequenceSeries, UF_INT8 nStartId, UF_INT8 nBusinessNo = UF_INT8_MAX, int nReaderTimes = 1);
	void UnPublish(unsigned short wSequenceSeries);
	void UnPublishAll();

	void RegisterSubscriber(CFTDCSubscriber *pSubscriber);
	void UnRegisterSubscriber(CFTDCSubscriber *pSubscriber);

	CFTDCPackage *CreatePackage();

	void SendRequestPackage(CFTDCPackage *pPackage);

	virtual void Disconnect(int nErrorCode);

	//�������FTDC��ѹ���㷨
	//@param chCompressMethod ѹ���㷨���룬0��ʾ��ѹ��
	void SetCompressMethod(unsigned char chCompressMethod);

	void SetVersion(unsigned char version);
	unsigned char GetVersion(void);

protected:
	CFTDCProtocol *m_pFTDCProtocol;
	CCompressProtocol *m_pCRPProtocol;
	CFTDCSessionCallback *m_pPackageHandler;
	unsigned char m_version;
};

#endif
