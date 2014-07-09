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
#ifndef __TIMER_H
#define __TIMER_H
#include <map>
#include <list>
#include <vector>
#include "framecommon/framecommon.h"
using namespace MYFRAMECOMMON;
const int MAXTIMERID=1000000000;

using namespace std;
class CTask;
const int TIMERCYCLE=3600;

struct TTimeEvent
{
        ULONG64  m_iTimerId;
        int  m_nTimerPoint;
        int  m_nEvent;
};

struct TTimerStr
{
        ULONG64        iTaskId;
        int iEntityType;
        int iEntityId;
        TTimeEvent event;        
};

/**
* dec:
* ClassName:CTimer
* author: 
* version:
*CreateTime: 2005.3.28
*/
class CTimer
{
public:
        CTimer();
        ~CTimer();
        /**
         * ����һ����ʱ�¼�
         *  \return   int
         */
        int AddOneTimer(TTimerStr &timer);            
        
        /**
         * ɾ��һ����ʱ�¼�
         *  \return   int
         */
        int DeleteOneTimer(int iTimePoint,  ULONG64 iTimerId);
        
        /**
         *  ��ʱ�¼����
         *  \return   void
         */
        int TimerCheck();

        
        /**
         *  ���õ�ǰ��ʱʱ���
         *  \param   timePoint
         *  \return   void
         */
        void SetCurTimePoint(int timeSec)
        {
                m_nCurTimePoint = timeSec%TIMERCYCLE;
        }

        int GetCurTimePoint()
        {
                return m_nCurTimePoint;
        }

        int SendTimerOutMsg(TTimerStr &timerStr);

        void PrintTimer();

        static  ULONG64 GentTimerId();
private:
        int   m_nCurTimePoint; //��ǰѭ����ʱ���
        int   m_nPreTimePoint; //�ϸ�ѭ����ʱ���
        vector <map<ULONG64, TTimerStr> >  m_timerList;       
        static ULONG64 m_iTimerId;
        static CThreadMutex m_sTimerIdMutex;
};

#endif
