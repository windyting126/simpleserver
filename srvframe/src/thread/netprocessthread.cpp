/*
 * FileName:       
 * Author:         wangqifeng  Version: 1.0  Date: 2005-12-9
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version >   <desc>
 */
#include "netprocessthread.h"
#include "listentask.h"
#include "mypoll.h"
#include "myserver.h"
#define PROCESSMSGNUM 20
int CNetProcessThread::Init()
{
        #ifdef  __USE_EPOLL
        m_epoll = new CRealEpoll();  //epollģʽ
        #else
        m_epoll = new CMyPoll();      //pollģʽ
        #endif
        
        //��ʼ��epoll
        int retValue = m_epoll->Init();
        if (retValue != 0)
        {
                WriteRunInfo::WriteLog("Init epoll fail");
                return -1;
        }
        return 0;
}

int CNetProcessThread::PutMsgToInputList(CMsg *pMsg)
{
        m_inputList.put(pMsg);
        return 0;
}

int CNetProcessThread::GetMsgQueueLen()
{
	return m_inputList.size();
}

int CNetProcessThread::Run()
{
        while (m_nRunFlag)
        {
                //���ȼ��������¼�
                HandleProcess();
                int ii=0;
                while (ii < PROCESSMSGNUM) //ÿ������ó�20����Ϣ��������
                {
                    //������Ϣ
                    CMsg *pMsg =  m_inputList.get();
                    if (pMsg != NULL)
                    {
                            Process(pMsg);
                            delete pMsg;
                    }
                    else
                    {
                        break;
                    }
					ii++;
                }
                //��鳬ʱ������
                CheckTaskTimeOut();

                OtherProcess();
        }
        return 0;
}


int CNetProcessThread::OtherProcess()
{
        return 0;
}


int  CNetProcessThread::Process(CMsg *pMsg)
{
        TMsg *pTMsg = pMsg->GetTMsg();
        if (pTMsg->msgType == TIMERREACHTYPE)
        {
               TimerMsgProcess(pMsg);
        }    
        else  //��ʱֻ����ʱ����Ϣ
        {
                WriteRunInfo::WriteLog("In netprocessthread, recv invalid msg %d", pTMsg->msgType);
        }
        return 0;
}


int   CNetProcessThread::HandleProcess()
{      
        return m_epoll->EpollWait();
}

int  CNetProcessThread::CheckTaskTimeOut()
{
        int nowTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
        if (nowTime - m_nPreCheckTimeOut > CHECKTASKTIMEOUTINTERVAL || m_nPreCheckTimeOut > nowTime)
        {        
                int ret = 0;
                list<CSocketTask*>::iterator listItr = m_pSocketList.begin();
                for (; listItr != m_pSocketList.end(); )
                {
                    list<CSocketTask*>::iterator tmpItr = listItr;
                    listItr ++;
                    ret += (*tmpItr)->CheckTimeOut();
                }
                
                //map��Ҫɾ��
                map<ULONG64, CSocketTask*>::iterator mapItr = m_pMapSocketList.begin();
                for (; mapItr != m_pMapSocketList.end();)
                {
                    map<ULONG64, CSocketTask*>::iterator aaItr = mapItr;
                    mapItr++;
                    ret += (aaItr->second)->CheckTimeOut();
                }
                    
                WriteRunInfo::WriteInfoRun("The send&thread check task timeout, total %d task time out", ret);
                m_nPreCheckTimeOut = nowTime;
        }
        return 0;
}

CSocketPos* CNetProcessThread::AddSocketToList(CSocketTask *pSockTask)
{
	CSocketPos* pTmp = new CSocketPos();
	if (pSockTask->GetListType() == LINKEDLISTTYPE)
	{
	    m_pSocketList.push_back(pSockTask);
	    list<CSocketTask*>::iterator tmpItr = m_pSocketList.end();
	    tmpItr--;
		pTmp->SetListItr(tmpItr);
	}
	else
	{
		std::pair<map<ULONG64, CSocketTask*>::iterator, bool> tmpPair
			= m_pMapSocketList.insert(make_pair(pSockTask->GetTaskId(), pSockTask));
		
		if (tmpPair.second)
		{
			pTmp->SetMapItr(tmpPair.first);
		}
	}
    return pTmp;
}

int CNetProcessThread::DelSocketTask(CSocketTask *pSockTask)
{
	if (pSockTask->GetListType())
	{
    	m_pSocketList.erase(pSockTask->GetPos()->GetListItr());
	}
	else
	{	
		m_pMapSocketList.erase(pSockTask->GetPos()->GetMapItr());
	}
    return 0;
}


CSocketTask *CNetProcessThread::FindSocketTask(ULONG64 lTaskId)
{
	CSocketTask *pTask = NULL;
	map<ULONG64, CSocketTask*>::iterator mapItr = m_pMapSocketList.find(lTaskId);
	if (mapItr != m_pMapSocketList.end())
	{
		pTask = mapItr->second;
	}
	return pTask;
}
