//////////////////////////////////////////////////////////////////////
// SimpleReactor.h: interface for the CSimpleReactor class.
// 20140625 chenchao  ÆÕÍ¨Reactor  
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLEREACTOR_H__0EB9BAD9_FE38_4CE1_9923_9FB932010E5B__INCLUDED_)
#define AFX_SIMPLEREACTOR_H__0EB9BAD9_FE38_4CE1_9923_9FB932010E5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "Reactor.h"
//#include "SocketInit.h"

class CSimpleReactor : public CReactor  
{
public:
	CSimpleReactor();
	virtual ~CSimpleReactor();

private:
	virtual void DispatchIOs();
};

#endif // !defined(AFX_SIMPLEREACTOR_H__0EB9BAD9_FE38_4CE1_9923_9FB932010E5B__INCLUDED_)

