#include "onlinepeerhelper.h"
#include <stdio.h>
#include <stdlib.h>
#include "rserver.h"
#include <string>
#include "framecommon/writeRunInfo.h"
#include "mycommmsg.h"
#include "rsrvdef.h"

using namespace std;

COnlinePeerHelper::COnlinePeerHelper() //altered by dijie 2009-3-26
{
}

COnlinePeerHelper::~COnlinePeerHelper()
{

}

int COnlinePeerHelper::Init(int iIndex)
{
	return InitMySqlStuff(iIndex);
}

void COnlinePeerHelper::AddList(const list<string> &onlineList)
{
	m_OnlineList.clear();
	m_OnlineList = onlineList;
}

void COnlinePeerHelper::Export()
{
	FILE *fp = fopen("./conf/online.list","w");
	if (!fp)
	{
		return;
	}

	list<string>::iterator itor = m_OnlineList.begin();
	while(itor != m_OnlineList.end())
	{
		string strPeerId = *itor;
		fprintf(fp, "%s\n",strPeerId.c_str());
		++itor;
	}

	fclose(fp);
}

int COnlinePeerHelper::ClearOnlinePeerTable()
{
	try
	{
		string strDelTable = "DELETE FROM ";
		strDelTable += g_strPeerRCDB;
		strDelTable += ".";
		strDelTable += "t_online_peer_list_temp";
		strDelTable += ";";
		WriteRunInfo::WriteRunLog("[COnlinePeerHelper::ClearOnlinePeerTable] %s", strDelTable.c_str());
		m_mysql.Query(strDelTable);
	}
	catch(CMysqlException &e)
	{
		WriteRunInfo::WriteError("[COnlinePeerHelper::ClearOnlinePeerTable] %s",e.GetErrMsg());
		return -1;

	}

	return 0;
}

int COnlinePeerHelper::SendToDB(const list<string> &onlineList)
{
	if (0 == onlineList.size())
	{
		return 0;
	}

	try
	{
		string strSql = "INSERT INTO ";
		strSql += g_strPeerRCDB;
		strSql += ".";

		strSql += "t_online_peer_list_temp ";
		strSql += "VALUES ";

		list<string>::const_iterator itor = onlineList.begin();
		while (itor != onlineList.end())
		{
			string strPid = *itor;

			m_mysql.EscapeString(strPid);
			/*
			printf("\n>>>>>>>>>>>>>>>>>>>>>>>>\n");
			for (int i=0; i<strPid.size(); ++i)
			{
				printf("%02x",(unsigned char)strPid[i]);
			}
			printf("\n>>>>>>>>>>>>>>>>>>>>>>>>\n");
			*/
			strSql += "('";
			strSql += strPid;
			strSql += "'),";

			itor++;
		}

		strSql[strSql.size()-1] = ';'; 

		WriteRunInfo::WriteRunLog("[COnlinePeerHelper::SendToDB] %s", strSql.c_str());
		m_mysql.Query(strSql);
	}
	catch(CMysqlException &e)
	{
		WriteRunInfo::WriteError("[COnlinePeerHelper::SendToDB] %s",e.GetErrMsg());
		return -1;
	}

	return 0;
}

int COnlinePeerHelper::InitMySqlStuff(int iIndex)
{
	string strIp, strUser, strPwd;
	((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetOnlineDBInfo(strIp, strUser, strPwd, iIndex);

	try
	{
		m_mysql.Init();
		my_bool bReconnect = 1;
		m_mysql.SetOptions(MYSQL_OPT_RECONNECT, &bReconnect);
		m_mysql.Connect(strIp, strUser, strPwd);
	}
	catch(CMysqlException &e)
	{
		WriteRunInfo::WriteError("[COnlinePeerHelper::InitMySqlStuff] %s",e.GetErrMsg());
		return -1;
	}

	return 0;
}

