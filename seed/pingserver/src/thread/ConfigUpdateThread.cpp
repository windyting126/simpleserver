#include "ConfigUpdateThread.h"
#include "ConfigUpdateTask.h"
#include "PingServer.h"

CConfigUpdateThread::CConfigUpdateThread(int iEntityType, int iEntityId)
	: CWorkThread(iEntityType, iEntityId)
{
}

int CConfigUpdateThread::Run()
{
	unsigned long long nCurTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
	unsigned long long ullLastUpdate = nCurTime;
	TPingServerConf &tConf = ((CPingServer*)CMyserver::GetInstance())->GetConf().GetConf();
	
	while (m_nRunFlag)
	{
		usleep(100000);
		nCurTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
		if (nCurTime >= ullLastUpdate + (unsigned long long)tConf.nUpdateMgrDbConfigInterval)
		{
			CConfigUpdateTask::GetInstance().UpdateQuerySrv();
			ullLastUpdate = nCurTime;
		}
	}
	
	return 0;
}

int CConfigUpdateThread::PutMsgToInputList(CMsg *pMsg)
{
	return 0;
}

int CConfigUpdateThread::Process(CMsg *pMsg)
{
	return 0;	
}

