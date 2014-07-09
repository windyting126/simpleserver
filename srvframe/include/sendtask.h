/*
 * FileName:       
 * Author:         wangqifeng  Version: 1.0  Date: 2005-12-8
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version >   <desc>
 */
#ifndef __SEND_TASK_H
#define __SEND_TASK_H
#include "commserver.h"




/**
* dec: �����᲻ͣ����ͻ���push����, ֱ�����Ͷ�������һ����ֹ��Ϣ, ��ֹ��Ϣ����Ϣ��Ϊ��, ����Ϊ0
* ClassName:CSendTask
* author: 
* version:
*CreateTime: 2005.12.8
*/
class CSendTask:public CSocketTask
{
public:
        CSendTask(CTcpSocket *pSocket, CNetProcessThread *pWorkThread, int nListType=0);
       
        virtual ~CSendTask();

	virtual int Init();
         /**
         *  ������Ϣ����
         *  \param   *msg
         *  \return   int
         */
        int Process(CMsg *pMsg);
        
        /**
         * �������
         *  \return   virtual int
         */
        int HandleProcess(int events);

        /**
         * ��ʱ������
         *  \param   &timeEvent
         *  \return   virtual int
         */
//        int OnTimer(TTimeEvent &timeEvent);

        int DestroySelf(); 

        
        void PutSendMsg(CSendMsg *pSendMsg)
        {
                m_pSendMsgList.push_back(pSendMsg);
        }        
        void PutEndMsg();
        
        virtual void Routine(); //���д���
protected:   
        CSendMsg* GetSendMsg();
        
        
        int AddToEpoll();         
        list<CSendMsg*>  m_pSendMsgList;
        CSendMsg *m_pSendMsg;
        int m_iCurSendLen;
        int SendData();
};


#endif

