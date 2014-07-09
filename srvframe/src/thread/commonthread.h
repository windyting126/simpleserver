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
#ifndef __COMMON_THREAD_H
#define __COMMON_THREAD_H
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include "framecommon/framecommon.h"
using namespace MYFRAMECOMMON;
const int MAXIDLESLEEPTIME=20000;


/**
* dec:
* ClassName:CCommonThread
* author: 
* version:
*CreateTime: 2005.8.22
*/
class CCommonThread
{
public:
        CCommonThread();
        virtual ~CCommonThread();
        /**
         *  �߳�����
         *  \return   int
         */
        int Start();

        void Stop();
        /**
         * �߳����к���
         *  \return   int
         */
       virtual int Run()=0;

       virtual int Init()=0;

       int GetRunFlag()
       {
               return m_nRunFlag;
       }

       pthread_t GetThreadId();        
       pid_t GetPid();
       void SetPid(pid_t pid);
protected:
       static void *ThreadEntry(void *arg);
       pthread_t  m_nThreadId; //�߳�id
       int m_nRunFlag; //0ֹͣ1���� 
       pid_t m_nPid;
};
#endif
