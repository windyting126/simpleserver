/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: TimeoutCheckThread.h
* Description	: 这个线程必须是单例
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
#ifndef _TIMEOUTCHECKTHREAD_H_61D06FCF_400F_4460_8D6D_156098741884_
#define _TIMEOUTCHECKTHREAD_H_61D06FCF_400F_4460_8D6D_156098741884_

#include "workthread.h"
#include "pingpeerhashtable.h"
#include "PingServer.h"

class CTimeoutCheckThread : public CWorkThread
{
public:
	CTimeoutCheckThread(int nEntityType,  int nEntityId);
	~CTimeoutCheckThread();
public:
	virtual int Init();
	virtual int Run();
	virtual int PutMsgToInputList(CMsg *pMsg);
private:
	virtual int Process(CMsg *pMsg);
	void TransMsg(char *pClientCommandPacketBuf, unsigned int uClientCommandPacketLen, int iServerIndex, int nServerType);
private:
	CPingPeerHashTable *m_pPeerHashTable;
	CPingServer *m_pServer;
	int m_nCheckInterval;
	int m_nSynInterval;
	int m_nTimeout;
	int m_nStatInterval;
};

#endif

