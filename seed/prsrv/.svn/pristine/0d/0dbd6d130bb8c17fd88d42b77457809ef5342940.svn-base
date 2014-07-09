#include "dbtask.h"
#include "dbupdatethread.h"

#include "onlinepeerhelper.h"
#include "pingpeerhashtable.h"
#include "rserver.h"
#include "statistic.h"
#include "rsrvdef.h"

#include <vector>
#include <string>
#include <iostream>
#include <map>
using namespace std;

CDBTask::CDBTask(ULONG64 taskId, CWorkThread *pThread)
	: CTask(taskId, pThread)
{
	m_nOnlineDBNum = ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetOnlineDBNum();
	m_nResourceDBNum = ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetResourceDBNum();
	m_nRcTableNum = ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetRcTableNum();
	for (int i = 0; i < m_nResourceDBNum; ++i)
	{
		CMysql * pmysql = new CMysql;
		m_vMysql.push_back(pmysql);
	}
}

CDBTask::~CDBTask()
{
	for (vector<CMysql *>::iterator ite = m_vMysql.begin(); ite != m_vMysql.end(); ++ite)
	{
		delete *ite;
		*ite = NULL;
	}
	m_vMysql.clear();
}

int CDBTask::Init()
{
	
	for(int i = 0; i < m_nResourceDBNum; ++i)
	{
		if (0 != InitMySqlDb(i))
		{
			return -1;
		}
		//判断资源文件表 与 资源在线内存表 结构是否存在,若不存在则创建
		char szTmp[1024];
		string strSql;
		string strRcTableName;
		string strRcOnlineName;
		string strRcTableTmpName;
		for (int j = 0; j < m_nRcTableNum; ++j)
		{
			memset(szTmp, 0, sizeof(szTmp));
			strRcTableName = g_strRcTableName + CToolKit::IntToStr(j);
			snprintf(szTmp, sizeof(szTmp), CREATE_RESOURCE_TABLE, g_strPeerRCDB.c_str(), strRcTableName.c_str());
			strSql.assign(szTmp);
			try
			{
				WriteRunInfo::WriteRunLog("[CDBTask::Init] %s",strSql.c_str());
				m_vMysql[i]->Query(strSql);
			}
			catch(CMysqlException& e)
			{
				WriteRunInfo::WriteError("[CDBTask::Init] %s",e.GetErrMsg());
				return -1;
			}

			memset(szTmp, 0, sizeof(szTmp));
			strRcOnlineName = g_strRcOnlineTableName + CToolKit::IntToStr(j);
			snprintf(szTmp, sizeof(szTmp), CREATE_RESOUCE_ONLINE_TEMP_TABLE, g_strPeerRCDB.c_str(), strRcOnlineName.c_str());
			strSql.assign(szTmp);
			try
			{
				WriteRunInfo::WriteRunLog("[CDBTask::Init] %s",strSql.c_str());
				m_vMysql[i]->Query(strSql);
			}
			catch(CMysqlException& e)
			{
				WriteRunInfo::WriteError("[CDBTask::Init] %s",e.GetErrMsg());
				return -1;
			}

			memset(szTmp, 0, sizeof(szTmp));
			strRcTableTmpName = g_strRcTableTempName + CToolKit::IntToStr(j);
			snprintf(szTmp, sizeof(szTmp), CREATE_RESOURCE_TEMP_TABLE, g_strPeerRCDB.c_str(), strRcTableTmpName.c_str());
			strSql.assign(szTmp);
			try
			{
				WriteRunInfo::WriteRunLog("[CDBTask::Init] %s", strSql.c_str());
				m_vMysql[i]->Query(strSql);
			}
			catch(CMysqlException& e)
			{
				WriteRunInfo::WriteError("[CDBTask::Init] %s",e.GetErrMsg());
				return -1;
			}
		}
	}
	return 0;
}

int CDBTask::Process(CMsg *pMsg)
{
	return 0;
}

int CDBTask::DestroySelf()
{
	delete this;
	return 0;
}

