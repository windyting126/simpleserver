#include "ConfigUpdateTask.h"
#include "PingServer.h"
#include "PingServerDef.h"
#include "framecommon/framecommon.h"
#include "EpollReactor.h"

#define MGR_DB_TALBE_PEER_SERVER_INFO "peer_server_info"

using namespace std;
using namespace MYFRAMECOMMON;

CConfigUpdateTask& CConfigUpdateTask::GetInstance()
{
	static CConfigUpdateTask Instance;
	return Instance;
}

int CConfigUpdateTask::Init()
{
	CPingServerConf &Conf = ((CPingServer*)CMyserver::GetInstance())->GetConf();
	
	Conf.GetQuerySrv(m_vecQuerySrv);
	UpdateQuerySrv(false);

	Conf.GetResourceSrv(m_vecResourceSrv);
	UpdateResourceSrv(false);
	
	return 0;
}

void CConfigUpdateTask::UpdateQuerySrv(bool bIfNotifyOnChanged/* = true*/)
{
	CPingServerConf &Conf = ((CPingServer*)CMyserver::GetInstance())->GetConf();

	vector<string> vecSrvIP;
	int nRet = GetConfig(Conf.GetConf().nPQSrvType, vecSrvIP);
	if (nRet < 0 || vecSrvIP.size() != m_vecQuerySrv.size())
	{
		WriteRunInfo::WriteError("get config fail or unmatched pqsrv number: new [%d] old [%d]", vecSrvIP.size(), m_vecQuerySrv.size());		
		return;
	}

	bool bConfigChanged = false;
	for (unsigned i = 0; i < m_vecQuerySrv.size(); ++i)
	{
		if (m_vecQuerySrv[i].strIp != vecSrvIP[i])
		{
			WriteRunInfo::WriteLog("query server configure changed, new IP [%s] old IP [%s]", vecSrvIP[i].c_str(), m_vecQuerySrv[i].strIp.c_str());
			m_vecQuerySrv[i].strIp = vecSrvIP[i];
			if (bIfNotifyOnChanged)
			{
				NotifyConfigChange(QSERVERMACHINETYPE, i, m_vecQuerySrv[i].strIp, m_vecQuerySrv[i].nPort);
			}
			bConfigChanged = true;
		}
	}
	if (bConfigChanged)
	{
		Conf.UpdateQuerySrv(m_vecQuerySrv);
	}
	WriteRunInfo::WriteTrace("update query server configure");
}

void CConfigUpdateTask::UpdateResourceSrv(bool bIfNotifyOnChanged/* = true*/)
{
	CPingServerConf &Conf = ((CPingServer*)CMyserver::GetInstance())->GetConf();

	vector<string> vecSrvIP;
	int nRet = GetConfig(Conf.GetConf().nPRSrvType, vecSrvIP);
	if (nRet < 0 || vecSrvIP.size() != m_vecResourceSrv.size())
	{
		WriteRunInfo::WriteError("get config fail or unmatched prsrv number: new [%d] old [%d]", vecSrvIP.size(), m_vecResourceSrv.size());		
		return;
	}

	bool bConfigChanged = false;
	for (unsigned i = 0; i < m_vecResourceSrv.size(); ++i)
	{
		if (m_vecResourceSrv[i].strIp != vecSrvIP[i])
		{
			WriteRunInfo::WriteLog("resource server configure changed, new IP [%s] old IP [%s]", vecSrvIP[i].c_str(), m_vecResourceSrv[i].strIp.c_str());
			m_vecResourceSrv[i].strIp = vecSrvIP[i];
			if (bIfNotifyOnChanged)
			{
				NotifyConfigChange(RSERVERMACHINETYPE, i, m_vecResourceSrv[i].strIp, m_vecResourceSrv[i].nPort);
			}
			bConfigChanged = true;
		}
	}
	if (bConfigChanged)
	{
		Conf.UpdateResourceSrv(m_vecResourceSrv);
	}
	WriteRunInfo::WriteTrace("update resource server configure");	
}

int CConfigUpdateTask::GetConfig(int nType, std::vector<std::string> &vecSrvIp) const
{
	TPingServerConf &tConf = ((CPingServer*)CMyserver::GetInstance())->GetConf().GetConf();
	
	string pMgrDbIp;
	if (tConf.bUseMgrDbDomain)
	{
		if (CLocalDomainParser::GetInstance()->ParseDomain(tConf.strMgrDbDomain, pMgrDbIp) != 0)
		{
			WriteRunInfo::WriteError("Parse server domain %s failed!", tConf.strMgrDbDomain.c_str());
			pMgrDbIp = tConf.strMgrDbIp;
		}
	}
	else
	{
		pMgrDbIp = tConf.strMgrDbIp;
	}

	char strTmp[1024] = {0};
	snprintf(strTmp, sizeof(strTmp),
			 "select machineindex, ifusepublicip, vpexternalip, vpinternalip from %s.%s where machinetype=%d order by machineindex;",
			 tConf.strMgrDbName.c_str(), MGR_DB_TALBE_PEER_SERVER_INFO, nType);
	int nIndex = 0;
	int nRet = 0;
	char *pStrTmp;
	int nIp;
	
	CMysql mySql;
	try
	{	
		mySql.Init();
		mySql.Connect(pMgrDbIp.c_str(), tConf.strMgrDbUser.c_str(), tConf.strMgrDbPasswd.c_str());
		mySql.Query(strTmp);
		while (mySql.FetchRow())
		{
			pStrTmp = mySql.GetField("machineindex");
			if (!pStrTmp || atoi(pStrTmp) != nIndex)
			{
				WriteRunInfo::WriteError("null machineindex or invalid machineindex");
				nRet = -1;
				break;
			}
			++nIndex;
			pStrTmp = mySql.GetField("ifusepublicip");
			if (!pStrTmp)
			{
				WriteRunInfo::WriteError("null ifusepublicip");
				nRet = -1;
				break;
			}
			pStrTmp = (atoi(pStrTmp) == 0 ? mySql.GetField("vpinternalip") : mySql.GetField("vpexternalip"));				
			if (!pStrTmp)
			{
				WriteRunInfo::WriteError("null ip");
				nRet = -1;
				break;
			}
			if (inet_pton(AF_INET, pStrTmp, &nIp) <= 0)
			{
				WriteRunInfo::WriteError("invalid ip address: [%s]", pStrTmp);
				nRet = -1;
				break;
			}
			vecSrvIp.push_back(pStrTmp);
		}
	}
	catch (CMysqlException &e)
	{
		WriteRunInfo::WriteError("CTaskManager::InitDataBase fail, error %s", e.GetErrMsg());
		return -1;
	}

	return nRet;
}

void CConfigUpdateTask::NotifyConfigChange(unsigned char ucMachineType, unsigned short usMachineIndex, const std::string &strIp, short nPort) const
{
	CEpollReactor::GetInstance()->ReConnect(ucMachineType, usMachineIndex, strIp, nPort, 3);
}

