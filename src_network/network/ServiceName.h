// ServiceName.h: interface for the CServiceName class .
//
///20180727 xuzh NT-0048:构造函数参数改为const, 降低对传入参数的要求。
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICENAME_H__FE5CED63_6EC3_11D7_BB64_444553540000__INCLUDED_)
#define AFX_SERVICENAME_H__FE5CED63_6EC3_11D7_BB64_444553540000__INCLUDED_

/**CServiceName是表示一个网络服务名称的对象。其意义类似于URL。我们这里的网络服务
*名称由四部分组成，channel表示网络通讯的类型，host表示主机，port表示端口号，
*name表示内部的文件或者服务名。网络通讯类型是必须有的值，其他的值可以根据网络
*通讯的要求，设置其中某些部分。例如对于TCP，host和port是有意义的，但是name
*就没有意义。
*/
class  CServiceName
{
public:
	/**构造函数，创建一个新的服务名称
	*@参数	location	服务名称，其表示方式为channel://host:port/name
	*			其简化方式包括
	*			channel://host/name
	*			channel://host:port
	*			channel://:port/name
	*			channel://host
	*			channel:///name
	*			channel://:port
	*/
	CServiceName(const char *location);
		
	/**析构函数
	*/
	virtual ~CServiceName();
	
	/**获取网络通讯类型
	* @return 	网络通讯类型
	*/
	inline char *GetChannel(void);
	
	/**获取主机名或地址
	* @return	主机名或地址
	*/
	inline char *GetHost(void);
	
	/**获取端口号
	* @return  端口号
	*/
	inline int GetPort(void);
	
	/**获取文件名或服务名
	* @return	文件名或服务名
	*/
	inline char *GetName(void);

	/**获取服务名称
	*/
	inline char *GetLocation(void);
private:
	char *m_pLocation;	/**< 存放服务名称的串 */

	char *m_pBuffer;	/**< 供分析用的服务名称*/

	char *m_pChannel;	/**< 存储网络通讯类型 */
	
	char *m_pHost;		/**< 存储主机名或地址 */
	
	int m_nPort;		/**< 获取端口号 */
	
	char *m_pServiceName;   /**< 获取文件名或服务名 */
};

inline char *CServiceName::GetChannel(void)
{
	return m_pChannel;
}
	
inline char *CServiceName::GetHost(void)
{
	return m_pHost;
}

inline int CServiceName::GetPort(void)
{
	return m_nPort;
}
	
inline char *CServiceName::GetName(void)
{
	return m_pServiceName;
}

inline char *CServiceName::GetLocation(void)
{
	return m_pLocation;
}


#endif !defined(AFX_SERVICENAME_H__FE5CED63_6EC3_11D7_BB64_444553540000__INCLUDED_)
