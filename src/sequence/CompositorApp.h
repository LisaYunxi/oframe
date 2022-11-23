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

/**排队服务反应器，驱动通讯接口、排队服务等，并管理它们的生命周期
*/
class CCompReactor : public CSelectReactor
{
public:
	/**构造函数
	*/
	CCompReactor(int nsequenceID,CStatusCtrlInterfaces *pStatusCtrlInf,CCompFlowFactory *pFlowFactory,const char *sPreName="");
	
	/**初始化实例
	* @reutrn true 初始化成功  false 初始化失败
	* @remark 初始化通讯接口、流管理器、排队服务等
	*/
	virtual bool InitInstance();

	/**清理实例
	* @remark 清理通讯接口、流管理器、排队服务等
	*/
	virtual void ExitInstance();
	
private:
	unsigned int m_nsequenceID;
	CCompFlowFactory *m_pFlowFactory;		/**< 流管理器 */
	CCompServer *m_pCompServer;				/**< 排队服务 */
	CStatusCtrlInterfaces *m_pStatusCtrlEng;
	char m_sPreName[128];
};

