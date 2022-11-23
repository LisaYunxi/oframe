#ifndef AFX_CHANNELPACKAGE_H__
#define AFX_CHANNELPACKAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Package.h"
#include "Channel.h"

/**�����Channel��ȡ��ԭʼ��Package
*/
class  CChannelPackage : public CPackage
{
public:
	/**���캯��
	* @param nPackageMaxSize���������ݻ���������, Ӧ����Э��ջ�ܴ��������ĳ���
	*/
	CChannelPackage(int nPackageMaxSize);

	/**��������
	*/
	virtual ~CChannelPackage();

	/**��Channel�ж�ȡ����
	* @param pChannel ָ��Channelָ��
	* @return ��Channel�ж��������ݳ���
	*/
	int ReadFromChannel(CChannel *pChannel);

	/**ͨѶ���ϴ�ʱ���ϲ�Э��ı�ţ�������з��ص���XMPProtocolЭ����
	* @return �ϲ�Э��ı�� ˵��(����������0����Ϊ�ϲ�Э��ֻ��XMPProtocol��
	* �ұ��Ϊ0)
	*/
	virtual unsigned int GetActiveID();

	/** �����������Ƿ񹹳�һ�������İ���˵��(���ڶ�ӦЭ��δ�����������
	* �������Ƿ������������ݰ�)
	* @return 1 ��һ�������İ�
	* @return 0 ���ݰ�������
	* @return -1 ���ݰ��д���
	*/
	virtual int ValidPackage();
};

#endif
