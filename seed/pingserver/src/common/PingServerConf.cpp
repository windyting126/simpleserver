/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingServerConf.cpp
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
#include "PingServerConf.h"


const int HEX_PEERID_LEN = 32;

void TPingServerConf::Print() const
{
	WriteRunInfo::WriteLog("pingserver configure:");
	WriteRunInfo::WriteLog("SERVERINFO/TIMEOUT [%u]", nTimeout);
	WriteRunInfo::WriteLog("SERVERINFO/SYNWAIT [%u]", nSynWait);
	WriteRunInfo::WriteLog("SERVERINFO/SYNBUFLEN [%u]", nSynBufLen);
	WriteRunInfo::WriteLog("SERVERINFO/CHECKINTERVAL [%u]", nCheckTimeInterval);
	WriteRunInfo::WriteLog("SERVERINFO/SYNINTERVAL [%u]", nOntimeSynInterval);
	WriteRunInfo::WriteLog("SERVERINFO/MAXCLIENTCONNS [%u]", nClientMaxUploadConns);
	WriteRunInfo::WriteLog("SERVERINFO/UPLOADNORMALPERCENT [%f]", fUploadConnsNormalPercent);
	WriteRunInfo::WriteLog("SERVERINFO/UPLOADBUSYPERCENT [%f]", fUploadConnsBusyPercent);
	WriteRunInfo::WriteLog("SERVERINFO/ENCRYPTTYPE [%u]", nEncryptType);
	WriteRunInfo::WriteLog("SERVERINFO/HASHBUCKETLEN [%u]", nHashBucketLen);
	WriteRunInfo::WriteLog("SERVERINFO/STATINTERVAL [%u]", nStatInterval);	
	WriteRunInfo::WriteLog("MULTICAST/IFUSE [%u]", bUseMulticast);
	WriteRunInfo::WriteLog("MULTICAST/MADDR [%s]", strMAddr.c_str());
	WriteRunInfo::WriteLog("MULTICAST/OUTIFADDR [%s]", strMOutIFAddr.c_str());
	WriteRunInfo::WriteLog("MULTICAST/SENDPORT [%u]", nMSendPort);
	WriteRunInfo::WriteLog("MULTICAST/RECVPORT [%u]", nMRecvPort);
	WriteRunInfo::WriteLog("MULTICAST/LOOP [%u]", nMLoop);
	WriteRunInfo::WriteLog("MULTICAST/TTL [%u]", nMTtl);
	WriteRunInfo::WriteLog("PMGRDB/IFUSEDOMAIN [%u]", bUseMgrDbDomain);
	WriteRunInfo::WriteLog("PMGRDB/DBDOMAIN [%s]", strMgrDbDomain.c_str());
	WriteRunInfo::WriteLog("PMGRDB/DBIP [%s]", strMgrDbIp.c_str());
	WriteRunInfo::WriteLog("PMGRDB/DBUSER [%s]", strMgrDbUser.c_str());
	WriteRunInfo::WriteLog("PMGRDB/DBPASSWD [%s]", strMgrDbPasswd.c_str());
	WriteRunInfo::WriteLog("PMGRDB/DBNAME [%s]", strMgrDbName.c_str());
	WriteRunInfo::WriteLog("PMGRDB/UPDATEINTERVAL [%u]", nUpdateMgrDbConfigInterval);
	WriteRunInfo::WriteLog("PMGRDB/PQSRVTYPE [%u]", nPQSrvType);
	WriteRunInfo::WriteLog("PMGRDB/PRSRVTYPE [%u]", nPRSrvType);	
	for (int i = 0; i < (int)vecQuervSrv.size(); ++i)
	{
		WriteRunInfo::WriteLog("QSERVER%d/IP [%s]", i, vecQuervSrv[i].strIp.c_str());
		WriteRunInfo::WriteLog("QSERVER%d/PORT [%u]", i, vecQuervSrv[i].nPort);
	}
	for (int i = 0; i < (int)vecResourceSrv.size(); ++i)
	{
		WriteRunInfo::WriteLog("RSERVER%d/IP [%s]", i, vecResourceSrv[i].strIp.c_str());
		WriteRunInfo::WriteLog("RSERVER%d/PORT [%u]", i, vecResourceSrv[i].nPort);
	}
}

