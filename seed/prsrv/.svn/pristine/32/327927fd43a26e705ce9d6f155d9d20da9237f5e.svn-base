/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: pingsynrecvtask.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-10
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-10			1.0			
* =======================================================================
*/
#ifndef _PINGSYNTASK_H_AD6A10BC_CA88_4608_AB50_D30D920A216B_
#define _PINGSYNTASK_H_AD6A10BC_CA88_4608_AB50_D30D920A216B_

#include <vector>

#include "CommonTask.h"
#include "framecommon/framecommon.h"
#include "pingpeerhashtable.h"
#include "commmsg.h"

using namespace MYFRAMECOMMON;

const int HANDSHAKE_STATUS_NO  = 0;
const int HANDSHAKE_STATUS_YES = 1;

struct TTaskStatus
{
	unsigned char ucMachineType;
	unsigned short usMachineIndex;
	int iHandShakeState;				//SYN_RECVTASK_HANDSHAKE_NO : Œ¥Œ’ ÷ | SYN_RECVTASK_HANDSHAKE_YES“—Œ’ ÷
};

class CPingSynRecvTask : public CCommonTask
{
public:
	CPingSynRecvTask(CTcpSocket *pSocket, CNetProcessThread *pWorkThread, unsigned char ucMachineType, unsigned short usMachineIndex, 
		bool bActive, int iListType);
	~CPingSynRecvTask();

protected:
	//void SetHandshakeState(unsigned char ucMachineType, unsigned short usMachineIndex, int iHandShakeState);
	//int CheckHandshakeState(unsigned char ucMachineType, unsigned short usMachineIndex);
	void SetHandshakeState(int iHandShakeState);
	int CheckHandshakeState();

	virtual int ProcessPacket(PSERVERCOMMANDPACKET pServerCommandPacket);
	virtual int ProcessOnlinePacket(char *pBuf , int iLength);

	virtual int ProcessSynAffirm(char *pBuf , int iLength);
	virtual int ProcessHandShake(char *pBuf , int iLength);
	void SendLongConnectMsg(char *pClientCommandPacketBuf, unsigned int uClientCommandPacketLen);
protected:
	//static vector<TTaskStatus> m_vecTaskStatus;
	int m_nTaskStatus;
};


#endif


