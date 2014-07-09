
#include "insertrctoolsconf.h"
#include <sstream>

CConf::CConf()
{

}

CConf::~CConf()
{

}

int CConf::Init()
{
	Magic_Config conf (string(CONFFILE));

	int nRet = conf.Init();
	if (0 != nRet)
	{
		WriteRunInfo::WriteError ("Read config file error.");
		return -1;
	}

	string strTmp;
	conf.ReadItem("SERVERINFO", "SPEEDPERSEC", "", strTmp);
	if (0 == strTmp.length())
	{
		WriteRunInfo::WriteError("config file error : read SPEEDPERSEC error");
		return -1;
	}
	m_confItem.nSpeedPerSec = atoi(strTmp.c_str());
	
	conf.ReadItem("SERVERINFO", "SERVERIP", "", strTmp);
	if (0 == strTmp.length())
	{
		WriteRunInfo::WriteError("config file error : read SERVERIP error");
		return -1;
	}
	m_confItem.strIp = strTmp;

	conf.ReadItem("SERVERINFO", "SERVERPORT", "", strTmp);
	if (0 == strTmp.length())
	{
		WriteRunInfo::WriteError("config file error : read SERVERPORT error");
		return -1;
	}
	m_confItem.nPort = atoi(strTmp.c_str());

	conf.ReadItem("SERVERINFO", "DBNAME", "", strTmp);
	if (0 == strTmp.length())
	{
		WriteRunInfo::WriteError("config file error : read DBNAME error");
		return -1;
	}
	m_confItem.strDbName = strTmp;

	conf.ReadItem("SERVERINFO", "DBNUM", "1", strTmp);
	if (0 == strTmp.length())
	{
		WriteRunInfo::WriteError("config file error : read DBNUM error");
		return -1;
	}
	m_confItem.nDbNum = atoi(strTmp.c_str());

	conf.ReadItem("SERVERINFO", "TABLENUM", "11", strTmp);
	if (0 == strTmp.length())
	{
		WriteRunInfo::WriteError("config file error : read DBNUM error");
		return -1;
	}
	m_confItem.nTableNum = atoi(strTmp.c_str());

	for (int i = 0; i < m_confItem.nDbNum; i ++)
	{
		stringstream strName;
		strName << "DB";
		strName << i;
		DBITEM tDbItem;

		conf.ReadItem(strName.str().c_str(), "IP", "", strTmp);
		if (0 == strTmp.length())
		{
			WriteRunInfo::WriteError("config file error : read IP error");
			return -1;
		}
		tDbItem.strIp = strTmp;

		conf.ReadItem(strName.str().c_str(), "USERNAME", "", strTmp);
		if (0 == strTmp.length())
		{
			WriteRunInfo::WriteError("config file error : read USERNAME error");
			return -1;
		}
		tDbItem.strUserName = strTmp;

		conf.ReadItem(strName.str().c_str(), "PASSWD", "", strTmp);
		tDbItem.strPasswd = strTmp;

		m_confItem.vecDbItems.push_back(tDbItem);
	}

	return 0;
}

