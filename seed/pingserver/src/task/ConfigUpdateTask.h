/**
 * @file ConfigUpdateTask.h
 * @author yanshi <yanshi@baofeng.com>
 * @date 2013-03-07
 */

#ifndef _CONFIGUPDATETASK_H_
#define _CONFIGUPDATETASK_H_

#include "PingServerConf.h"
#include <vector>
#include <string>

/// configure update task.
class CConfigUpdateTask
{
public:
	/**
	 * singleton method.
	 * @return singleton
	 */
	static CConfigUpdateTask& GetInstance();
	/**
	 * initialize.
	 * @return 0 succeed; otherwise fail
	 */
	int Init();
	/**
	 * update query server connection configure information from pmgrsrv DB.
	 * @param[in] bIfNotifyOnChanged whether notify on configure changed
	 */
	void UpdateQuerySrv(bool bIfNotifyOnChanged = true);
	/**
	 * update resource server connection configure information from pmgrsrv DB.
	 * @param[in] bIfNotifyOnChanged whether notify on configure changed
	 */
	void UpdateResourceSrv(bool bIfNotifyOnChanged = true);
private:
	/// constructor.
	CConfigUpdateTask() {}
	/**
	 * copy constructor.
	 * @attention not implement
	 */
	CConfigUpdateTask(const CConfigUpdateTask&);
	/**
	 * assign opertator.
	 * @attention not implement
	 */
	CConfigUpdateTask& operator=(const CConfigUpdateTask&);
	/**
	 * get server configure from pmgrsrv DB.
	 * @param[in] nType server type
	 * @param[out] vecServerIP vector of server IP
	 * @return 0 succeed; otherwise fail
	 */
	int GetConfig(int nType, std::vector<std::string> &vecServerIp) const;
	/**
	 * notify configure changed.
	 * @param[in] ucMachineType machine type
	 * @param[in] ucMachineIndex machine index
	 * @param[in] strIp new IP address
	 * @param[in] nPort new port
	 */
	void NotifyConfigChange(unsigned char ucMachineType, unsigned short usMachineIndex, const std::string &strIp, short nPort) const;
private:
	vector<TSeedServer> m_vecQuerySrv;			///< connection information of query server
	vector<TSeedServer> m_vecResourceSrv;		///< connection information of resource server
};

#endif // _CONFIGUPDATETASK_H_