CPingServerConf::CPingServerConf()
{

}
CPingServerConf::~CPingServerConf()
{

}
int CPingServerConf::Init()
{
	std::string tmpStr;
	Magic_Config MagicConfig(std::string(PINGSERVERCONF));
	//Init Magic Config for NatServer
	int iRet = MagicConfig.Init();
	if (iRet != 0)
	{
		WriteRunInfo::WriteLog("Read config file error.");
		return -1;
	}
	//Read Timeout
	MagicConfig.ReadItem("SERVERINFO", "TIMEOUT", "0", tmpStr);
	m_Conf.nTimeout = atoi(tmpStr.c_str());
	if (m_Conf.nTimeout <= 0)
	{
		WriteRunInfo::WriteLog("Read config file error : TIMEOUT!");
		return -1;
	}
	//Read SynWait
	MagicConfig.ReadItem("SERVERINFO", "SYNWAIT", "5", tmpStr);
	m_Conf.nSynWait = atoi(tmpStr.c_str());
	if (m_Conf.nSynWait <= 0)
	{
		WriteRunInfo::WriteLog("Read config file error : SYNWAIT!");
		return -1;
	}
	//Read SynBufLen
	MagicConfig.ReadItem("SERVERINFO", "SYNBUFLEN", "20971520", tmpStr);
	m_Conf.nSynBufLen = atoi(tmpStr.c_str());
	if (m_Conf.nSynBufLen <= 0)
	{
		WriteRunInfo::WriteLog("Read config file error : SYNBUFLEN!");
		return -1;
	}
	//Read Check Timeout interval
	MagicConfig.ReadItem("SERVERINFO", "CHECKINTERVAL", "0", tmpStr);
	m_Conf.nCheckTimeInterval = atoi(tmpStr.c_str());
	if (m_Conf.nCheckTimeInterval <= 0)
	{
		WriteRunInfo::WriteLog("Read config file error : CHECKINTERVAL!");
		return -1;
	}
	//Read Ontime Syn interval
	MagicConfig.ReadItem("SERVERINFO", "SYNINTERVAL", "0", tmpStr);
	m_Conf.nOntimeSynInterval = atoi(tmpStr.c_str());
	if (m_Conf.nOntimeSynInterval <= 0)
	{
		WriteRunInfo::WriteLog("Read config file error : SYNINTERVAL!");
		return -1;
	}
	//Read Max Client Upload Connection Nums
	MagicConfig.ReadItem("SERVERINFO", "MAXCLIENTCONNS", "0", tmpStr);
	m_Conf.nClientMaxUploadConns = atoi(tmpStr.c_str());
	if (m_Conf.nClientMaxUploadConns <= 0 || m_Conf.nClientMaxUploadConns > 255)
	{
		WriteRunInfo::WriteLog("Read config file error : MAXCLIENTCONNS!");
		return -1;
	}
	//Read Client Upload Normal Percent
	MagicConfig.ReadItem("SERVERINFO", "UPLOADNORMALPERCENT", "0", tmpStr);
	m_Conf.fUploadConnsNormalPercent = atoi(tmpStr.c_str()) / 100.0;
	if (m_Conf.fUploadConnsNormalPercent <= 0 || m_Conf.fUploadConnsNormalPercent > 1)
	{
		WriteRunInfo::WriteLog("Read config file error : UPLOADBUSYPERCENT!");
		return -1;
	}
	//Read Client Upload Busy Percent
	MagicConfig.ReadItem("SERVERINFO", "UPLOADBUSYPERCENT", "0", tmpStr);
	m_Conf.fUploadConnsBusyPercent = atoi(tmpStr.c_str()) / 100.0;
	if (m_Conf.fUploadConnsBusyPercent <= 0 || m_Conf.fUploadConnsBusyPercent > 1 || m_Conf.fUploadConnsBusyPercent <= m_Conf.fUploadConnsNormalPercent)
	{
		WriteRunInfo::WriteLog("Read config file error : UPLOADBUSYPERCENT!");
		return -1;
	}
	//Read Encrypt Type
	MagicConfig.ReadItem("SERVERINFO", "ENCRYPTTYPE", "-1", tmpStr);
	m_Conf.nEncryptType = atoi(tmpStr.c_str());
	if (m_Conf.nEncryptType < 0 || m_Conf.nEncryptType > 3)
	{
		WriteRunInfo::WriteLog("Read config file error : ENCRYPTTYPE!");
		return -1;
	}
	//Read Cid Store Server NO.
	MagicConfig.ReadItem("SERVERINFO", "HASHBUCKETLEN", "0", tmpStr);
	m_Conf.nHashBucketLen = atoi(tmpStr.c_str());
	if(m_Conf.nHashBucketLen <= 0)
	{
		WriteRunInfo::WriteLog("Read config file error : HASHBUCKETLEN");
		return -1;
	}
	//Read statistics interval
	tmpStr.clear();
	MagicConfig.ReadItem("SERVERINFO", "STATINTERVAL", "300", tmpStr);
	m_Conf.nStatInterval = atoi(tmpStr.c_str());
	if(m_Conf.nStatInterval <= 0)
	{
		WriteRunInfo::WriteLog("Read config file error : STATINTERVAL");
		return -1;
	}
	//Read multicast configure
	do {
		//Read whether use multicast to sync
		tmpStr.clear();
		MagicConfig.ReadItem("MULTICAST", "IFUSE", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : IFUSE");
			return -1;
		}
		m_Conf.bUseMulticast = (atoi(tmpStr.c_str()) == 0 ? false : true);
		if (!m_Conf.bUseMulticast)
		{
			break;
		}
		//Read multicast address.
		tmpStr.clear();		
		MagicConfig.ReadItem("MULTICAST", "MADDR", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : MADDR");
			return -1;
		}
		m_Conf.strMAddr = tmpStr;		
		//Read multicast outbound interface address.
		tmpStr.clear();		
		MagicConfig.ReadItem("MULTICAST", "OUTIFADDR", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : OUTIFADDR");
			return -1;
		}
		m_Conf.strMOutIFAddr = tmpStr;
		//Read multicast send port.
		tmpStr.clear();		
		MagicConfig.ReadItem("MULTICAST", "SENDPORT", "0", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : SENDPORT");
			return -1;
		}
		m_Conf.nMSendPort = atoi(tmpStr.c_str());
		//Read multicast recv port.
		tmpStr.clear();		
		MagicConfig.ReadItem("MULTICAST", "RECVPORT", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : RECVPORT");
			return -1;
		}
		m_Conf.nMRecvPort = atoi(tmpStr.c_str());
		//Read multicast loop socket option.
		tmpStr.clear();		
		MagicConfig.ReadItem("MULTICAST", "LOOP", "0", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : LOOP");
			return -1;
		}
		int nTmp = atoi(tmpStr.c_str());
		if (nTmp != 0 && nTmp != 1)
		{
			WriteRunInfo::WriteLog("invalid MULTICAST/LOOP");
			return -1;
		}
		m_Conf.nMLoop = nTmp;
		//Read multicast ttl socket option.
		tmpStr.clear();		
		MagicConfig.ReadItem("MULTICAST", "TTL", "1", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : TTL");
			return -1;
		}
		nTmp = atoi(tmpStr.c_str());
		if (nTmp <= 0 || nTmp >= 256)
		{
			WriteRunInfo::WriteLog("invalid MULTICAST/TTL");
            return -1;			
		}
		m_Conf.nMTtl = nTmp;
	} while (0);
	//Read pmgrsrv DB configure
	{
		//Read whether use domain to access pmgrsrv DB
		tmpStr.clear();		
		MagicConfig.ReadItem("PMGRDB", "IFUSEDOMAIN", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : IFUSEDOMAIN");
			return -1;
		}
		m_Conf.bUseMgrDbDomain = ((atoi(tmpStr.c_str()) == 0) ? false : true);
		//Read pmgrsrv DB domain
		if (m_Conf.bUseMgrDbDomain)
		{
			tmpStr.clear();		
			MagicConfig.ReadItem("PMGRDB", "DBDOMAIN", "", tmpStr);
			if (tmpStr.empty())
			{
				WriteRunInfo::WriteLog("Read config file error : DBDOMAIN");
				return -1;
			}
			m_Conf.strMgrDbDomain = tmpStr;
		}
		//Read pmgrsrv DB IP
		tmpStr.clear();		
		MagicConfig.ReadItem("PMGRDB", "DBIP", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : DBIP");
			return -1;
		}
		m_Conf.strMgrDbIp = tmpStr;
		//Read pmgrsrv DB user
		tmpStr.clear();		
		MagicConfig.ReadItem("PMGRDB", "DBUSER", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : DBUSER");
			return -1;
		}
		m_Conf.strMgrDbUser = tmpStr;
		//Read pmgrsrv DB password
		tmpStr.clear();		
		MagicConfig.ReadItem("PMGRDB", "DBPASSWD", "", tmpStr);
		m_Conf.strMgrDbPasswd = tmpStr;
		//Read pmgrsrv DB name
		tmpStr.clear();		
		MagicConfig.ReadItem("PMGRDB", "DBNAME", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : DBNAME");
			return -1;
		}
		m_Conf.strMgrDbName = tmpStr;
		//Read update pmgrsrv DB config interval
		tmpStr.clear();
		MagicConfig.ReadItem("PMGRDB", "UPDATEINTERVAL", "30", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : UPDATEINTERVAL");
			return -1;
		}
		m_Conf.nUpdateMgrDbConfigInterval = atoi(tmpStr.c_str());
		//Read pqsrv type in pmgrsrv DB table
		tmpStr.clear();
		MagicConfig.ReadItem("PMGRDB", "PQSRVTYPE", "1", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : PQSRVTYPE");
			return -1;
		}
		m_Conf.nPQSrvType = atoi(tmpStr.c_str());
		//Read prsrv type in pmgrsrv DB table
		tmpStr.clear();
		MagicConfig.ReadItem("PMGRDB", "PRSRVTYPE", "0", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : PRSRVTYPE");
			return -1;
		}
		m_Conf.nPRSrvType = atoi(tmpStr.c_str());
	}

	if (InitQServer() != 0)
	{
		WriteRunInfo::WriteLog("Read config file error : QServer");
		return -1;
	}
	if (InitRServer() != 0)
	{
		WriteRunInfo::WriteLog("Read config file error : RServer");
		return -1;
	}	

	//Read UdpProcAndSendThread No.
	MagicConfig.ReadItem("SERVERINFO", "UDPPROCANDSENDTHREAD", "1", tmpStr);
	m_Conf.nUdpProcAndSendThreadNum = atoi(tmpStr.c_str());

	if(m_Conf.nUdpProcAndSendThreadNum <= 0)
	{
		WriteRunInfo::WriteLog("Read config file error : UDPPROCANDSENDTHREAD");
		return -1;
	}

	m_Conf.Print();
	return 0;
}

