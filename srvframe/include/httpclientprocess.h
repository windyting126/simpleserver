/*
 * =====================================================================================
 *
 *       Filename:  httpclientprocess.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年04月23日 19时13分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dijie
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef __HTTP_CLIENT_PROCESS_H
#define __HTTP_CLIENT_PROCESS_H

#include "netprocessthread.h"
#include "recvandsendtask.h"

class CHttpClientProcess
{
public:
	CHttpClientProcess(CNetProcessThread* pThread);
	virtual ~CHttpClientProcess();

public:
	virtual int ProcessResponseHttpHead();
	virtual int ProcessRecvBodyData(const char* pBuf, int nlen);
	virtual int EndRecv();
	void SetTask(CRecvAndSendTask* pTask)
	{
		m_pTask = pTask;
	}
	virtual int InitTask();

protected:
	CNetProcessThread* m_pThread;
	CRecvAndSendTask* m_pTask;

};


#endif


