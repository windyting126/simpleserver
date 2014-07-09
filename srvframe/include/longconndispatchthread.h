/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconndispatchthread.h
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
#ifndef _LONGCONNDISPATCHTHREAD_H_ 
#define _LONGCONNDISPATCHTHREAD_H_ 
#include "synworkthread.h"

class CLongConnDispatchThread : public CSynWorkThread
{
public:
	CLongConnDispatchThread(int nEntityType, int nEntityId);
	virtual ~CLongConnDispatchThread();
public:
	virtual int Init();
protected:
	virtual int Process(CMsg* pMsg);
	int AddOneTimer(int nEvent, int nTimeLen);
protected:
	int ProcessConnectServerRes(CMsg* pMsg);	
	int ProcessDispatch();						
	static int m_nObjectNum;
	static CThreadMutex m_lockObjectNum;
};


#endif





