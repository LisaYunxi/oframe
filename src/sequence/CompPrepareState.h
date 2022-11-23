// CompPrepareState.h: interface for the CCompPrepareState class
//////////////////////////////////////////////////////////////////////

#ifndef _COMPPREPARESTATE_H__
#define _COMPPREPARESTATE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CompState.h"

/**�Ŷӷ���״̬���Ĵӻ�׼��״̬��Prepare��
*/
class CCompPrepareState : public CCompState  
{
public:
	/**���캯��
	* @param pCommServer �Ŷӷ���
	*/
	CCompPrepareState(CCompServer *pCommServer);

	/**��������
	*/
	virtual ~CCompPrepareState();

	/**��ȡ��״̬��״̬����
	* @return ����״̬���룬���ӻ�׼��״̬(Prepare)
	*/
	virtual TCompStateID GetStateID();
	
	/**���뱾״̬ʱ����
	*/
	virtual void OnEntry(TCompStateID nLastStateID);

	/**�����������������İ�
	* @param pPackage ͨ���������������İ�
	*/
	virtual void OnHotlinePackageReceived(CHotlinePackage *pHotlinePackage);

	/**���������������жϵ��¼�
	* @param pSession ����һ�Ŷӷ��������ĻỰ
	* @param ErrorCode �Ự�ж�ԭ��
	*/
	virtual void OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode);

	/**�����ٲû������İ�
	* @param pPackage �ٲû������İ�
	* @remark �����ٲõ�ָ�����״̬�л�
	*/
	virtual void OnGuardPackageReceived(CGuardPackage *pArbPackage);

	/**�������ٲû������жϵ��¼�
	* @param pSession ���ٲû������ĻỰ
	* @param ErrorCode �Ự�ж�ԭ��
	*/
	virtual void OnGuardChannelLost(CGuardSession *pSession, int ErrorCode);

};

#endif !defined(AFX_COMPPREPARESTATE_H__43A3B345_CBAC_4A2C_8EC4_ADC524725487__INCLUDED_)
