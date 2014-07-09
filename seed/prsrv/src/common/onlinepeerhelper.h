#ifndef _ONLINE_PEER_HELPER_
#define _ONLINE_PEER_HELPER_

#include <list>
#include <string>
using namespace std;

#include "framecommon/cmysql.h"
using namespace MYFRAMECOMMON;

class COnlinePeerHelper
{
public:
	COnlinePeerHelper();
	virtual ~COnlinePeerHelper();

	int Init(int iIndex);
	void AddList(const list<string> &onlineList);
	void Export();

	int ClearOnlinePeerTable();
	int SendToDB(const list<string> &onlineList);

protected:
	list<string> m_OnlineList;

	CMysql m_mysql;

	int InitMySqlStuff(int iIndex);

};

#endif 