int CPingServerConf::InitQServer()
{
	std::string tmpStr;
	Magic_Config MagicConfig(std::string(QUERYSERVERCONF));
	//Init Magic Config for NatServer
	int iRet = MagicConfig.Init();
	if (iRet != 0)
	{
		WriteRunInfo::WriteLog("Read config file error.");
		return -1;
	}
	//Read SERVERNO
	MagicConfig.ReadItem("SERVERINFO", "SERVERNO", "0", tmpStr);
	int nServerNumber = atoi(tmpStr.c_str());
	if (nServerNumber <= 0)
	{
		WriteRunInfo::WriteLog("Read config file error : NUMBER!");
		return -1;
	}
	//Read port
	tmpStr.clear();
	MagicConfig.ReadItem("SERVERINFO", "PORT", "", tmpStr);
	if (tmpStr.empty())
	{
		WriteRunInfo::WriteLog("Read config file error : PORT!");
		return -1;
	}
	int nPort = atoi(tmpStr.c_str());
	TSeedServer SeedServer;
	m_Conf.vecQuervSrv.clear();
	for (int iA = 0; iA < nServerNumber; iA ++)
	{
		std::stringstream flagStr;
		flagStr << "QSERVER";
		flagStr << iA;

		SeedServer.strIp.clear();		
		MagicConfig.ReadItem(flagStr.str().c_str(), "IP", "", SeedServer.strIp);
		if(SeedServer.strIp == "")
		{
			WriteRunInfo::WriteLog("Read config file error : supernode server info error!");
			return -1;
		}
		if(inet_pton(AF_INET, SeedServer.strIp.c_str(), &SeedServer.nIp) > 0)
		{
			SeedServer.nIp = ntohl(SeedServer.nIp);
		}
		else 
		{
			SeedServer.nIp = 0;
		}
		SeedServer.nPort = nPort;
		
		m_Conf.vecQuervSrv.push_back(SeedServer);
	}

	UpdateQuerySrv(m_Conf.vecQuervSrv);
	return 0;
}

