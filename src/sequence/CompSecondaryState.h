// CompSecondaryState.h: interface for the CCompSecondaryState class.
//////////////////////////////////////////////////////////////////////

#ifndef _COMPSECONDARYSTATE_H__
#define _COMPSECONDARYSTATE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CompState.h"

/**�Ŷӷ���״̬���Ĵӻ�״̬��Secondary��
*/
class CCompSecondaryState : public CCompState  
{
public:
	/**���캯��
	* @param pCommServer �Ŷӷ���
	*/
	CCompSecondaryState(CCompServer *pCompServer);
	
	/**��������
	*/
	virtual ~CCompSecondaryState();

	/**��ȡ��״̬��״̬����
	* @return ����״̬���룬���ӻ�״̬(Secondary)
	*/
	virtual TCompStateID GetStateID();
	
	virtual void OnEntry(TCompStateID nLastStateID);

	/**�����������������İ�
	*/
	virtual void OnHotlinePackageReceived(CHotlinePackage *pHotlinePackage);

	/**���������������жϵ��¼�
	*/
	virtual void OnHotlineChannelLost(CHotlineSession *pSession, int ErrorCode);

	/**�����ٲû��������İ�
	*/
	virtual void OnGuardPackageReceived(CGuardPackage *pArbPackage);

	/**�������ٲû������жϵ��¼�
	*/
	virtual void OnGuardChannelLost(CGuardSession *pSession, int ErrorCode);

};

#endif
