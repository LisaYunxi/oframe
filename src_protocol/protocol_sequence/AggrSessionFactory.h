#ifndef AFX_AGGRSESSIONFACTORY_H__
#define AFX_AGGRSESSIONFACTORY_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "SessionFactory.h"
#include "SessionAggregation.h"

const int AGGR_ID_UNKNOWN = 0;

class CAggrSessionFactory : public CSessionFactory, public CAggrPackageHandler, public CSessionAggrCallback
{
public:
	CAggrSessionFactory(CReactor *pReactor, int nMaxSession, unsigned int dwDefaultAggrID);
	virtual ~CAggrSessionFactory();

	int HandleAggrPackage(CAggrPackage *pPackage, CAggrSession *pSession);

protected:
	virtual CSessionAggregation *GetSessionAggregation(unsigned int dwAggregationID);

	/**�����Ự����
	* @param dwAggregationID ����������ͨ�������������Ӵ�����
	*/
	virtual CSessionAggregation *CreateSessionAggregation(unsigned int dwAggregationID) = 0;

	/**�Ự���Ͻ��������
	* @param pSessionAggr �����ĻỰ����
	*/
	virtual void OnSessionAggrLogin(CSessionAggregation *pSessionAggr);

	/**�Ự���϶Ͽ������
	* @param pSessionAggr �Ͽ��ĻỰ����
	* @param nReason �Ͽ���ԭ��
	*/
	virtual void OnSessionAggrDisconnected(CSessionAggregation *pSessionAggr, int nReason);

	virtual void OnSessionAggrWarning(CSessionAggregation *pSessionAggr, int nReason, int nParam);

	virtual CSession *CreateSession(CChannel *pChannel, unsigned int dwMark);

	virtual void OnSessionDisconnected(CSession *pSession, int nReason);

private:
	unsigned int m_dwDefaultAggrID;
	typedef map<unsigned int, CSessionAggregation *> CSessionAggrMap;
	CSessionAggrMap m_mapSessionAggr;
};

#endif
