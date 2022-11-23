// CompInitState.h: interface for the CCompInitState class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _COMPINITSTATE_H__
#define _COMPINITSTATE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CompState.h"

/**�Ŷӷ���״̬���ĳ�ʼ״̬��Init��
*/
class CCompInitState : public CCompState
{
public:
	/**���캯��
	* @param pCommServer �Ŷӷ���
	*/
	CCompInitState(CCompServer *pCommServer);

	/**��������
	*/
	virtual ~CCompInitState();

	/**��ȡ��״̬��״̬����
	* @return ����״̬���룬����ʼ״̬(INIT)
	*/
	virtual TCompStateID GetStateID();

	/**���뱾״̬ʱ����
	* @param nLastStateID ��һ״̬
	*/
	virtual void OnEntry(TCompStateID nLastStateID);

	/**�����ٲû������İ�
	* @param pPackage �ٲû������İ�
	* @remark �����ٲõ�ָ�����״̬�л�
	*/
	virtual void OnGuardPackageReceived(CGuardPackage *pArbPackage);
};

#endif
