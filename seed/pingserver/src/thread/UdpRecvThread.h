/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: UdpRecvThread.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-03
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-03			1.0			
* =======================================================================
*/
#ifndef _UDPRECVTHREAD_H_4D1DBACC_CE05_41aa_A574_C7689E3D6C24_
#define _UDPRECVTHREAD_H_4D1DBACC_CE05_41aa_A574_C7689E3D6C24_

#include "udpcommonrecvthread.h"

class CUdpRecvThread : public CUdpCommonRecvThread
{
public:
	CUdpRecvThread(int iEntityType,  int iEntityId, string &strServerIp, unsigned short usPort);
public:
	virtual int Init();
	virtual int Run();
};

#endif


