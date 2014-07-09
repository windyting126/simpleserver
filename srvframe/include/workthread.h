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
#ifndef __WORK_THREAD_h
#define __WORK_THREAD_h
#include "commonthread.h"
#include "commmsg.h"
#include <sys/time.h>
#include "task.h"
#include "framecommon/framecommon.h"
using namespace MYFRAMECOMMON;

const int THREADBUFLEN=200* 1024; //�̻߳���
/**
* dec:
* ClassName:CWorkThread
* author: 
* version:
*CreateTime: 2005.8.22
*/
class CWorkThread:public CCommonThread
{
public:
        CWorkThread(int iEntityType,  int iEntityId)
        {
                m_nEntityId = iEntityId;
                m_nEntityType = iEntityType;
                char *pTmpBuf = new char[THREADBUFLEN];
                m_threadBuf.AttachBuf(pTmpBuf, THREADBUFLEN);
        }
        
        virtual ~CWorkThread()
        {
        }

        
        /**
         *����ǹ����߳����е�������, ��ѭ�������������
         *  \return   int
         */
        virtual int Run()=0;
        virtual int PutMsgToInputList(CMsg *pMsg)=0;

		/**
		 *  ��ȡ��Ϣ���г��ȣ���Ҫ�����߳��Լ�ʵ��
		 *  \return message queue length
		 */
		virtual int GetMsgQueueLen()
		{
			return 0;
		}

        /**
         *�����̳߳�ʼ������
         *  \return   int
         */
        virtual int Init();
   
  
        /**
         *
         *  \param   *pMsg
         *  \return   int
         */

        /**
         * Ѱ������
         *  \param   iTaskId
         *  \return   CTask *
         */
        CTask * FindTask(ULONG64        iTaskId);

        /**
         *��ȡʵ��id
         *  \return   int
         */
        int GetEntityId()
        {
                return m_nEntityId;
        }

        int GetEntityType()
        {
                return m_nEntityType;
        }

    
        /**
         *
         *  \param   *pTask
         *  \return   int
         */
        int AddTask(CTask *pTask);

        /**
         *
         *  \param   taskId
         *  \return   int
         */
        int DelTask(ULONG64         taskId);


        CBufMgr &GetThreadBuf()
        {
                return m_threadBuf;
        }

        void SendMsg(int destEntityType, int destEntityId, ULONG64 lTaskId,  int msgType,  CMsgBody *pMsgBody, ULONG64 lSrcTaskId=0);
protected:

        int TimerMsgProcess(CMsg *pMsg);

        /**
         *  ��Ϣ��������һ���麯���� ������ʵ��ʵ��
         *  \param   *msg
         *  \return   int
         */
        virtual int   Process(CMsg *pMsg) = 0; 
		
        //��Ҫ�����ȴ������� 
        //���һ��ִ����ϵ�����ֱ�Ӿʹ������        
        map<ULONG64       ,  CTask*> m_taskList;
        
        int m_nEntityId;  //ʵ��ɣ�
        int m_nEntityType; //ʵ������

        //ÿ���߳���һЩ����
        CBufMgr m_threadBuf;        
};
#endif
