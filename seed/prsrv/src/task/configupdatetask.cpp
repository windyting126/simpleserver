#include "configupdatetask.h"
#include "rserver.h"
#include "myconfig.h"
#include <string>
#include <vector>

#define MGR_DB_TALBE_PEER_SERVER_INFO "peer_server_info"

using namespace std;

CConfigUpdateTask& CConfigUpdateTask::GetInstance()
{
	static CConfigUpdateTask Instance;
	return Instance;
}

int CConfigUpdateTask::UpdateConfig()
{
	CMyConfig &Conf = ((CPRServer*)CMyserver::GetInstance())->GetMyConf();
	string pMgrDbIp;
	if (Conf.IfUseMgrDbDomain())
	{
		if (CLocalDomainParser::GetInstance()->ParseDomain(Conf.GetMgrDbDomain(), pMgrDbIp) != 0)
		{
			WriteRunInfo::WriteError("Parse server domain %s failed!", Conf.GetMgrDbDomain().c_str());
			pMgrDbIp = Conf.GetMgrDbIp();
		}
	}
	else
	{
		pMgrDbIp = Conf.GetMgrDbIp();		
	}

	char strTmp[1024] = {0};
	snprintf(strTmp, sizeof(strTmp),
			 "select machineindex, ifusepublicip, vpexternalip, vpinternalip from %s.%s where machinetype=%d order by machineindex;",
			 Conf.GetMgrDbName().c_str(), MGR_DB_TALBE_PEER_SERVER_INFO, Conf.GetResourceDbType());
	int nIndex = 0;
	char *pStrTmp;
	int nIp;
	vector<string> vecDbIp;
	
	CMysql mySql;
	try
	{	
		mySql.Init();
		mySql.Connect(pMgrDbIp.c_str(), Conf.GetMgrDbUser().c_str(), Conf.GetMgrDbPasswd().c_str());
		mySql.Query(strTmp);
		while (mySql.FetchRow())
		{
			pStrTmp = mySql.GetField("machineindex");
			if (!pStrTmp || atoi(pStrTmp) != nIndex)
			{
				WriteRunInfo::WriteError("null machineindex or invalid machineindex");
				return -1;
			}
			++nIndex;
			pStrTmp = mySql.GetField("ifusepublicip");
			if (!pStrTmp)
			{
				WriteRunInfo::WriteError("null ifusepublicip");
				return -1;
			}
			pStrTmp = (atoi(pStrTmp) == 0 ? mySql.GetField("vpinternalip") : mySql.GetField("vpexternalip"));				
			if (!pStrTmp)
			{
				WriteRunInfo::WriteError("null ip");
				return -1;
			}
			if (inet_pton(AF_INET, pStrTmp, &nIp) <= 0)
			{
				WriteRunInfo::WriteError("invalid ip address: [%s]", pStrTmp);
				return -1;
			}
			vecDbIp.push_back(pStrTmp);
		}
	}
	catch (CMysqlException &e)
	{
		WriteRunInfo::WriteError("CTaskManager::InitDataBase fail, error %s", e.GetErrMsg());
		return -1;
	}

	if (vecDbIp.size() != (unsigned)Conf.GetResourceDBNum())
	{
		WriteRunInfo::WriteError("unmatched DB number: config file %d, mgr DB %d", Conf.GetResourceDBNum(), vecDbIp.size());
		return -1;
	}

	string strIp;	
	for (int i = 0; i < (int)vecDbIp.size(); ++i)
	{
		if (Conf.GetResourceDbIp(strIp, i) == 0)
		{
			if (strIp != vecDbIp[i])
			{
				WriteRunInfo::WriteLog("DB IP changed: new [%s] old[%s]", vecDbIp[i].c_str(), strIp.c_str());
				Conf.SetResourceDbIp(vecDbIp[i], i);
			}
		}
		strIp.clear();
	}
	WriteRunInfo::WriteTrace("update DB configure");

	return 0;
}

