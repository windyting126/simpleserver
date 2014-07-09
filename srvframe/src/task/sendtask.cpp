/*
 * FileName:       
 * Author:         wangqifeng  Version: 1.0  Date: 2005-8-31
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version >   <desc>
 */
#include "sendtask.h"
#include "netprocessthread.h"
#include "myserver.h"

CSendTask::CSendTask(CTcpSocket *pSocket, CNetProcessThread *pWorkThread, int nListType):
                CSocketTask(pSocket, pWorkThread, nListType)
{
        m_iCurSendLen=0;
        m_iTaskType = SENDTASK;
        m_pSendMsg = NULL;
        AddToEpoll();
}

CSendTask::~CSendTask()
{                      
        //WRITE_TRACE("delete a send task %llu", m_iTaskId);
        if (m_pSendMsg != NULL)
                delete m_pSendMsg;
        list<CSendMsg*>::iterator listItr = m_pSendMsgList.begin();
        for (; listItr != m_pSendMsgList.end();)
        {
                list<CSendMsg*>::iterator tmpItr = listItr;        
                listItr++;
                delete *tmpItr;
                m_pSendMsgList.erase(tmpItr);
        }        
}

int CSendTask::Init()
{
	AddToEpoll();
	return 0;
}

int CSendTask::Process(CMsg *pMsg)
{
        return 0;
}


int CSendTask::HandleProcess(int events)
{        
//        DelFromEpoll();        
        if ((events&MYPOLLERR) == MYPOLLERR || (events&MYPOLLHUP) == MYPOLLHUP)
        {
                CMyserver::GetInstance()->GetCommonStat().AddNormalStat(SOCKETERRORTYPE);
                WriteRunInfo::WriteLog("Receive abnormity event %d", events);
                DestroySelf();
                return 0;
        }  
        int ret = 0;
        //ÿ�����д10K
        if (events &(MYPOLLOUT|MYPOLLRDNORM)) //��д�¼�
        {               
                ret = SendData();               
        }
        else //�����¼�
        {
                DestroySelf();
                return 0;
        }
        
        if (ret == -1 || ret == 1) //�ͻ����Ѿ��ر�, �����Ǿ������, �����Ѿ��������
        {
                DestroySelf();
                return 0;
        }
        //���������û�����ʱ����ܹ��ӽ�ȥ
//        AddToEpoll();
        return 0;
}

//int CSendTask::OnTimer(TTimeEvent &timeEvent)
//{
//        CTask::OnTimer(timeEvent);
//        if (timeEvent.m_nEvent == SENDDATATIMEOUTEVENT)
//        {
//                //д��־
//                DestroySelf(); //�����ɾ��
//                return 0;
//        }
//        else 
//        {
//                //д��־������Щ��ʱ�¼�
//                WriteRunInfo::WriteLog("Error time out event %d", timeEvent);
//                DestroySelf();                
//        }        
//        return 0;
//}

int CSendTask::DestroySelf()
{      
        delete this;
        return 0;
}


int CSendTask::SendData()
{
        if (m_pSendMsg == NULL)
        {
                m_pSendMsg = GetSendMsg();
                if (m_pSendMsg == NULL)
                {
                        WRITE_TRACE("Get send msg , but no msg");
                        return 0;
                }
                m_iCurSendLen = 0;
        }

        if (m_pSendMsg->GetBody() == NULL)
        {
                WRITE_TRACE("The sendmsg body is NULL, exit the sendtask .....");
                return 1;
        }
 
        
        int retLen = 0;
        int retValue = 0;
        int sendLen = (SENDMAXLEN >  (m_pSendMsg->GetLen()- m_iCurSendLen))?
                 (m_pSendMsg->GetLen()- m_iCurSendLen):SENDMAXLEN;
        retLen = send( m_pTcpSocket->GetSocket(), m_pSendMsg->GetBody()+ m_iCurSendLen, 
                sendLen, MSG_DONTWAIT);
        if (retLen == sendLen) //ȫ�����ͳ�ȥ
        {
                delete m_pSendMsg; //�������,����Ϣɾ����
                m_pSendMsg = NULL;
                m_iCurSendLen = 0;
                retValue = 0;         
                //��ȡһ����Ϣ����
                m_pSendMsg = GetSendMsg();
                if (m_pSendMsg != NULL)
                {
                        if (m_pSendMsg->GetBody() == NULL)
                        {
                                retValue = 1; //������
                        }
                }                
        }
        else if (retLen > 0 && retLen < sendLen) //������һ����
        {
                m_iCurSendLen += retLen;
                retValue = 0;
        }
        else //<= 0
        {
                if (errno == ECONNRESET||
                errno == EDESTADDRREQ||
                errno == ENOTCONN||
                errno == EPIPE||
                errno == ETIMEDOUT || errno == EMSGSIZE)
                {
                        retValue = -1;
                }
                //д��־
                WRITE_TRACE("Send data len invalid %d", retLen);                        
        }
        return retValue;
}

int CSendTask::AddToEpoll()
{          
        int tmpEvent = MYPOLLERR|MYPOLLHUP|MYPOLLET|MYPOLLOUT|MYPOLLRDNORM;                         
        CNetProcessThread *pNetThread = (CNetProcessThread*)m_pThread;
        CEpoll *pEoll = pNetThread->GetEpoll();
        pEoll->AddToEpoll(this, tmpEvent);
        return 0;
}

CSendMsg* CSendTask::GetSendMsg()
{
        CSendMsg *pSendMsg = NULL;
        if (m_pSendMsgList.size() == 0)
        {
                return pSendMsg;
        }
        
        list<CSendMsg*>::iterator listItr =  m_pSendMsgList.begin();
        pSendMsg = *listItr;
        //�������ȥ��
        m_pSendMsgList.erase(listItr);
        return pSendMsg;
}

void CSendTask::PutEndMsg()
{
        CSendMsg *pSendMsg = new CSendMsg(0, NULL);
        PutSendMsg(pSendMsg);        
}


void CSendTask::Routine()
{
        return;
}