int CPingServerConf::InitRServer()
{
	std::string tmpStr;
	Magic_Config MagicConfig(std::string(RESOURCESERVERCONF));
	//Init Magic Config for NatServer
	int iRet = MagicConfig.Init();
	if (iRet != 0)
	{
		WriteRunInfo::WriteLog("Read config file error.");
		return -1;
	}
	//Read SERVERNO
	MagicConfig.ReadItem("SERVERINFO", "SERVERNO", "0", tmpStr);
	int nServerNumber = atoi(tmpStr.c_str());
	if (nServerNumber <= 0)
	{
		WriteRunInfo::WriteLog("Read config file error : NUMBER!");
		return -1;
	}
	//Read port
	tmpStr.clear();
	MagicConfig.ReadItem("SERVERINFO", "PORT", "", tmpStr);
	if (tmpStr.empty())
	{
		WriteRunInfo::WriteLog("Read config file error : PORT!");
		return -1;
	}
	int nPort = atoi(tmpStr.c_str());
	TSeedServer SeedServer;
	m_Conf.vecResourceSrv.clear();
	for (int iA = 0; iA < nServerNumber; iA ++)
	{
		std::stringstream flagStr;
		flagStr << "RSERVER";
		flagStr << iA;

		SeedServer.strIp.clear();		
		MagicConfig.ReadItem(flagStr.str().c_str(), "IP", "", SeedServer.strIp);
		if(SeedServer.strIp == "")
		{
			WriteRunInfo::WriteLog("Read config file error : supernode server info error!");
			return -1;
		}
		if(inet_pton(AF_INET, SeedServer.strIp.c_str(), &SeedServer.nIp) > 0)
		{
			SeedServer.nIp = ntohl(SeedServer.nIp);
		}
		else 
		{
			SeedServer.nIp = 0;
		}
		SeedServer.nPort = nPort;
		
		m_Conf.vecResourceSrv.push_back(SeedServer);
	}

	UpdateResourceSrv(m_Conf.vecResourceSrv);
	return 0;
}

TPingServerConf & CPingServerConf::GetConf()
{
	return m_Conf;
}

void CPingServerConf::GetQuerySrv(vector<TSeedServer> &vecQuerySrv)
{
	m_QuerySrvLock.GetReadLock();
	vecQuerySrv = m_vecQuerySrv;
	m_QuerySrvLock.ReleaseLock();
}

void CPingServerConf::UpdateQuerySrv(const vector<TSeedServer> &vecQuerySrv)
{
	m_QuerySrvLock.GetWriteLock();
	m_vecQuerySrv = vecQuerySrv;
	m_QuerySrvLock.ReleaseLock();
}

void CPingServerConf::GetResourceSrv(vector<TSeedServer> &vecResourceSrv)
{
	m_ResourceSrvLock.GetReadLock();
	vecResourceSrv = m_vecResourceSrv;
	m_ResourceSrvLock.ReleaseLock();
}

void CPingServerConf::UpdateResourceSrv(const vector<TSeedServer> &vecResourceSrv)
{
	m_ResourceSrvLock.GetWriteLock();
	m_vecResourceSrv = vecResourceSrv;
	m_ResourceSrvLock.ReleaseLock();
}

