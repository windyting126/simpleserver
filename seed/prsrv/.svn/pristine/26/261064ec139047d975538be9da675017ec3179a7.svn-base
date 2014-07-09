/* Copyright:2009-
* Version		: 1.0
* FileName		: onlinedbupdatethread.cpp
* Description	: 
*---- Class List	: 
*---- Function List	: 
*---- Variable List : 
*---- Related Files : 
* Author:       : wangtiefeng
* Created Date	: 2009-1-14
* 
* History		: 
*		<author>		  <time>		<version>		<description>
*		wangtiefeng		2009-1-14			1.0			
*/

#include "onlinedbupdatethread.h"
#include "rserver.h"
#include "pingpeerhashtable.h"
#include "onlinepeerhelper.h"
#include "statistic.h"
#include "rsrvdef.h"
#include "configupdatetask.h"

COnlineDBUpdateThread::COnlineDBUpdateThread()
{

}

COnlineDBUpdateThread::~COnlineDBUpdateThread()
{

}

int COnlineDBUpdateThread::Init()
{
	m_iOntimeInterval = ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetOnlineDBOntimeInterval();
	m_nOnlineDBNum = ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetOnlineDBNum();
	m_nStatInterval = ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetStatInterval();
	if(m_iOntimeInterval <= 0 || m_nOnlineDBNum < 0)
	{
		return -1;
	}
	return 0;
}

int COnlineDBUpdateThread::Run()
{
	int nCurTime = ((CPRServer*)(CMyserver::GetInstance()))->GetTimeVal().tv_sec;
	int nLastPrintCmdTime = nCurTime;
	int nLastConfigUpdateTime = nCurTime;
	int *pLastCheckTime = NULL;
	pLastCheckTime = new int[m_nOnlineDBNum];
	for(int i = 0; i < m_nOnlineDBNum; i ++)
	{
		pLastCheckTime[i] = nCurTime;
	}
	int nRet = 0;
	CMyConfig &Conf = ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf();

	CCmdCounter* pCmdHandler = CCmdCounter::GetHandler();

	while ( m_nRunFlag )
	{
		nCurTime = ((CPRServer*)(CMyserver::GetInstance()))->GetTimeVal().tv_sec;

		if (nCurTime - nLastPrintCmdTime >= m_nStatInterval)
		{
			pCmdHandler->WriteLog();
			CMyserver::GetInstance()->WriteMsgQueueLog(STATISTIC_LOG_TYPE);
			nLastPrintCmdTime = nCurTime;
		}

		for(int i = 0; i < m_nOnlineDBNum; i ++)
		{
			if(nCurTime - pLastCheckTime[i] >= m_iOntimeInterval)
			{
				nRet = UpdateRcOnlineTable(i);
				if(nRet == 0)
				{
					pLastCheckTime[i] = nCurTime;
				}
				else
				{
					pLastCheckTime[i] += MERGETABLE_DELAY;
				}
			}
		}

		if (nCurTime >= nLastConfigUpdateTime + Conf.GetIntervalOfUpdateMgrConfig())
		{
			//update resource DB information from pmgrsrv DB
			CConfigUpdateTask::GetInstance().UpdateConfig();
			nLastConfigUpdateTime = nCurTime;
		}

		usleep(10000);
	}

	if(pLastCheckTime != NULL)
	{
		delete [] pLastCheckTime;
	}
	return 0;
}

