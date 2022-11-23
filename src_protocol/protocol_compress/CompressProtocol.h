#ifndef AFX_COMPRESSPROTOCOL_H__
#define AFX_COMPRESSPROTOCOL_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Protocol.h"
#include "CompressPackage.h"

struct TCRPCompressMethod
{
	unsigned char Method;
	unsigned char ActiveID;
};

#define CRPCM_NONE	0x00		//��ѹ��
#define CRPCM_ZERO	0x03		//ֻѹ��0x00���г�ѹ��

class CCompressProtocol : public CProtocol  
{
public:
	CCompressProtocol(CReactor *pReactor);
	virtual ~CCompressProtocol();

	/**����CRP��ͷ�������²�Э��
	* @param pPackage �ϲ��Э���´��İ�
	* @return 0   �ɹ�
	* @return <>0 ʧ��
	*/
	virtual int Push(CPackage *pPackage, CProtocol *pUpper);
	
	/**��������ϲ�Э���ѹ���㷨
	*@param chCompressMethod ѹ���㷨���룬0��ʾ��ѹ��
	*@param chActiveID �ϲ�Э�����
	*/
	void SetCompressMethod(unsigned char chActiveID, unsigned char chCompressMethod);	

protected:	
	/**�ж��Ƿ�CRP�������ϴ�
	* @param pPackage �²�Э���ϴ��İ�
	* @return 0   �ɹ�
	* @return <>0 ʧ��
	*/
	virtual int Pop(CPackage *pPackage);

	/**ȡ����ϲ�Э���ѹ���㷨
	*@param chActiveID �ϲ�Э�����
	*@return ѹ���㷨����
	*/
	unsigned char GetCompressMethod(unsigned char chActiveID);

protected:
	typedef vector<TCRPCompressMethod> CCompressMethodArray;
	CCompressMethodArray m_CompressMethods;	/**< �ϲ�Э��ѹ���㷨�� */

	CCompressPackage m_pkgCommpress;
};

#endif