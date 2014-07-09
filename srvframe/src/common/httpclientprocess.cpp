/*
 * =====================================================================================
 *
 *       Filename:  httpclientprocess.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年04月23日 19时13分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dijie
 *        Company:  
 *
 * =====================================================================================
 */

#include "httpclientprocess.h"

CHttpClientProcess::CHttpClientProcess(CNetProcessThread* pThread)
	: m_pThread(pThread), m_pTask(NULL)
{
}

CHttpClientProcess::~CHttpClientProcess()
{
}

int CHttpClientProcess::ProcessResponseHttpHead()
{
	return 0;
}

int CHttpClientProcess::ProcessRecvBodyData(const char* pBuf, int nlen)
{
	return 0;
}

int CHttpClientProcess::EndRecv()
{
	return 0;
}

int CHttpClientProcess::InitTask()
{
	return 0;
}



