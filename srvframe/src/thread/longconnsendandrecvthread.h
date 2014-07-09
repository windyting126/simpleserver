/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconnsendandrecvthread.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : Chen Hui (Buck), chenhui101@gmail.com
* Company		: 
* Date			: 2012-1-12
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
* -----------------------------------------------------------------------
*		Chen Hui		2012-1-12			1.0			
*		Wang Tiefeng	2012-02-17			1.0			Optimization
* =======================================================================
*/

#ifndef _LONGCONNSENDANDRECVTHREAD_H_
#define _LONGCONNSENDANDRECVTHREAD_H_

#include "netprocessthread.h"

class CLongConnSendAndRecvThread : public CNetProcessThread
{
public:	
	CLongConnSendAndRecvThread(int nEntityType, int nEntityId);
	virtual ~CLongConnSendAndRecvThread();
protected:
	virtual int Process(CMsg* pMsg);
	virtual int OtherProcess();
protected:
	int CheckConnectValidity();
protected:
	long long m_nLastCheckConnectTime;
};

#endif //_LONGCONNECTEDTHREAD_H_H_
