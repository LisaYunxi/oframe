#include "public.h"
#include "CompressProtocol.h"
#include "CompressUtil.h"

CCompressProtocol::CCompressProtocol(CReactor *pReactor)
	:CProtocol(pReactor, CRPHLEN, new CCompressPackage(), new CCompressPackage())
{
	m_pkgCommpress.ConstructAllocate(2 * CRP_PACKAGE_MAX_SIZE, 256);
}

CCompressProtocol::~CCompressProtocol()
{
}

int CCompressProtocol::Pop(CPackage *pPackage)
{
	CCompressPackage *pCRPPackage = (CCompressPackage *)pPackage;

	switch (pCRPPackage->GetCompressMethod())
	{
	case CRPCM_ZERO:
	{
		m_pkgCommpress.AllocateToAdd();
		unsigned long nOutputNumBytes = m_pkgCommpress.Length();
		CompressUtil::Zerodecompress((unsigned char *)pCRPPackage->Address(),
			pCRPPackage->Length(), (unsigned char *)m_pkgCommpress.Address(),
			nOutputNumBytes);
		m_pkgCommpress.Truncate(nOutputNumBytes);

//		NET_COMPRESS_LOG2("Decompress, compress len=[%d], orig len=[%d]\n",
//			pCRPPackage->Length(), nOutputNumBytes);
		m_pkgCommpress.GetCRPHeader()->Type = pCRPPackage->GetCRPHeader()->Type;
	}
	break;
	default:
//		NET_COMPRESS_LOG0("no compress\n");
		return CProtocol::Pop(pPackage);
	}

	return CProtocol::Pop(&m_pkgCommpress);
}

int CCompressProtocol::Push(CPackage *pPackage, CProtocol *pUpper)
{
	CCompressPackage *pCRPSendPackage = (CCompressPackage *)m_pSendPackage;

	unsigned char chActiveID = (unsigned char)pUpper->GetActiveID();

	pCRPSendPackage->GetCRPHeader()->Type = chActiveID;

	unsigned char chCompressMethod = GetCompressMethod(chActiveID);
	pCRPSendPackage->GetCRPHeader()->Method = chCompressMethod;

	if (chCompressMethod == CRPCM_NONE)
	{
		return CProtocol::Push(pPackage, pUpper);
	}

	m_pkgCommpress.AllocateToAdd();
	unsigned long nOutputNumBytes = m_pkgCommpress.Length();
	switch (chCompressMethod)
	{
	case CRPCM_ZERO:
	{
		CompressUtil::Zerocompress((unsigned char *)pPackage->Address(), pPackage->Length(), (unsigned char *)m_pkgCommpress.Address(), nOutputNumBytes);
		//		NET_COMPRESS_LOG2("Compress, compress len=[%d], orig len=[%d]\n",nOutputNumBytes, pPackage->Length());
	}
	break;
	default:
		break;
	}

	if (nOutputNumBytes >= pPackage->Length())
	{
		pCRPSendPackage->GetCRPHeader()->Method = CRPCM_NONE;
		return CProtocol::Push(pPackage, pUpper);
	}

	m_pkgCommpress.Truncate(nOutputNumBytes);
	return CProtocol::Push(&m_pkgCommpress, pUpper);
}

void CCompressProtocol::SetCompressMethod(unsigned char chActiveID, unsigned char chCompressMethod)
{
	TCRPCompressMethod cm;
	cm.Method = chCompressMethod;
	cm.ActiveID = chActiveID;
	m_CompressMethods.push_back(cm);
}

unsigned char CCompressProtocol::GetCompressMethod(unsigned char chActiveID)
{
	CCompressMethodArray::iterator i = m_CompressMethods.begin();
	for (; i != m_CompressMethods.end(); i++)
	{
		if ((*i).ActiveID == chActiveID)
		{
			return (*i).Method;
		}
	}
	return CRPCM_NONE;
}