int CDBTask::AddReportRcList(CReportRcListCmd* pCmd)
{
	unsigned char nMinDownPercent = ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetMinDownPercent();
	vector<TReportRc>::iterator itrReportRc = pCmd->GetInCmdBody().m_vecReportRcList.begin();
	int nRcIndex = 0;
	CCmdCounter* pCmdHandler = CCmdCounter::GetHandler();
	while (itrReportRc != pCmd->GetInCmdBody().m_vecReportRcList.end())
	{
		if (itrReportRc->m_nDownloadScale != g_nDownPercentUnknow && itrReportRc->m_nDownloadScale < nMinDownPercent)
		{
			continue;
		}
		string strSql;
		int nDbIndex = GenDBIndex(itrReportRc->m_strGcid);
		if (InitMySqlDb(nDbIndex) != 0) return -1;

		if (BuildReportRcListSql(pCmd, strSql, nRcIndex, nDbIndex) < 0) return -1;

		try
		{
			WriteRunInfo::WriteRunLog("[CDBTask::AddReportRcList] %s", strSql.c_str());
			m_vMysql[nDbIndex]->Query(strSql);
		}
		catch (CMysqlException& e)
		{
			WriteRunInfo::WriteError("[CDBTask::AddReportRcList] %s", e.GetErrMsg());
			return -1;
		}

		++ itrReportRc;
		++ nRcIndex;

		pCmdHandler->AddCount(INSERTDB_COUNT);
	}
	return 0;
}

int CDBTask::AddInsertRc(CInsertRcCmd* pCmd)
{
	unsigned char nMinDownPercent = ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetMinDownPercent();
	if (pCmd->GetInCmdBody().m_nDownloadScale != g_nDownPercentUnknow && pCmd->GetInCmdBody().m_nDownloadScale < nMinDownPercent)
	{
		return 0;
	}

	string strGcid = pCmd->GetInCmdBody().m_strGcid;
	string strSql;
	int nDBIndex = GenDBIndex(strGcid);
	if (InitMySqlDb(nDBIndex) != 0) return -1;

	if (BuildInsertRcSql(pCmd, strSql, nDBIndex) < 0) return -1;

	try
	{
		WriteRunInfo::WriteRunLog("[CDBTask::AddInsertRcMv] %s", strSql.c_str());
		m_vMysql[nDBIndex]->Query(strSql);
	}
	catch (CMysqlException &e)
	{
		WriteRunInfo::WriteError("[CDBTask::AddInsertRcMv] %s", e.GetErrMsg());
		return -1;
	}

	CCmdCounter* pCmdHandler = CCmdCounter::GetHandler();
	pCmdHandler->AddCount(INSERTDB_COUNT);
	return 0;
}

int CDBTask::AddDeleteRc(CDeleteRcListCmd* pCmd)
{

	vector<TDeleteRc>::iterator itrDeleteRc =  pCmd->GetInCmdBody().m_vecDeleteRcLst.begin();
	int nRcIndex = 0;
	CCmdCounter* pCmdHandler = CCmdCounter::GetHandler();
	while (itrDeleteRc != pCmd->GetInCmdBody().m_vecDeleteRcLst.end())
	{
		string strSql;
		int nDbIndex = GenDBIndex(itrDeleteRc->m_strGcid);
		if (InitMySqlDb(nDbIndex) != 0) return -1;

		if (BuildDeleteRcSql(pCmd, strSql, nRcIndex, nDbIndex) < 0) return -1;

		try
		{
			WriteRunInfo::WriteRunLog("[CDBTask::AddDeleteRc] %s", strSql.c_str());
			m_vMysql[nDbIndex]->Query(strSql);
		}
		catch (CMysqlException& e)
		{
			WriteRunInfo::WriteError("[CDBTask::AddDeleteRc] %s",e.GetErrMsg());
			return -1; 
		}
		++ itrDeleteRc;
		++ nRcIndex;

		pCmdHandler->AddCount(DELETEDB_COUNT);
		
	}
	
	return 0;

}

