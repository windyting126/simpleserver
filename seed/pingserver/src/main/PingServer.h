/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingServer.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-04
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-04			1.0			
* =======================================================================
*/
#ifndef _PINGSERVER_H_30E28DB3_7095_4604_A6A0_CC1B387BAB8C_
#define _PINGSERVER_H_30E28DB3_7095_4604_A6A0_CC1B387BAB8C_

#include "myserver.h"
#include "threadgroup.h"
#include "maintainudpthread.h"
#include "PingServerConf.h"
#include "pingpeerhashtable.h"
#include <semaphore.h>
#include <sys/time.h>

class CPingServer : public CMyserver
{
public:
	CPingServer();
	~CPingServer();
public:
	virtual int InitServer( void );
	virtual int StartServer();
	void WriteSendListLog(int nLogType);

	CPingServerConf & GetConf()
	{
		return m_PingSrvConf;
	}
	CPingPeerHashTable * GetPeerHashTable()
	{
		return m_pPeerHashTable;
	}
	sem_t * GetSynSendSem()
	{
		return &m_semSynSend;
	}
private:
	int SendConnectMsg(const char *pcszRemoteIp, unsigned short usRemotePort, 
		unsigned char ucMachineType, unsigned short usMachineIndex);
private:
	CPingServerConf m_PingSrvConf;
private:
	CThreadGroup		*m_pTimerThreadGrp;
	CThreadGroup		*m_pMaintainThreadGrp;
	CMaintainUdpThread	*m_pMaintainUpdThread;
	CThreadGroup		*m_pUdpMSendThreadGrp;///< UDP multicast send thread group
	CThreadGroup		*m_pLongConnectThreadGrp;
	CThreadGroup		*m_pUdpRecvThreadGrp;
	CThreadGroup		*m_pUdpSendThreadGrp;
	CThreadGroup		*m_pOntimeSynThreadGrp;
	CThreadGroup		*m_pTimeoutCheckThreadGrp;
	CThreadGroup		*m_pConfigUpdateThreadGrp;///< pmgrsrv DB configure update thread group	
private:
	CPingPeerHashTable *m_pPeerHashTable;
	sem_t m_semSynSend;
};

#endif



