// sequenceApp.h
//////////////////////////////////////////////////////////////////////

#include "public.h"
#include "CompFlowFactory.h"
#include "SelectReactor.h"
#include "CompServer.h"
#include "CDate.h"
#include "monitorIndex.h"
#include "version.h"
#include "Config.h"
#include "CFilelogService.h"

class CStatusCtrlInterfaces;

/**�Ŷӷ���Ӧ��������ͨѶ�ӿڡ��Ŷӷ���ȣ����������ǵ���������
*/
class CCompReactor : public CSelectReactor
{
public:
	/**���캯��
	*/
	CCompReactor(int nsequenceID,CStatusCtrlInterfaces *pStatusCtrlInf,CCompFlowFactory *pFlowFactory,const char *sPreName="");
	
	/**��ʼ��ʵ��
	* @reutrn true ��ʼ���ɹ�  false ��ʼ��ʧ��
	* @remark ��ʼ��ͨѶ�ӿڡ������������Ŷӷ����
	*/
	virtual bool InitInstance();

	/**����ʵ��
	* @remark ����ͨѶ�ӿڡ������������Ŷӷ����
	*/
	virtual void ExitInstance();
	
private:
	unsigned int m_nsequenceID;
	CCompFlowFactory *m_pFlowFactory;		/**< �������� */
	CCompServer *m_pCompServer;				/**< �Ŷӷ��� */
	CStatusCtrlInterfaces *m_pStatusCtrlEng;
	char m_sPreName[128];
};