int CDBTask::AddDeleteRcNew(CDeleteRcListNewCmd* pCmd)
{

	vector<TDeleteRc>::iterator itrDeleteRc =  pCmd->GetInCmdBody().m_vecDeleteRcLst.begin();
	int nRcIndex = 0;
	CCmdCounter* pCmdHandler = CCmdCounter::GetHandler();
	while (itrDeleteRc != pCmd->GetInCmdBody().m_vecDeleteRcLst.end())
	{
		string strSql;
		int nDbIndex = GenDBIndex(itrDeleteRc->m_strGcid);
		if (InitMySqlDb(nDbIndex) != 0) return -1;

		if (BuildDeleteRcNewSql(pCmd, strSql, nRcIndex, nDbIndex) < 0) return -1;

		try
		{
			WriteRunInfo::WriteRunLog("[CDBTask::AddDeleteRc] %s", strSql.c_str());
			m_vMysql[nDbIndex]->Query(strSql);
		}
		catch (CMysqlException& e)
		{
			WriteRunInfo::WriteError("[CDBTask::AddDeleteRc] %s",e.GetErrMsg());
			return -1; 
		}
		++ itrDeleteRc;
		++ nRcIndex;

		pCmdHandler->AddCount(DELETEDB_COUNT);
	}

	return 0;

}

int CDBTask::InitMySqlDb(int iIndex)
{
	try
	{
		CMysql * pMysql = m_vMysql[iIndex];
		std::string strMySqlHost;
	    std::string strMySqlUser;
	    std::string strMySqlPass;

	    ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetResourceDBInfo(strMySqlHost, strMySqlUser, strMySqlPass, iIndex);
		if (!pMysql->IfConnected(strMySqlHost))
		{

			pMysql->Init();
			my_bool bReconnect = 1;
			pMysql->SetOptions(MYSQL_OPT_RECONNECT, &bReconnect);
			pMysql->Connect(strMySqlHost, strMySqlUser, strMySqlPass, 20);
		}
	}
	catch (CMysqlException &e)
	{
		WriteRunInfo::WriteError("[CDBTask::InitMySqlDb] %s",e.GetErrMsg());
		return -1;
	}

	return 0;
}

int CDBTask::BuildReportRcListSql(CReportRcListCmd* pCmd, string& strResultSql, int nRcIndex, int nDBIndex)
{
	TReportRc& ReportRc =  pCmd->GetInCmdBody().m_vecReportRcList[nRcIndex];

	strResultSql = "REPLACE INTO ";
	strResultSql += g_strPeerRCDB;
	strResultSql += ".";

	//strResultSql += g_strRcTableName;
	strResultSql += g_strRcTableTempName;
	int nRcTableIndex = GenRcTableIndex(ReportRc.m_strGcid);
	strResultSql += CToolKit::IntToStr(nRcTableIndex);
	strResultSql +="(fcid,fpeerid,fpublic_ip,ffilesize,fflag,fcapability,ftime,downpercent)";
	strResultSql += " VALUES ";

	string strGcid = ReportRc.m_strGcid;
	string strPeerId = pCmd->GetInCmdBody().m_strPeerId;
	m_vMysql[nDBIndex]->EscapeString(strGcid);
	m_vMysql[nDBIndex]->EscapeString(strPeerId);

	char szTmp[512];
	snprintf(szTmp, sizeof(szTmp), "('%s','%s',%u,%lld,%u, %u, %u, %u),",strGcid.c_str(), strPeerId.c_str(), pCmd->GetIp(), ReportRc.m_nFileSize, 0,
		pCmd->GetInCmdBody().m_nCapability, (unsigned int)time(NULL), ReportRc.m_nDownloadScale);

	strResultSql += szTmp;

	strResultSql[strResultSql.size()-1] = ';';
	return 0;
}

