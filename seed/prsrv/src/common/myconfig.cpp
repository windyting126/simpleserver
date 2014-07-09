#include "myconfig.h"
#include "pbasecommand.h"

CMyConfig::CMyConfig()
{
	m_nHashLeng  =10000;
	m_nUploadInterval = 0;			//dijie 2009-3-19
	m_nDbUpdateThreadNum = 1;
	m_nOnlineDBNum = 0;
	m_nResourceDBNum = 0;
	m_nRcTableNum = 0; 
	m_vConfigDBItem.resize(0);
	m_vOnlineDBIndex.resize(0);
}

CMyConfig::~CMyConfig()
{
}

int CMyConfig::Init()
{
	//∂¡»°≈‰÷√Œƒº˛
	Magic_Config config(string(BASECONFIGFILE));
	int ret = config.Init();
	if (ret != 0)
	{
		WriteRunInfo::WriteLog("Read config file error.");
		return -1;
	}
	string tmpStr;

	config.ReadItem("SERVER", "HASHLENG", "10000",  tmpStr);
	m_nHashLeng = atoi(tmpStr.c_str());
	config.ReadItem( "SERVER" , "ONLINESERVERNO" , "0" , tmpStr );	//wangtiefeng : 09-03-17
	m_sOnlineServerNo = ( short )atoi( tmpStr.c_str() );			//wangtiefeng : 09-03-17
	config.ReadItem( "SERVER" , "SERVERTYPE" , "0" , tmpStr );		//wangtiefeng : 09-03-19
	m_strServerType = tmpStr;										//wangtiefeng : 09-03-19
	config.ReadItem("SERVER", "UPLOADINTERVAL", "30", tmpStr);
	m_nUploadInterval = atoi(tmpStr.c_str());
	config.ReadItem("SERVER", "MINDOWNPERCENT", "0", tmpStr);
	int nDownPercent = atoi(tmpStr.c_str());
	if ((nDownPercent < 0 || nDownPercent > 100) && (unsigned)nDownPercent != g_nDownPercentUnknow)
	{
		WriteRunInfo::WriteError("CMyConfig::Init : MINDOWNPERCENT is not proper");
		return -1;
	}
	m_nMinDownPercent = (unsigned char)nDownPercent;

	config.ReadItem("SERVER", "DBUPDATETHREADNUM", "1", tmpStr);		//add by yinhaichen 2012-12-04
	m_nDbUpdateThreadNum = atoi(tmpStr.c_str());

	//read statistics output interval
	tmpStr.clear();
	config.ReadItem("SERVER", "STATINTERVAL", "300", tmpStr);
	m_nStatInterval = atoi(tmpStr.c_str());
	if (m_nStatInterval <= 0)
	{
		WriteRunInfo::WriteError("CMyConfig::Init : STATINTERVAL is not proper");
		return -1;
	}
	
	//added by dijie 2009-3-26
	config.ReadItem("MySql", "RCTABLENUM", "10", tmpStr);
	m_nRcTableNum = atoi(tmpStr.c_str());
	config.ReadItem("MySql", "ONLINEDBNUM", "1", tmpStr);
	m_nOnlineDBNum = atoi(tmpStr.c_str());
	config.ReadItem("MySql", "RESOURCEDBNUM", "1", tmpStr);
	m_nResourceDBNum = atoi(tmpStr.c_str());
	if (m_nResourceDBNum < m_nOnlineDBNum)
	{
		WriteRunInfo::WriteLog("config file error.");
		return -1;
	}

	config.ReadItem( "MySql" , "ONLINEDBONTIMEINTERVAL" , "0" , tmpStr );
	m_nOnlineDBOntimeInterval = atoi( tmpStr.c_str() );
	if( m_nOnlineDBOntimeInterval <= 0 )
	{
		WriteRunInfo::WriteLog("Config file error : read ONLINEDBONTIMEINTERVAL failed!");
		return -1;
	}

	m_vOnlineDBIndex.clear();
	for (int i = 0, itmpIndex = 0; i < m_nResourceDBNum; ++i)
	{
		std::stringstream strKeyflag;
		strKeyflag.clear();
		strKeyflag << "DB";
		strKeyflag << i;
		CONFIGDBITEM tConfigDBItem;
		config.ReadItem(strKeyflag.str().c_str(), "ip", "", tConfigDBItem.strMySqlIp);
		if( tConfigDBItem.strMySqlIp == "" )
		{
			WriteRunInfo::WriteLog("Read config file error : ip of %s ", strKeyflag.str().c_str());
			return -1;
		}
		config.ReadItem(strKeyflag.str().c_str(), "user", "root", tConfigDBItem.strMySqlUser);
		config.ReadItem(strKeyflag.str().c_str(), "password", "", tConfigDBItem.strMySqlPassword);
		string strIsonlineDB;
		config.ReadItem(strKeyflag.str().c_str(), "isonlinedb", "true", strIsonlineDB);
		if ("true" == strIsonlineDB)
		{
			m_vOnlineDBIndex.push_back(i);
			++itmpIndex;
		}
		m_vConfigDBItem.push_back(tConfigDBItem);
	}
	
	/*---------------added end    2009-3-26------------------------*/

	//read the number of pingserver syn process thread
	tmpStr.clear();
	config.ReadItem("SERVER", "PINGSYNPROCESSTHREADNUM", "1", tmpStr);
	if (tmpStr.empty())
	{
		WriteRunInfo::WriteError("CMyConfig::Init : PINGSYNPROCESSTHREADNUM is not proper");
		return -1;
	}
	m_nPingSynProcessThreadNum = atoi(tmpStr.c_str());
	if (m_nPingSynProcessThreadNum <= 0)
	{
		WriteRunInfo::WriteLog("CMyConfig::Init : PINGSYNPROCESSTHREADNUM is not proper");
		return -1;
	}
	
	//read multicast configure
	{
		//read multicast address to join in
		tmpStr.clear();
		config.ReadItem("MULTICAST", "MADDR", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteError("CMyConfig::Init : MADDR is not proper");
			return -1;
		}
		m_strMAddr = tmpStr;
		//read multicast receive port
		tmpStr.clear();
		config.ReadItem("MULTICAST", "RECVPORT", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteError("CMyConfig::Init : PORT is not proper");
			return -1;
		}
		m_nMRecvPort = atoi(tmpStr.c_str());
		//read multicast inbound interface address
		tmpStr.clear();
		config.ReadItem("MULTICAST", "INIFADDR", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteError("CMyConfig::Init : INIFADDR is not proper");
			return -1;
		}
		m_strMInIFAddr = tmpStr;
	}

	//read pmgrsrv DB configure
	{
		//Read whether use domain to access pmgrsrv DB
		tmpStr.clear();		
		config.ReadItem("PMGRDB", "IFUSEDOMAIN", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : IFUSEDOMAIN");
			return -1;
		}
		m_bUseMgrDbDomain = ((atoi(tmpStr.c_str()) == 0) ? false : true);
		//Read pmgrsrv DB domain
		if (m_bUseMgrDbDomain)
		{
			tmpStr.clear();		
			config.ReadItem("PMGRDB", "DBDOMAIN", "", tmpStr);
			if (tmpStr.empty())
			{
				WriteRunInfo::WriteLog("Read config file error : DBDOMAIN");
				return -1;
			}
			m_strMgrDbDomain = tmpStr;
		}
		//Read pmgrsrv DB IP
		tmpStr.clear();		
		config.ReadItem("PMGRDB", "DBIP", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : DBIP");
			return -1;
		}
		m_strMgrDbIp = tmpStr;
		//Read pmgrsrv DB user
		tmpStr.clear();		
		config.ReadItem("PMGRDB", "DBUSER", "root", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : DBUSER");
			return -1;
		}
		m_strMgrDbUser = tmpStr;
		//Read pmgrsrv DB password
		tmpStr.clear();		
		config.ReadItem("PMGRDB", "DBPASSWD", "", tmpStr);
		m_strMgrDbPasswd = tmpStr;
		//Read pmgrsrv DB name
		tmpStr.clear();		
		config.ReadItem("PMGRDB", "DBNAME", "", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : DBNAME");
			return -1;
		}
		m_strMgrDbName = tmpStr;
		//Read interval of updating config from pmgrsrv DB
		tmpStr.clear();
		config.ReadItem("PMGRDB", "UPDATEINTERVAL", "30", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : UPDATEINTERVAL");
			return -1;
		}
		m_nUpdateMgrDbConfigInterval = atoi(tmpStr.c_str());
		if (m_nUpdateMgrDbConfigInterval <= 0)
		{
			WriteRunInfo::WriteLog("invalid PMGRDB/UPDATEINTERVAL");
			return -1;
		}
		//Read resource DB type in pmgrsrv DB table
		tmpStr.clear();
		config.ReadItem("PMGRDB", "RESOURCEDBTYPE", "2", tmpStr);
		if (tmpStr.empty())
		{
			WriteRunInfo::WriteLog("Read config file error : RESOURCEDBTYPE");
			return -1;
		}
		m_nResourceDbType = atoi(tmpStr.c_str());
	}

	Print();
	return 0;
}


