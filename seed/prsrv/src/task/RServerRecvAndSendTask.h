/*
* =======================================================================
* Copyright (c) 2011-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: RServerRecvAndSendTask.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : zhang zhou
* Company		: 
* Date			: 2009-2-12
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		zhang zhou		2009-02-12			1.0
*		wangtiefeng		2011-03-25			2.0			
* =======================================================================
*/
#ifndef _RSERVER_RECV_AND_SEND_TASK_H_
#define _RSERVER_RECV_AND_SEND_TASK_H_
#include "pingsynrecvtask.h"
#include "netprocessthread.h"

class RServerRecvAndSendTask : public CPingSynRecvTask
{
public:
	RServerRecvAndSendTask(CTcpSocket *pSocket, CNetProcessThread *pWorkThread,  
		unsigned char ucMachineType, unsigned short usMachineIndex, 
		bool bActive, int iListType, int iUploadInterval);
	virtual ~RServerRecvAndSendTask();

protected:
	virtual int DestroySelf();
	virtual int ProcessPacket(PSERVERCOMMANDPACKET pServerCommandPacket);
private:
	int	m_iUploadInterval;
	char* m_pReturnBuf;
};


#endif

