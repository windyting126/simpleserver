#ifndef _DB_UPDATE_THREAD_H_
#define _DB_UPDATE_THREAD_H_

#include "workthread.h"
#include "dbtask.h"
#include "synqueue.h"
//#include "rcdatamgr.h"

class CDBUpdateThread : public CWorkThread
{
public:
	CDBUpdateThread(int iEntityType, int iEntityId);
	virtual ~CDBUpdateThread();

	int Run();
	int PutMsgToInputList(CMsg * pMsg);
	int Init();
	virtual int GetMsgQueueLen();

	//CRcDataMgr *GetRcDataMgr();

protected:
	int Process(CMsg *pMsg);         

	 CQueue m_inputList; 
	 //CRcDataMgr m_RcDataMgr;

	 CDBTask * m_pDBTask;
};

#endif

