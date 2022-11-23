#ifndef AFX_SELECTREACTOR_H__
#define AFX_SELECTREACTOR_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "Reactor.h"
#include "SocketInit.h"

class CSelectReactor : public CReactor  
{
public:
	CSelectReactor(bool bBindThreadsToCPU=false);
	virtual ~CSelectReactor();

	void SetNoShmChannelFlag(bool bFlag);

protected:
	bool  m_bWait;
	bool  m_bBindThreadsToCPU ;
private:
	virtual void DispatchIOs();
	void PrepareIds(fd_set &readfds,fd_set  &writefds, int &MaxID);
	void DispatchIO(fd_set &readfds,fd_set  &writefds, int &MaxID);
	bool m_bNoShmChannelFlag;
};

#endif