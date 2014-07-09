/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingServerConf.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-03
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-03			1.0			
* =======================================================================
*/
#ifndef _PINGSERVERCONF_H_7356DCBF_F290_4717_8423_F7FD63CE6701_
#define _PINGSERVERCONF_H_7356DCBF_F290_4717_8423_F7FD63CE6701_



#include <vector>
#include <string>
#include "framecommon/framecommon.h"

using namespace MYFRAMECOMMON;
using namespace std;

#define PINGSERVERCONF "conf/pingserver.conf"
#define QUERYSERVERCONF "conf/queryserver.conf"
#define RESOURCESERVERCONF "conf/resourceserver.conf"

struct TSeedServer
{
	string strIp;
	int nIp;
	short nPort;
	TSeedServer() : nIp(0), nPort(0)
	{

	}
};

struct TPingServerConf
{
	int nEncryptType;
	int nHashBucketLen;
	int nTimeout;
	int nSynWait;
	int nSynBufLen;
	int nCheckTimeInterval;
	int nOntimeSynInterval;
	int nClientMaxUploadConns;			//客户端最大上传连接数
	float fUploadConnsBusyPercent;		//上传连接数大于nClientMaxUploadConns*nUploadConnsBusyPercent，认为上传繁忙
	float fUploadConnsNormalPercent;	//上传连接数大于nClientMaxUploadConns*nUploadConnsBusyPercent，认为上传正常，小于这个值认为上传空闲
	vector<TSeedServer> vecQuervSrv;	///< pqsrv connection information which is read from configure file
	vector<TSeedServer> vecResourceSrv;	
	int nUdpProcAndSendThreadNum;
	int nStatInterval;					///< statistics output interval
	
	bool bUseMulticast; 				///< whether use UDP multicast sync
	string strMAddr; 					///< multicast address
	string strMOutIFAddr; 				///< multicast outbound interface address
	unsigned short nMSendPort; 			///< multicast send port, i.e. my binding port
	unsigned short nMRecvPort; 			///< multicast receive port, i.e. multicast receive port
	unsigned char nMLoop; 				///< multicast loop socket option
	unsigned char nMTtl;				///< multicast ttl socket option
	
	bool bUseMgrDbDomain;				///< whether use pmgrsrv DB domain to access DB
	string strMgrDbDomain;				///< pmgrsrv DB domain
	string strMgrDbIp;					///< pmgrsrv DB IP
	string strMgrDbUser;				///< pmgrsrv DB user
	string strMgrDbPasswd;				///< pmgrsrv DB password
	string strMgrDbName;				///< pmgrsrv DB name
	int nUpdateMgrDbConfigInterval;		///< update config from pmgrsrv DB interval
	int nPQSrvType;						///< pqsrv type in pmgrsrv DB table
	int nPRSrvType;						///< prsrv type in pmgrsrv DB table	
	
	TPingServerConf() : nEncryptType(0), nHashBucketLen(0), nTimeout(0), nCheckTimeInterval(0), nOntimeSynInterval(0),
						bUseMulticast(false), nMSendPort(0), nMRecvPort(0), nMLoop(0), nMTtl(0), bUseMgrDbDomain(false),
						nUpdateMgrDbConfigInterval(0), nPQSrvType(0), nPRSrvType(0)
	{

	}
	void Print() const;
};


class CPingServerConf
{
public:
	CPingServerConf();
	~CPingServerConf();
	int Init();
	TPingServerConf & GetConf();
	/**
	 * get query server connection information.
	 * @param[out] vecQuerySrv vector of query server
	 */
	void GetQuerySrv(vector<TSeedServer> &vecQuerySrv);
	/**
	 * update query server connection information.
	 * @param[in] vecQuerySrv vector of query server updated from
	 */
	void UpdateQuerySrv(const vector<TSeedServer> &vecQuerySrv);
	/**
	 * get resource server connection information.
	 * @param[out] vecResourceSrv vector of resource server
	 */
	void GetResourceSrv(vector<TSeedServer> &vecResourceSrv);
	/**
	 * update resource server connection information.
	 * @param[in] vecResourceSrv vector of resource server updated from
	 */
	void UpdateResourceSrv(const vector<TSeedServer> &vecResourceSrv);
private:
	int InitQServer();
	int InitRServer();
private:
	TPingServerConf m_Conf;
	vector<TSeedServer> m_vecQuerySrv;		///< pqsrv connection information which is from TPingServerConf and updated from pmrgsrv DB
	CThreadRwLock m_QuerySrvLock;			///< read&write lock of m_vecQuervSrv
	vector<TSeedServer> m_vecResourceSrv;	///< pqsrv connection information which is from TPingServerConf and updated from pmrgsrv DB
	CThreadRwLock m_ResourceSrvLock;		///< read&write lock of m_vecQuervSrv
};


#endif


