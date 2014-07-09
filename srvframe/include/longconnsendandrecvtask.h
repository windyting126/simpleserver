/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconnsendandrecvtask.h
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

#ifndef _LONGCONNSENDANDRECVTASK_H_
#define _LONGCONNSENDANDRECVTASK_H_

#include <vector>
#include <list>

#include "framecommon/framecommon.h"
#include "recvandsendtask.h"
#include "netprocessthread.h"
#include "longconndef.h"

using namespace MYFRAMECOMMON;

const int LONGCONNTASK_TIMEOUT = 5;

class CLongConnSendAndRecvTask : public CRecvAndSendTask
{
public:
	CLongConnSendAndRecvTask(CTcpSocket* pSocket, CNetProcessThread* pWorkThread, int nTaskTimeout = LONGCONNTASK_TIMEOUT);
	virtual ~CLongConnSendAndRecvTask();
public:
	virtual int DestroySelf();
	virtual int Init();
	virtual int PutMsgToSendList(const char* pBuf, int nLen);
	virtual int HandleProcess(int nEvent);
	virtual int CheckTimeOut();
public:
	int ConnectServer(const TLongConnectOption& ConnectOption);
	virtual int SendConnectState(int nResult);
protected:
	virtual int RecvProcess();
	virtual int SendProcess();
	virtual int ProcessPacket();
protected:
	virtual int AddToEpoll();
	int ModifyEpollAddWrite();
	int ModifyEpollDelWrite();
	int WriteLogConnectState(int nConnectResult);

	int PacketFinished();
	void ResetRecvBuffer(unsigned int uReceivePos, unsigned int uReceiveSize);
protected:
	enum State
	{
		ReadPacketHead = 0,
		ReadPacket = 1
	};
protected:
	unsigned int m_uReceivePos;	
	unsigned int m_uReceiveSize;
	State m_state;
	std::vector<char>  m_vReceiveBuffer;
protected:
	bool m_bIfConnectSuc;
	TLongConnectOption m_ConnectOption;	
	int m_nLastReadTime;
	int m_nLastWriteTime;	
	int m_nTcpSocketTimeout;
};

#endif //_COMMONTASK_H_H_

