int CMyConfig::GetUploadInterval()
{
	return	m_nUploadInterval;
}
int CMyConfig::GetDbUpdateThreadNum()
{
	return m_nDbUpdateThreadNum;
}
int CMyConfig::GetStatInterval()
{
	return m_nStatInterval;
}
int CMyConfig::GetHashLeng()
{
	return m_nHashLeng;
}

int CMyConfig::GetOnlineServerNo()
{
	return m_sOnlineServerNo;
}

string CMyConfig::GetServerType()
{
	return m_strServerType;
}

int CMyConfig::GetMySqlDbInfo(string &ip, string &user, string &password)
{
	ip = m_strMySqlIp;
	user = m_strMySqlUser;
	password = m_strMySqlPassword;
	return 0;
}


int CMyConfig::GetOnlineDBInfo(string &ip, string &user, string &password, int index)
{
	if (index >= m_nOnlineDBNum)
	{
		return -1;
	}

	m_rwlockConfigDBItem.GetReadLock();
	ip = m_vConfigDBItem[m_vOnlineDBIndex[index]].strMySqlIp;
	user = m_vConfigDBItem[m_vOnlineDBIndex[index]].strMySqlUser;
	password = m_vConfigDBItem[m_vOnlineDBIndex[index]].strMySqlPassword;
	m_rwlockConfigDBItem.ReleaseLock();
	
	return 0;
}


