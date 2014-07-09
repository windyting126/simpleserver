/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconnrecvandsendthread.h
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

#ifndef _LONGCONNRECVANDSENDTHREAD_H_
#define _LONGCONNRECVANDSENDTHREAD_H_

#include "netprocessthread.h"

class CLongConnRecvAndSendThread : public CNetProcessThread
{
public:	
	CLongConnRecvAndSendThread(int nEntityType, int nEntityId);
	virtual ~CLongConnRecvAndSendThread();
protected:
	virtual int Process(CMsg* pMsg);
};

#endif //_LONGCONNECTEDTHREAD_H_H_
