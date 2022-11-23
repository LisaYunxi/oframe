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

#define CRPCM_NONE	0x00		//不压缩
#define CRPCM_ZERO	0x03		//只压缩0x00的行程压缩

class CCompressProtocol : public CProtocol  
{
public:
	CCompressProtocol(CReactor *pReactor);
	virtual ~CCompressProtocol();

	/**加入CRP报头并传至下层协议
	* @param pPackage 上层层协议下传的包
	* @return 0   成功
	* @return <>0 失败
	*/
	virtual int Push(CPackage *pPackage, CProtocol *pUpper);
	
	/**设置针对上层协议的压缩算法
	*@param chCompressMethod 压缩算法代码，0表示不压缩
	*@param chActiveID 上层协议代码
	*/
	void SetCompressMethod(unsigned char chActiveID, unsigned char chCompressMethod);	

protected:	
	/**判断是否CRP包，并上传
	* @param pPackage 下层协议上传的包
	* @return 0   成功
	* @return <>0 失败
	*/
	virtual int Pop(CPackage *pPackage);

	/**取针对上层协议的压缩算法
	*@param chActiveID 上层协议代码
	*@return 压缩算法代码
	*/
	unsigned char GetCompressMethod(unsigned char chActiveID);

protected:
	typedef vector<TCRPCompressMethod> CCompressMethodArray;
	CCompressMethodArray m_CompressMethods;	/**< 上层协议压缩算法表 */

	CCompressPackage m_pkgCommpress;
};

#endif