/*
 * FileName:       
 * Author:         wangqifeng  Version: 1.0  Date: 2005-8-26
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version >   <desc>
 */
#include <string.h>
#include "errno.h" 
#include "commonthread.h"


CCommonThread::CCommonThread()
{
        m_nRunFlag = 1;
        m_nThreadId = 0;
        m_nPid = 0;
}


CCommonThread::~CCommonThread()
{
}


int CCommonThread::Start()
{
         int ret = pthread_create(&m_nThreadId, NULL, ThreadEntry, (void*)this);
         if (ret != 0)
         {
                WriteRunInfo::WriteLog("Create thread fail, error is %d.\n", strerror(errno));
                return -1;
         }         
         return 0;
}

void CCommonThread::Stop()
{
        m_nRunFlag = 0;      
}

pthread_t CCommonThread::GetThreadId()
{
        return m_nThreadId;
}

pid_t CCommonThread::GetPid()
{
        return m_nPid;
}

void CCommonThread::SetPid(pid_t pid)
{
        m_nPid = pid;
}


void *CCommonThread::ThreadEntry(void * arg)
{
        CCommonThread *commonThread = (CCommonThread*)arg;
        int tmpRet = 0;
        tmpRet =  commonThread->Init();
        if (tmpRet != 0)
        {
                //д��־�� ��ʾ�߳�����ʧ��
                WriteRunInfo::WriteLog("The thread init fail, exit program");
                exit(0);
        }
        commonThread->SetPid(getpid());
        commonThread->Run();
        return NULL;
}

