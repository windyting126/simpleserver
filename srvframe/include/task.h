/*
 * FileName:       
 * Author:         wangqifeng  Version: 1.0  Date: 2005-8-22
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version >   <desc>
 */
#ifndef  __TASK_H
#define __TASK_H
#include "timer.h"
#include "commmsg.h"
#include "framecommon/framecommon.h"
using namespace MYFRAMECOMMON;


const int INITSTATUS=0;  //��ʼ��״̬
const int ENDSTATUS=1000;//����״̬


class CWorkThread;
/**
* dec:
* ClassName:CTask
* author: 
* version:
*CreateTime: 2005.8.22
*/
class CTask
{
public:
        /**
         *����ID,  ����������ڵ��߳�
         *  \param   taskId
         *  \param   *pThread
         *  \return   
         */
        CTask(ULONG64   taskId,  CWorkThread *pThread);
        
        /**
         *
         *  \return   virtual
         */
        virtual  ~CTask();
        
        /**
         *  ������Ϣ����
         *  \param   *msg
         *  \return   int
         */
        virtual int Process(CMsg *pMsg)=0;
        

        /**
         * ��ʱ������
         *  \param   &timeEvent
         *  \return   virtual int
         */
        virtual int OnTimer(TTimeEvent &timeEvent);

        virtual int CheckTimeOut();
                
        /**
         * ɾ�����ж�ʱ��
         *  \return   int
         */
        int DeleteAllTimer();
        
        /**
         *����һ����ʱ��
         *  \return   int
         */
        int AddOneTimer(int event, int timeLength);
        
        /**
         *ɾ��һ����ʱ��
         *  \return   int
         */
        int DeleteOneTimer(ULONG64 iTimerId);        

        /**
         *ֱ��ɾ��һ����ʱ��, ��֪ͨ��ʱ�߳�
         *  \return   int
         */
        int DirectDeleteOneTimer(ULONG64 iTimerId);        
        

        /**
         * ��ȡ����ID
         *  \return   int
         */
        int GetTaskId()
        {
                return m_iTaskId;
        }

        int GetTaskType()
        {
                return m_iTaskType;
        }

        void SetTaskType(int iTaskType)
        {
                m_iTaskType = iTaskType;
        }


        static ULONG64  GentTaskId();

        static int GetTaskNum()
        {
                return m_nSTaskNum;
        }
        //static int DeleteTask(ULONG64 lTaskId, CWorkThread *pThread);

protected:        
        int SendTimerOutMsg(int msgType,  TTimeEvent &timerStr);

        /**
         *ɾ�����������
         *  \return    int
         */
        virtual int DestroySelf()=0;

        void UpdateTaskNum(int nNum);
        
        ULONG64        m_iTaskId; //����ID                
        //ʱ����б�
        map<ULONG64, TTimeEvent>  m_timerList;
        //�����״̬
        int m_status;

        int m_iTaskType;
        CWorkThread *m_pThread; //�̶߳���

        int  m_nTaskTimeOut;
        int  m_nStartTime;

        static ULONG64     m_lCurTaskId;
        static CThreadMutex m_taskIdMutex;
        
        
        static int m_nSTaskNum;
        static CThreadMutex m_nTaskNumMutex;
};

#endif

