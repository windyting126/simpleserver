/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconnrecvandsendtask.h
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

#ifndef _LONGCONNRECVANDSENDTASK_H_
#define _LONGCONNRECVANDSENDTASK_H_

#include <vector>
#include <list>

#include "framecommon/framecommon.h"
#include "recvandsendtask.h"
#include "netprocessthread.h"
#include "longconndef.h"

using namespace MYFRAMECOMMON;

class CLongConnRecvAndSendTask : public CRecvAndSendTask
{
public:
	CLongConnRecvAndSendTask(CTcpSocket* pSocket, CNetProcessThread* pWorkThread);
	virtual ~CLongConnRecvAndSendTask();
public:
	virtual int DestroySelf();
	virtual int Init();
	virtual int PutMsgToSendList(const char* pBuf, int nLen);
	virtual int HandleProcess(int nEvent);
protected:
	virtual int RecvProcess();
	virtual int SendProcess();
	virtual int ProcessPacket();
	virtual int ProcessLongConnMsg(char* pBuf, int nBufLen, TFrameLongConnectHead& FrameLongConnectHead);
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
};

#endif //_COMMONTASK_H_H_

















