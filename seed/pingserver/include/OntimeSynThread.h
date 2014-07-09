/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: OntimeSynThread.h
* Description	: 这个线程必须是单例
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
#ifndef _ONTIMESYNTHREAD_H_74E840E8_E248_4f0a_8EB3_23EF1640213D_
#define _ONTIMESYNTHREAD_H_74E840E8_E248_4f0a_8EB3_23EF1640213D_

#include "workthread.h"
#include "synqueue.h"
#include "PingServer.h"

class COntimeSynThread : public CWorkThread
{
public:
	COntimeSynThread(int nEntityType, int nEntityId);
	~COntimeSynThread();
public:
	virtual int Init();
	virtual int Run();
	virtual int PutMsgToInputList(CMsg *pMsg);
	virtual int GetMsgQueueLen()
	{
		return m_InputList.size();
	}
private:
	virtual int Process(CMsg *pMsg);
	int ProcessOntime(CMsg *pMsg);
	int ProcessResyn(CMsg *pMsg);
	int ProcessSynAffirm(CMsg *pMsg);
	int SendHandShake(char nServerType, short nServerIndex);
	int SendSynAffirm(char nServerType, short nServerIndex);
	int SynToServer(char nServerType = -1, short nServerIndex = -1);
	void TransMsg(char *pClientCommandPacketBuf, unsigned int uClientCommandPacketLen, int iServerIndex, int nServerType);
private:
	CQueue m_InputList;
	CPingServer *m_pServer;
	CPingPeerHashTable *m_pPeerHashTable;
	vector<bool> m_vecQConnectFirstTime;
	vector<bool> m_vecRConnectFirstTime;
	int m_nQServerNum;
	int m_nRServerNum;
	sem_t *m_pSynSendSem;
	int m_nSynWait;
	int m_nSynBufLen;
};

#endif