int CDBTask::BuildInsertRcSql(CInsertRcCmd* pCmd, string& strResultSql, int nDBIndex)
{
	char szTmp[512];
	string strTbName = g_strRcTableTempName;
	string strGcid = pCmd->GetInCmdBody().m_strGcid;
	int nRcTableIndex = GenRcTableIndex(strGcid);
	string strPeerId = pCmd->GetInCmdBody().m_strPeerId;

	m_vMysql[nDBIndex]->EscapeString(strTbName);
	m_vMysql[nDBIndex]->EscapeString(strGcid);
	m_vMysql[nDBIndex]->EscapeString(strPeerId);

	strTbName += CToolKit::IntToStr(nRcTableIndex);

	snprintf(szTmp, sizeof(szTmp), "REPLACE INTO %s.%s(fcid,fpeerid,fpublic_ip,ffilesize,fflag,fcapability,ftime,downpercent) VALUES ('%s','%s',%u,%lld,%d,%u,%u,%u);",
		g_strPeerRCDB.c_str(), strTbName.c_str(), strGcid.c_str(), strPeerId.c_str(), pCmd->GetIp(), 
		pCmd->GetInCmdBody().m_nFileSize, 0, pCmd->GetInCmdBody().m_nCapability, (unsigned int)time(NULL), pCmd->GetInCmdBody().m_nDownloadScale);
	strResultSql = szTmp;
	return 0;
}


int CDBTask::BuildDeleteRcSql(CDeleteRcListCmd* pCmd, string &strResultSql, int nRcIndex, int iDBIndex)
{
	TDeleteRc& DeleteRc = pCmd->GetInCmdBody().m_vecDeleteRcLst[nRcIndex]; 

	string strTbName = g_strRcTableTempName;
	string strGcid = DeleteRc.m_strGcid;
	string strPeerId = pCmd->GetInCmdBody().m_strPeerId;

	int nRcTableIndex = GenRcTableIndex(strGcid);

	m_vMysql[iDBIndex]->EscapeString(strTbName);
	m_vMysql[iDBIndex]->EscapeString(strGcid);
	m_vMysql[iDBIndex]->EscapeString(strPeerId);

	strTbName += CToolKit::IntToStr(nRcTableIndex);

	char szTmp[512];
	snprintf(szTmp, sizeof(szTmp), "DELETE FROM %s.%s WHERE (fcid = '%s' AND fpeerid = '%s');",
		g_strPeerRCDB.c_str(), strTbName.c_str(), strGcid.c_str(), strPeerId.c_str());
	strResultSql = szTmp;
	return 0;
}

int CDBTask::BuildDeleteRcNewSql(CDeleteRcListNewCmd* pCmd, string &strResultSql, int nRcIndex, int iDBIndex)
{
	TDeleteRc& DeleteRc = pCmd->GetInCmdBody().m_vecDeleteRcLst[nRcIndex]; 

	string strTbName = g_strRcTableTempName;
	string strGcid = DeleteRc.m_strGcid;
	string strPeerId = pCmd->GetInCmdBody().m_strPeerId;

	int nRcTableIndex = GenRcTableIndex(strGcid);

	m_vMysql[iDBIndex]->EscapeString(strTbName);
	m_vMysql[iDBIndex]->EscapeString(strGcid);
	m_vMysql[iDBIndex]->EscapeString(strPeerId);

	strTbName += CToolKit::IntToStr(nRcTableIndex);

	char szTmp[512];
	snprintf(szTmp, sizeof(szTmp), "DELETE FROM %s.%s WHERE (fcid = '%s' AND fpeerid = '%s');",
		g_strPeerRCDB.c_str(), strTbName.c_str(), strGcid.c_str(), strPeerId.c_str());
	strResultSql = szTmp;
	return 0;
}

int CDBTask::GenDBIndex(const string& strCid)
{
	unsigned int uCid = CToolKit::shortELFhash(strCid.c_str(), strCid.length());
	int iRHashCid = uCid % ((unsigned int)m_nResourceDBNum);
	return iRHashCid;
}

int CDBTask::GenRcTableIndex(const string& strCid)
{
	unsigned int nCid = CToolKit::shortELFhash(strCid.c_str(), strCid.length());
	int nRcTableIndex = nCid % m_nRcTableNum;
	return nRcTableIndex;
} 


