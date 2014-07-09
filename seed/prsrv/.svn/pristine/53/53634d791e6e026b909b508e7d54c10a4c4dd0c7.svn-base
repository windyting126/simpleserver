#include "dbupdatethread.h"
#include "mycommmsg.h"
#include "insertrccmd.h"
#include "reportrclistcmd.h"
#include "deleterclistcmd.h"
#include "statistic.h"

CDBUpdateThread::CDBUpdateThread(int iEntityType, int iEntityId):CWorkThread(iEntityType, iEntityId)
, m_pDBTask(NULL)
{

}

CDBUpdateThread::~CDBUpdateThread()
{

}

int CDBUpdateThread::Init()
{
	m_pDBTask = new CDBTask(CTask::GentTaskId(), this);
	m_pDBTask->Init();									//创建连接及资源数据表
	return 0;
}

int CDBUpdateThread::GetMsgQueueLen()
{
	return (int)m_inputList.size();
}

int CDBUpdateThread::Run()
{
	while (m_nRunFlag)
	{
		//处理消息
		CMsg *pMsg = m_inputList.get();
		if (pMsg != NULL)
		{
			Process(pMsg);
			delete pMsg;
		}
	}

	return 0;
}

int CDBUpdateThread::Process(CMsg *pMsg)
{
	TMsg *pTMsg = pMsg->GetTMsg();

	if (pTMsg->srcEntityType == LONGCONNECTEDPROCESSENTITY && pTMsg->msgType == MSGTYPE_UPDATE_TO_DB)
	{
		CCmdMsgBody* pCmdBody = (CCmdMsgBody*)(pMsg->GetMsgBody());
		CCommand* pCmd = pCmdBody->GetCmd();
		
		CCmdCounter* pCmdHandler = CCmdCounter::GetHandler();
		pCmdHandler->AddCount(pCmdBody->GetCmdId());
		
		switch (pCmdBody->GetCmdId())
		{
		case SEEDCMD_REPORT_RCLIST:
			{
				m_pDBTask->AddReportRcList((CReportRcListCmd*)pCmd);
				break;
			}
		case SEEDCMD_INSERT_RC:
			{
				m_pDBTask->AddInsertRc((CInsertRcCmd*)pCmd);
				break;
			}
		case SEEDCMD_DELETE_RCLIST:
			{
				m_pDBTask->AddDeleteRc((CDeleteRcListCmd*)pCmd);
				break;
			}
		case SEEDCMD_DELETE_RCLISTNEW:
			{
				m_pDBTask->AddDeleteRcNew((CDeleteRcListNewCmd*)pCmd);
				break;
			}
		default:
			{
				break;
			}
		}
	}
	return 0;
}


int CDBUpdateThread::PutMsgToInputList(CMsg * pMsg)
{
	m_inputList.put(pMsg);
	return 0;
}