int CMyConfig::GetResourceDBInfo(string &ip, string &user, string &password, int index)
{
	if (index >= m_nResourceDBNum)
	{
		return -1;
	}

	m_rwlockConfigDBItem.GetReadLock();
	ip = m_vConfigDBItem[index].strMySqlIp;
	user = m_vConfigDBItem[index].strMySqlUser;
	password = m_vConfigDBItem[index].strMySqlPassword;	
	m_rwlockConfigDBItem.ReleaseLock();

	return 0;
}

int CMyConfig::GetResourceDbIp(string &strIp, int nIndex)
{
	if (nIndex >= m_nResourceDBNum)
	{
		return -1;
	}

	m_rwlockConfigDBItem.GetReadLock();
	strIp = m_vConfigDBItem[nIndex].strMySqlIp;
	m_rwlockConfigDBItem.ReleaseLock();
	
	return 0;
}

int CMyConfig::SetResourceDbIp(const string &strIp, int nIndex)
{
	if (nIndex >= m_nResourceDBNum)
	{
		return -1;
	}

	m_rwlockConfigDBItem.GetWriteLock();
	m_vConfigDBItem[nIndex].strMySqlIp = strIp;
	m_rwlockConfigDBItem.ReleaseLock();
	
	return 0;
}

int CMyConfig::GetOnlineDBNum() 
{
	return m_nOnlineDBNum;
}

int CMyConfig::GetOnlineDBOntimeInterval()
{
	return m_nOnlineDBOntimeInterval;
}

int CMyConfig::GetResourceDBNum()
{
	return m_nResourceDBNum;
}

int CMyConfig::GetRcTableNum()
{
	return m_nRcTableNum;
}

int CMyConfig::GetOnlineDBIndex(int index)
{
	if (index >= m_nOnlineDBNum)
	{
		return -1;
	}
	return m_vOnlineDBIndex[index];
}

unsigned char CMyConfig::GetMinDownPercent()
{
	return m_nMinDownPercent;
}

int CMyConfig::GetPingSynProcessThreadNum() const
{
	return m_nPingSynProcessThreadNum;
}

const string& CMyConfig::GetMAddr() const
{
	return m_strMAddr;
}

unsigned short CMyConfig::GetMRecvPort() const
{
	return m_nMRecvPort;
}

const string& CMyConfig::GetMInIFAddr() const
{
	return m_strMInIFAddr;
}

bool CMyConfig::IfUseMgrDbDomain() const
{
	return m_bUseMgrDbDomain;
}

const string& CMyConfig::GetMgrDbDomain() const
{
	return m_strMgrDbDomain;
}

