#ifndef _COMPSINGLESTATE_H__
#define _COMPSINGLESTATE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CompState.h"

/**�Ŷӷ���״̬���ĵ���״̬��Single��
*/
class CCompSingleState : public CCompState  
{
public:
	/**���캯��
	* @param pCommServer �Ŷӷ���
	*/
	CCompSingleState(CCompServer *pCompServer);
	
	/**��������
	*/
	virtual ~CCompSingleState();
	
	/**��ȡ��״̬��״̬����
	* @return ����״̬���룬������״̬(Single)
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

private:
	bool m_bPreapreFlag;			/**< ��һ�Ŷӷ����Ƿ�������������Ŷӵİ� */
};

#endif
