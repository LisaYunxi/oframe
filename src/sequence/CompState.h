#ifndef AFX_COMPSTATE_H__
#define AFX_COMPSTATE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CompServer.h"

// �Ŷӷ���״̬����״̬����
class CCompState
{
public:
	CCompState(CCompServer *pCommServer);
	virtual ~CCompState();

	//��ȡ��״̬��״̬����
	// @return ����״̬����
	virtual TCompStateID GetStateID() = 0;

	//���뱾״̬ʱ����
	// @param nLastStateID ��һ״̬
	virtual void OnEntry(TCompStateID nLastStateID) = 0;

	//�����������������İ�
	// @param pPackage ͨ���������������İ�
	virtual void OnHotlinePackageReceived(CHotlinePackage *pHotlinePackage);

	//���������������жϵ��¼�
	// @param pSession ����һ�Ŷӷ��������ĻỰ
	// @param ErrorCode �Ự�ж�ԭ��
	virtual void OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode);

	//�����ٲû������İ�
	// @param pPackage �ٲû������İ�
	// @remark �����ٲõ�ָ�����״̬�л�
	virtual void OnGuardPackageReceived(CGuardPackage *pArbPackage);

	//�������ٲû������жϵ��¼�
	// @param pSession ���ٲû������ĻỰ
	// @param ErrorCode �Ự�ж�ԭ��
	virtual void OnGuardChannelLost(CGuardSession *pSession, int ErrorCode);

protected:
	CCompServer *m_pCommServer;			// �Ŷӷ���
};

#endif
