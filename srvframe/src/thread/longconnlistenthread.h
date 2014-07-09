/*
* =======================================================================
* Copyright (c) 2012-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconnistenthread.h
* Description	: this thread must be used as a single thread
* -----------------------------------------------------------------------
* Author        : Wang Tiefeng (), wangtiefeng@baofeng.com
* Company		: 
* Date			: 2012-2-14
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
* -----------------------------------------------------------------------
*		Wang Tiefeng	2012-02-14			1.0			
* =======================================================================
*/
#ifndef _LONGCONNLISTENTHREAD_H_
#define _LONGCONNLISTENTHREAD_H_

#include "netprocessthread.h"

class CLongConnListenThread : public CNetProcessThread
{
public:
	CLongConnListenThread(int iEntityType, int iEntityId, int nHost, short nPort, int nListenTaskType = LONGCONNLISTENTASK);
	virtual ~CLongConnListenThread();
public:
	virtual int Init();
protected:
	int m_nHost;
	short m_nPort;
	int m_nListenTaskType;
};
#endif //_LONGCONNECTEDLISTENTHREAD_H_H_