const string& CMyConfig::GetMgrDbIp() const
{
	return m_strMgrDbIp;
}

const string& CMyConfig::GetMgrDbUser() const
{
	return m_strMgrDbUser;
}

const string& CMyConfig::GetMgrDbPasswd() const
{
	return m_strMgrDbPasswd;
}

const string& CMyConfig::GetMgrDbName() const
{
	return m_strMgrDbName;
}

int CMyConfig::GetIntervalOfUpdateMgrConfig() const
{
	return m_nUpdateMgrDbConfigInterval;
}

int CMyConfig::GetResourceDbType() const
{
	return m_nResourceDbType;
}

void CMyConfig::Print() const
{
	WriteRunInfo::WriteLog("prsrv configure:");
	WriteRunInfo::WriteLog("SERVER/HASHLENG [%u]", m_nHashLeng);
	WriteRunInfo::WriteLog("SERVER/ONLINESERVERNO [%u]", m_sOnlineServerNo);
	WriteRunInfo::WriteLog("SERVER/SERVERTYPE [%s]", m_strServerType.c_str());
	WriteRunInfo::WriteLog("SERVER/UPLOADINTERVAL [%u]", m_nUploadInterval);
	WriteRunInfo::WriteLog("SERVER/MINDOWNPERCENT [%u]", m_nMinDownPercent);
	WriteRunInfo::WriteLog("SERVER/DBUPDATETHREADNUM [%u]", m_nDbUpdateThreadNum);
	WriteRunInfo::WriteLog("SERVER/PINGSYNPROCESSTHREADNUM [%u]", m_nPingSynProcessThreadNum);
	WriteRunInfo::WriteLog("SERVER/STATINTERVAL [%u]", m_nStatInterval);		
	WriteRunInfo::WriteLog("MySql/RCTABLENUM [%u]", m_nRcTableNum);
	WriteRunInfo::WriteLog("MySql/ONLINEDBNUM [%u]", m_nOnlineDBNum);
	WriteRunInfo::WriteLog("MySql/RESOURCEDBNUM [%u]", m_nResourceDBNum);
	WriteRunInfo::WriteLog("MySql/ONLINEDBONTIMEINTERVAL [%u]", m_nOnlineDBOntimeInterval);
	for (int i = 0; i < (int)m_vConfigDBItem.size(); ++i)
	{
		WriteRunInfo::WriteLog("resource DB%d/ip [%s]", i, m_vConfigDBItem[i].strMySqlIp.c_str());
		WriteRunInfo::WriteLog("resource DB%d/user [%s]", i, m_vConfigDBItem[i].strMySqlUser.c_str());
		WriteRunInfo::WriteLog("resource DB%d/password [%s]", i, m_vConfigDBItem[i].strMySqlPassword.c_str());
	}
	for (int i = 0; i < (int)m_vOnlineDBIndex.size(); ++i)
	{
		WriteRunInfo::WriteLog("online DB index %d", m_vOnlineDBIndex[i]);
	}
	WriteRunInfo::WriteLog("MULTICAST/MADDR [%s]", m_strMAddr.c_str());
	WriteRunInfo::WriteLog("MULTICAST/RECVPORT [%u]", m_nMRecvPort);
	WriteRunInfo::WriteLog("MULTICAST/INIFADDR [%s]", m_strMInIFAddr.c_str());
	WriteRunInfo::WriteLog("PMGRDB/IFUSEDOMAIN [%u]", m_bUseMgrDbDomain);
	WriteRunInfo::WriteLog("PMGRDB/DBDOMAIN [%s]", m_strMgrDbDomain.c_str());
	WriteRunInfo::WriteLog("PMGRDB/DBIP [%s]", m_strMgrDbIp.c_str());
	WriteRunInfo::WriteLog("PMGRDB/DBUSER [%s]", m_strMgrDbUser.c_str());
	WriteRunInfo::WriteLog("PMGRDB/DBPASSWD [%s]", m_strMgrDbPasswd.c_str());
	WriteRunInfo::WriteLog("PMGRDB/DBNAME [%s]", m_strMgrDbName.c_str());
	WriteRunInfo::WriteLog("PMGRDB/UPDATEINTERVAL [%u]", m_nUpdateMgrDbConfigInterval);
	WriteRunInfo::WriteLog("PMGRDB/RESOURCEDBTYPE [%u]", m_nResourceDbType);	
}

