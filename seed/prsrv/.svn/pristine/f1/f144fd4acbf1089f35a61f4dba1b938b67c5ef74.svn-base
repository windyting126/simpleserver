#ifndef _DBTASK_H_
#define _DBTASK_H_

#include "task.h"
#include "framecommon/cmysql.h"
#include "mycommmsg.h"
#include "insertrccmd.h"
#include "reportrclistcmd.h"
#include "deleterclistcmd.h"
#include "deleterclistnewcmd.h"
#include <string>
#include <vector>
using namespace std;

class CDBTask : public CTask
{
public:
	CDBTask(ULONG64 taskId, CWorkThread *pThread);
	virtual ~CDBTask();
public:
	int Init();
	int Process(CMsg *pMsg);
public:
	int AddReportRcList(CReportRcListCmd* pCmd);
	int AddInsertRc(CInsertRcCmd* pCmd);
	int AddDeleteRc(CDeleteRcListCmd* pCmd);
	int AddDeleteRcNew(CDeleteRcListNewCmd* pCmd);

protected:
	int DestroySelf();
protected:
	int InitMySqlDb(int iIndex);

	int BuildReportRcListSql(CReportRcListCmd* pCmd, string& strResultSql, int nRcIndex, int nDBIndex);
	int BuildInsertRcSql(CInsertRcCmd* pCmd, string& strResultSql, int nDBIndex);
	int BuildDeleteRcSql(CDeleteRcListCmd* pCmd, string& strResultSql, int nRcIndex, int nDBIndex);
	int BuildDeleteRcNewSql(CDeleteRcListNewCmd* pCmd, string& strResultSql, int nRcIndex, int nDBIndex);
	
	int GenDBIndex(const string& strCid);
	int GenRcTableIndex(const string& strCid);
protected:
	int m_nOnlineDBNum;
	int m_nResourceDBNum;
	int m_nRcTableNum;
	vector <CMysql *> m_vMysql;
};
#endif

