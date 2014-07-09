#ifndef _MY_CONFIG_H_INC_
#define _MY_CONFIG_H_INC_

#include "framecommon/framecommon.h"
using namespace MYFRAMECOMMON;

#include <vector>  //added by dijie 2009-3-26
#include <string>
using namespace std;

#define BASECONFIGFILE  "conf/base.conf"
//added by dijie 2009-3-26
typedef struct tagConfigDBItem
{
	string strMySqlIp;
	string strMySqlUser;
	string strMySqlPassword;
} CONFIGDBITEM, *PCONFIGDBITEM;
/*-----------------------added end----------------*/

class CMyConfig
{
public:
	CMyConfig();
	virtual ~CMyConfig();

	int Init();
	int GetHashLeng();
	int GetOnlineServerNo();			//wangtiefeng : 09-03-17
	string GetServerType();				//wangtiefeng : 09-03-17
    int GetUploadInterval();			//dijie : 2009-3-19
	int GetDbUpdateThreadNum();	//add by yinhaichen 2012-12-04
	int GetMySqlDbInfo(string &ip, string &user, string &password);
	int GetOnlineDBInfo(string &ip, string &user, string &password, int index);
	int GetResourceDBInfo(string &ip, string &user, string &password, int index);
	int GetOnlineDBNum();				//add by dijie 2009-3-26
	int GetOnlineDBOntimeInterval();	//wangtiefeng : 09-04-15
	int GetRcTableNum();
	int GetResourceDBNum();				//add by dijie 2009-3-26
	int GetOnlineDBIndex(int index);
	unsigned char GetMinDownPercent();			//wangtiefeng : 102版本开始添加的下载进度

	/**
	 * get statistics output interval.
	 * @return statistics output interval
	 */
	int GetStatInterval();
	/**
	 * get resource DB IP.
	 * @param[out] strIp store resource DB IP
	 * @param[in] nIndex index of resource DB to update
	 * @return 0 succeed; otherwise fail
	 */
	int GetResourceDbIp(string &strIp, int nIndex);
	/**
	 * set resource DB IP.
	 * @param[in] strIp new IP of resource DB	 
	 * @param[in] nIndex index of resource DB to set
	 * @return 0 succeed; otherwise fail
	 */
	int SetResourceDbIp(const string &strIp, int nIndex);
	/**
	 * get pingserver sync process thread number.
	 * @return pingserver sync process thread number
	 */
	int GetPingSynProcessThreadNum() const;
	/**
	 * get multicast address to join in.
	 * @return multicast address to join in
	 */
	const string& GetMAddr() const;
	/**
	 * get multicast receive port.
	 * @return get multicast receive port
	 */
	unsigned short GetMRecvPort() const;
	/**
	 * get multicast inbound interface address.
	 * @return multicast inbound interface address
	 */
	const string& GetMInIFAddr() const;
	/**
	 * whether use pmgrsrv DB domain to access pmgrsrv.
	 * @return true use domain to access; false use IP directly
	 */
	bool IfUseMgrDbDomain() const;
	/**
	 * get pmgrsrv DB domain.
	 * @return pmgrsrv DB domain
	 */
	const string& GetMgrDbDomain() const;
	/**
	 * get pmgrsrv DB IP.
	 * @return pmgrsrv DB IP
	 */
	const string& GetMgrDbIp() const;
	/**
	 * get pmgrsrv DB user name.
	 * @return pmgrsrv DB user name
	 */
	const string& GetMgrDbUser() const;
	/**
	 * get pmgrsrv DB password.
	 * @return pmgrsrv DB password
	 */
	const string& GetMgrDbPasswd() const;
	/**
	 * get pmgrsrv DB name.
	 * @return pmgrsrv DB name
	 */
	const string& GetMgrDbName() const;
	/**
	 * get interval of updating config from pmgrsrv DB.
	 * @return interval of updating config from pmgrsrv DB
	 */
	int GetIntervalOfUpdateMgrConfig() const;
	/**
	 * get resource DB type in pmgrsrv DB table.
	 * return resource DB type in pmgrsrv DB table
	 */
	int GetResourceDbType() const;
	/// log configure items.
	void Print() const;
protected:

	int m_nHashLeng;
	short m_sOnlineServerNo;		//wangtiefeng : 09-03-17
	int m_nUploadInterval;			//dijie 2009-3-19
	int m_nDbUpdateThreadNum;		//add by yinhaichen 2012-12-04
	int m_nPingSynProcessThreadNum;	///< ping server syn process thread number
	
	int m_nOnlineDBNum;
	int m_nOnlineDBOntimeInterval;	//wangtiefeng : 09-04-15
	int m_nResourceDBNum;
	string m_strMySqlIp;
	string m_strMySqlUser;
	string m_strMySqlPassword;   //annotated by dijie 2009-3-26
	std::vector <CONFIGDBITEM> m_vConfigDBItem;
	CThreadRwLock m_rwlockConfigDBItem;	///< read&write lock of m_vConfigDBItem
	std::vector <int> m_vOnlineDBIndex;
	string m_strServerType;			//wangtiefeng : 09-03-19

	int m_nRcTableNum; //资源文件表的数量
	unsigned char m_nMinDownPercent;	//wangtiefeng : 102版本开始添加的下载进度

	int m_nStatInterval;				///< statistics output interval

	string m_strMAddr; 					///< multicast address to join in
	unsigned short m_nMRecvPort;		///< multicast receive port
	string m_strMInIFAddr;				///< multicast inbound interface address

	bool m_bUseMgrDbDomain;				///< whether use pmgrsrv DB domain to access DB
	string m_strMgrDbDomain;			///< pmgrsrv DB domain
	string m_strMgrDbIp;				///< pmgrsrv DB IP
	string m_strMgrDbUser;				///< pmgrsrv DB user
	string m_strMgrDbPasswd;			///< pmgrsrv DB password
	string m_strMgrDbName;				///< pmgrsrv DB name
	int m_nUpdateMgrDbConfigInterval;	///< interval of updating config from pmgrsrv DB
	int m_nResourceDbType;				///< resource DB type in pmgrsrv DB table 
};


#endif 

