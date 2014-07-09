
#ifndef __PHUB_CONNECT_CONF_H
#define __PHUB_CONNECT_CONF_H

#include "framecommon/framecommon.h"
#include <string>
using namespace std;
using namespace MYFRAMECOMMON;

#define CONFFILE "conf/inserttools.conf"

typedef struct
{
	string strIp;
	string strUserName;
	string strPasswd;
} DBITEM;

struct TConfItem
{
	int nSpeedPerSec;
	string strIp;
	int nPort;
	string strDbName;
	int nDbNum;
	int nTableNum;
	vector<DBITEM> vecDbItems;
};

class CConf
{
public:
	CConf();
	~CConf();

	int Init();
	TConfItem& GetConfItem()
	{
		return m_confItem;
	}

private:
	TConfItem m_confItem;
};

#endif