int COnlineDBUpdateThread::UpdateRcOnlineTable(int nIndex)
{
	struct timespec tm1, tm2, tm3;
	long lInsertDbInterval = 0;
	long lMergeDbInterval = 0;
	long lPeerNum = 0;
	int nRet = 0;

	nRet = CreateRcOnlineTempTable(nIndex);
	if(nRet != 0)
	{
		return -1;
	}
	nRet = DeleteRcOnlineTempTable(nIndex);
	if(nRet != 0)
	{
		return -1;
	}

	clock_gettime(CLOCK_MONOTONIC, &tm1);
	nRet = InsertPeerInfo(nIndex, lPeerNum);
	if(nRet != 0)
	{
		return -1;
	}
	clock_gettime(CLOCK_MONOTONIC, &tm2);
	lInsertDbInterval =  (tm2.tv_sec - tm1.tv_sec) * 1000000000 + (tm2.tv_nsec - tm1.tv_nsec);
	
	nRet = MergeTable(nIndex);
	if(nRet != 0)
	{
		return -1;
	}
	clock_gettime(CLOCK_MONOTONIC, &tm3);
	lMergeDbInterval = (tm3.tv_sec - tm2.tv_sec) * 1000000000 + (tm3.tv_nsec - tm2.tv_nsec);

	lInsertDbInterval = lInsertDbInterval / 1000000;
	lMergeDbInterval = lMergeDbInterval / 1000000;

	string strIp, strUser, strPwd;
	((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetOnlineDBInfo(strIp, strUser, strPwd, nIndex);
	WRITE_ALL_LOG(STATISTIC_LOG_TYPE, "<<merge>>:[ip: %s][peernum: %ld][insert: %ld millisec][merge: %ld millisec]", strIp.c_str(), lPeerNum, lInsertDbInterval, lMergeDbInterval);
	return 0;
}

int COnlineDBUpdateThread::InsertPeerInfo(int nIndex, long &lPeerNum)
{
	COnlinePeerHelper oph;
	int nRet = oph.Init(nIndex);
	if(nRet != 0)
	{
		return -1;
	}
	oph.ClearOnlinePeerTable();

	int dwCurPeerCount = 0;
	list<string> peer_list;
	CPingPeerHashTable *pOPS = CPingPeerHashTable::GetHashTable();
	if (pOPS)
	{
		for (int i=0; i<pOPS->GetHashLength(); ++i)
		{
			list<TPingPeerInfo> tmp_list;
			pOPS->GetPeerList(tmp_list, i);

			list<TPingPeerInfo>::iterator itor = tmp_list.begin();
			while(itor != tmp_list.end())
			{
				peer_list.push_back(itor->strPeerId);

				++dwCurPeerCount;

				if (dwCurPeerCount >= EACH_SENT_TO_DB_PEER_COUNT)
				{
					nRet = oph.SendToDB(peer_list);
					if(nRet != 0)
					{
						return -1;
					}
					lPeerNum += dwCurPeerCount;
					dwCurPeerCount = 0;
					peer_list.clear();
				}
				++itor;
			}
		}
	}

	if ( peer_list.size()!=0 )
	{
		nRet = oph.SendToDB(peer_list);
		if(nRet != 0)
		{
			return -1;
		}
		lPeerNum += (long)peer_list.size();
	}
	return 0;
}

int COnlineDBUpdateThread::MergeTable(int nIndex)
{
	int nRcTableNum = ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetRcTableNum();
	string strIp, strUser, strPwd, strSql;
	((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetOnlineDBInfo(strIp, strUser, strPwd, nIndex);

	CMysql mySql;
	try
	{
		mySql.Init();
		mySql.Connect(strIp.c_str(), strUser.c_str(), strPwd.c_str(), 20);
		for(int j = 0; j < nRcTableNum; j ++)
		{
			strSql = "delete from presource_peer_new.t_rc_peer_list_temp;";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();
				
			strSql = "insert into presource_peer_new.t_rc_peer_list_temp select * from presource_peer_new.t_rc_peer_list_";
			strSql += CToolKit::IntToStr(j);
			strSql += ";";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();

			strSql = "replace into presource_peer_new.t_rc_peer_list_temp select * from presource_peer_new.t_rc_peer_list_temp_";
			strSql += CToolKit::IntToStr(j);
			strSql += ";";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();

			strSql = "delete from presource_peer_new.t_rc_peer_list_temp where UNIX_TIMESTAMP(now()) - ftime > 2*86400;";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();

			strSql = "delete from presource_peer_new.t_rc_peer_list_temp_";
			strSql += CToolKit::IntToStr(j);
			strSql += ";";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();

			strSql = "delete from presource_peer_new.t_rc_online_peer_list_temp_temp;";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();

			strSql = "insert into presource_peer_new.t_rc_online_peer_list_temp_temp select a.fcid,a.fpeerid,a.fpublic_ip,";
			strSql += "a.ffilesize,a.fflag,a.fcapability,a.ftime,a.downpercent from presource_peer_new.t_rc_peer_list_temp AS a,";
			strSql += "presource_peer_new.t_online_peer_list_temp AS b WHERE a.fpeerid = b.fpeerid;";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();

			strSql = "rename table presource_peer_new.t_rc_online_peer_list_temp_";
			strSql += CToolKit::IntToStr(j);
			strSql += " to presource_peer_new.t_rc_online_peer_list_temp_;";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();

			strSql = "rename table presource_peer_new.t_rc_online_peer_list_temp_temp to presource_peer_new.t_rc_online_peer_list_temp_";
			strSql += CToolKit::IntToStr(j);
			strSql += ";";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();

			strSql = "rename table presource_peer_new.t_rc_online_peer_list_temp_ to presource_peer_new.t_rc_online_peer_list_temp_temp;";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();

			strSql = "delete from presource_peer_new.t_rc_online_peer_list_temp_temp;";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();

			strSql = "delete from presource_peer_new.t_rc_peer_list_";
			strSql += CToolKit::IntToStr(j);
			strSql += ";";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();

			strSql = "insert into presource_peer_new.t_rc_peer_list_";
			strSql += CToolKit::IntToStr(j);
			strSql += " select * from presource_peer_new.t_rc_online_peer_list_temp_";
			strSql += CToolKit::IntToStr(j);
			strSql += ";";
			mySql.Query(strSql.c_str());
			mySql.FreeResult();
		}

		strSql = "delete from presource_peer_new.t_rc_peer_list_temp;";
		mySql.Query(strSql.c_str());
		mySql.FreeResult();
	}
	catch(CMysqlException &e)
	{
		WriteRunInfo::WriteError("[COnlineDBUpdateThread::MergeTable] %s",e.GetErrMsg());
		return -1;
	}

	return 0;
}

int COnlineDBUpdateThread::CreateRcOnlineTempTable(int nIndex)
{
	char szTmp[1024];
	string strRcTableName;
	int nRcTableNum = ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetRcTableNum();
	string strIp, strUser, strPwd;

	((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetOnlineDBInfo(strIp, strUser, strPwd, nIndex);
	CMysql mySql;
	try
	{
		mySql.Init();
		mySql.Connect(strIp.c_str(), strUser.c_str(), strPwd.c_str(), 20);

		for (int j = 0; j < nRcTableNum; ++j)
		{
			memset(szTmp, 0, sizeof(szTmp));
			strRcTableName = g_strRcOnlineTableName + CToolKit::IntToStr(j);
			snprintf(szTmp, sizeof(szTmp), CREATE_RESOUCE_ONLINE_TEMP_TABLE, g_strPeerRCDB.c_str(), strRcTableName.c_str());
			mySql.Query(szTmp);
			mySql.FreeResult();
		}

		memset(szTmp, 0, sizeof(szTmp));
		strRcTableName = g_strRcOnlineTableName + "temp";
		snprintf(szTmp, sizeof(szTmp), CREATE_RESOUCE_ONLINE_TEMP_TABLE, g_strPeerRCDB.c_str(), strRcTableName.c_str());
		mySql.Query(szTmp);
		mySql.FreeResult();
	}
	catch(CMysqlException& e)
	{
		WriteRunInfo::WriteError("[COnlineDBUpdateThread::CreateRcOnlineTempTable] %s",e.GetErrMsg());
		return -1;
	}
	
	return 0;
}

int COnlineDBUpdateThread::DeleteRcOnlineTempTable(int nIndex)
{
	char szTmp[1024];
	string strRcTableName;
	string strIp, strUser, strPwd;

	((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetOnlineDBInfo(strIp, strUser, strPwd, nIndex);
	CMysql mySql;
	try
	{
		mySql.Init();
		mySql.Connect(strIp.c_str(), strUser.c_str(), strPwd.c_str(), 20);

		memset(szTmp, 0, sizeof(szTmp));
		strRcTableName = "t_rc_online_peer_list_temp_";
		snprintf(szTmp, sizeof(szTmp), "drop table if exists %s.%s;", g_strPeerRCDB.c_str(), strRcTableName.c_str());
		mySql.Query(szTmp);
		mySql.FreeResult();
	}
	catch(CMysqlException& e)
	{
		WriteRunInfo::WriteError("[COnlineDBUpdateThread::DeleteRcOnlineTempTable] %s",e.GetErrMsg());
		return -1;
	}
	
	return 0;
}
