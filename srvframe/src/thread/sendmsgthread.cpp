#include "sendmsgthread.h"
#include "commmsg.h"
#include "sendtask.h"
#include "mypoll.h"
#include "myserver.h"

CRecvAndSendMsgThread::CRecvAndSendMsgThread(int iEntityType, int iEntityId):CNetProcessThread(iEntityType, iEntityId)
{
}

CRecvAndSendMsgThread::~CRecvAndSendMsgThread()
{
}

int CRecvAndSendMsgThread::Process(CMsg * pMsg)
{
        TMsg *pTMsg = pMsg->GetTMsg();
        WRITE_TRACE("The recvsend thread recv a  msg , msgtype %d...", pTMsg->msgType);        
        
        if (pTMsg->srcEntityType== SERVICEPROENTITY)
        {
                if (pTMsg->msgType == SENDMSGTYPE)
                {
                        CSendMsgBody *pBody = (CSendMsgBody*)pMsg->GetMsgBody();
                        CSendTask *pSendTask = (CSendTask*)CMyserver::GetInstance()->GetClassFactory()->GenSocketTask(this,
                                pBody->GetTcpSocket(), SENDTASK);
                        if (pSendTask != NULL)
                        {
                        	    pSendTask->Init();
                                CSendMsg *pSendMsg = new CSendMsg(pBody->GetSendLen(), pBody->GetSendBuf());                                
                                pSendTask->PutSendMsg(pSendMsg);
                                pSendTask->PutEndMsg();
                                
                                pBody->SetTcpSocket(NULL); //��ɾ��ͨ�ž��
                                pBody->SetSendBuf(NULL);  //��ɾ��������
                        }
                }
                else
                {
                        WriteRunInfo::WriteLog("Send msg thread recv invalid msgtype %d", pTMsg->msgType);
                }
        }
        else if (pTMsg->srcEntityType == LISTENENTITY) //�յ���������
        {
                if (pTMsg->msgType == RECVMSGTYPE)
                {
                        CRecvMsgBody *pBody = (CRecvMsgBody*)pMsg->GetMsgBody();
                        CTcpSocket *pSocket = new CTcpSocket(pBody->GetSockAddr(), sizeof(sockaddr_in), pBody->GetSocket());
                        CSocketTask *tmpSocketTask = NULL;
                        tmpSocketTask = CMyserver::GetInstance()->GetClassFactory()->GenSocketTask(this,  
                                pSocket, RECVTASK);               
                        tmpSocketTask->Init();         
                        tmpSocketTask->SetTimeStat(pBody->GetTimeStat());
                }
                else
                {
                        WriteRunInfo::WriteLog("Send&recv  thread recv invalid msgtype %d", pTMsg->msgType);
                }
        }        
        else if (pTMsg->srcEntityType == TIMERENTITY)
        {
                TimerMsgProcess(pMsg);
        }    
        else
        {
                WriteRunInfo::WriteLog("Send msg thread recv invalid srcEntityType %d", pTMsg->srcEntityType);
        }
        return 0;
}

