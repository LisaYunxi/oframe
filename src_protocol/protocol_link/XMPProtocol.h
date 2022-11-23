#ifndef AFX_XMPPROTOCOL_H__
#define AFX_XMPPROTOCOL_H__

#include "Protocol.h"
#include "XMPPackage.h"

#define MSG_XMPERR_RECVHEARTBEAT		0x2001
#define MSG_XMPERR_SENDHEARTBEAT		0x2002
#define MSG_XMPERR_BADPACKAGE			0x2003

#define MSG_XMPWARNING_RECVHEARTBEAT	0x2101

#define XMPTypeNone				0x00
#define XMPTypeBinary			0x01
#define XMPTypeCompressed		0x02
#define XMPTypeSCP				0x03
#define XMPTypeXTP				0x04

const unsigned int XMPHBT_SLAVE_MODE	= 0x00000001;

/**Э��ջ�����ĵڶ���Э�飬��Ҫ������·����
*/
class  CXMPProtocol : public CProtocol  
{
public:
	/**���캯��
	* @param pReactor ָ�����������ָ��
	* @param pBelow �²�Э���ָ��
	*/
	CXMPProtocol(CReactor *pReactor);
	/**��������
	*/
	virtual ~CXMPProtocol();

	/**����XMP��ͷ�������²�Э��
	* @param pPackage �ϲ��Э���´��İ�
	* @return 0   �ɹ�
	* @return <>0 ʧ��
	*/
	virtual int Push(CPackage *pPackage, CProtocol *pUpper);
	
	void EnableHeartbeat(bool bEnable);

	void SetHeartbeatTimeout(unsigned int dwReadTimeout);

	void SetHeartbeatMode(unsigned int dwHeartbeatMode);
protected:	
	/**��¼��ǰ�յ�����ʱ�䣬�ж��Ƿ�XMP�������ϴ�
	* @param pPackage �²�Э���ϴ��İ�
	* @return 0   �ɹ�
	* @return <>0 ʧ��
	*/
	virtual int Pop(CPackage *pPackage);
		
	/**����ʱ��������CReactor���á�����������ʱ����֪ͨ�ϲ�Э�顣
	* ������д��ʱ�������������ġ�
	* @param nIDEvent ������Ϊ��ʱ��ʱ���ڶ�ʱ������ʱ����ʶ   
	*/
	virtual void OnTimer(int nIDEvent);
		
	/**�����������ģ�����������
	* @return >0   �ɹ�
	* @return <=0  ʧ�� 
	*/
	int SendHeartbeat();

	/**���ɲ���������������ʱ����
	* @return >0   �ɹ�
	* @return <=0  ʧ�� 
	*/
	int SendWriteTimeout(unsigned int dwWriteTimeout);

	/**��������������ʱ���ģ����÷���������ʱʱ��
	* @param pPackage ����������ʱ����
	*/
	void SetWriteTimeout(CXMPPackage *pPackage);

	/**�յ�һ����������ݰ�ʱ����
	* @param pPackage ��������ݰ�
	* @return 0   �ɹ�
	* @return <>0 ʧ��
	*/
	virtual int OnRecvErrorPackage(CPackage *pPackage);

protected:
	unsigned int m_LastWriteTime;		/**< ��󷢰�ʱ�䡡*/
	unsigned int m_LastReadTime;		/**< ����հ�ʱ�䡡*/
	unsigned int m_LastWarnTime;		/**< ��󷢳�����ʱ�䡡*/
	bool m_bHeartEnable;		/**< �Ƿ����/�������� */
	unsigned int m_dwWriteTimeout;		/**< ����������ʱ���� */
	unsigned int m_dwReadTimeout;		/**< ���������ĳ�ʱʱ�� */
	unsigned int m_dwWarningTimeout;	/**< ���������ı���ʱ�� */
	unsigned int m_dwHeartbeatMode;	/**< ����ʱ��ģʽ*/
};

#endif