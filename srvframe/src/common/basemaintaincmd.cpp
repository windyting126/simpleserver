/*
 * FileName:       
 * Author:         wangqifeng  Version: 1.0  Date: 2007-1-5
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version >   <desc>
 */
#include "basemaintaincmd.h"
#include "commmsg.h"
#include "task.h"
#include "myserver.h"
CBaseMaintainCmd::CBaseMaintainCmd(int nMsgType, int nBodyLen, char *pBody)
{ 
        if (nBodyLen > 0 && pBody != NULL)
        {
                char *pBuf = new char[nBodyLen];
                memcpy(pBuf, pBody, nBodyLen);
                m_sRecvBuf.AttachBuf(pBuf, nBodyLen);
        }
        m_nMsgType = nMsgType;
}
 
CBaseMaintainCmd::~CBaseMaintainCmd()
{
}

int CBaseMaintainCmd::Process()
{
        switch(m_nMsgType)
        {                       
                case CMD_DEBUGINFO: //������Ϣ
                {                                           
                       //�Ȱ�����ͻ��˹رյ�                       
                       //ֱ������
                       if ( (size_t)m_sRecvBuf.GetBufLen() < sizeof(CmdModLogInfo))
                       {
                            break;
                       }
                       
                       CmdModLogInfo *pCmdModLogInfo = (CmdModLogInfo*)m_sRecvBuf.GetBuf();
                       pCmdModLogInfo->iFlag = ntohl(pCmdModLogInfo->iFlag);
                       pCmdModLogInfo->iModLogType = ntohl(pCmdModLogInfo->iModLogType);
                       WriteRunInfo::WriteLog("Recv debufInfo logmod %d flag %d",pCmdModLogInfo->iModLogType,
                                pCmdModLogInfo->iFlag);
                       //���ô�ӡ�Ŀ��� 
                       WriteRunInfo::SetLogFlag(pCmdModLogInfo->iModLogType, 
                                pCmdModLogInfo->iFlag);
                       break;         
                }              
                case PRINTSTATINFO:  //��ӡͳ����Ϣ
                {
                        WriteRunInfo::WriteStat("Now task num is %d", CTask::GetTaskNum());
                        //��ӡͳ����Ϣ
                        CMyserver::GetInstance()->GetCommonStat().PrintStat();
                        //SendMsg(TIMERENTITY, 0, 0, PRINTTIMER, NULL, 0);
                        break;                                
                }
                default:
                {                      
                        WriteRunInfo::WriteLog("In maintain thread, the msgtype %d is invalid", m_nMsgType);                       
                        break;
                }
        }          
        return 0;
}

