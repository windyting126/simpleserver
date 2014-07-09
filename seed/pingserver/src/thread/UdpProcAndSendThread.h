/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: UdpProcAndSendThread.h
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
#ifndef _UDPPROCANDSENDTHREAD_H_AE389087_667D_4aab_9D80_AE88949E4C45_
#define _UDPPROCANDSENDTHREAD_H_AE389087_667D_4aab_9D80_AE88949E4C45_

#include "udpcommonsendthread.h"

class CUdpProcAndSendThread : public CUdpCommonSendThread
{
public:
	CUdpProcAndSendThread(int iEntityType,  int iEntityId);
	~CUdpProcAndSendThread(){}
public:
	virtual int MsgProcess(CMsg * pMsg);
	virtual int Run();
};

#endif
