/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: pingsynrecvtask.cpp
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
#include <assert.h>
#include "pingsynrecvtask.h"
#include "pinglongconnectcmd.h"
#include "statistic.h"
#include "rsrvdef.h"

CPingSynRecvTask::CPingSynRecvTask(CTcpSocket *pSocket, CNetProcessThread *pWorkThread,
								   unsigned char ucMachineType, unsigned short usMachineIndex,
								   bool bActive, int iListType)
								   : CCommonTask(pSocket, pWorkThread, ucMachineType, usMachineIndex, bActive, iListType)
{
	m_nTaskStatus = HANDSHAKE_STATUS_NO;
}

CPingSynRecvTask::~CPingSynRecvTask()
{

}

//void CPingSynRecvTask::SetHandshakeState(unsigned char ucMachineType, unsigned short usMachineIndex, int iHandShakeState)
//{
//	vector<TTaskStatus>::iterator iterTmp = m_vecTaskStatus.begin();
//	while(iterTmp != m_vecTaskStatus.end())
//	{
//		if(ucMachineType == iterTmp->ucMachineType && usMachineIndex == iterTmp->usMachineIndex)
//		{
//			iterTmp->iHandShakeState = iHandShakeState;
//			break;
//		}
//		iterTmp++;
//	}
//	if(iterTmp == m_vecTaskStatus.end())
//	{
//		TTaskHandShakeUnit TaskHandShakeUnit;
//		TaskHandShakeUnit.ucMachineType = ucMachineType;
//		TaskHandShakeUnit.usMachineIndex = usMachineIndex;
//		TaskHandShakeUnit.iHandShakeState = iHandShakeState;
//		m_vecTaskStatus.push_back(TaskHandShakeUnit);
//	}
//}

//int CPingSynRecvTask::CheckHandshakeState(unsigned char ucMachineType, unsigned short usMachineIndex)
//{
//	vector<TTaskStatus>::iterator iterTmp = m_vecTaskStatus.begin();
//	while(iterTmp != m_vecTaskStatus.end())
//	{
//		if(ucMachineType == iterTmp->ucMachineType && usMachineIndex == iterTmp->usMachineIndex)
//			break;
//		iterTmp++;
//	}
//	if (iterTmp != m_vecTaskStatus.end())
//	{
//		if(iterTmp->iHandShakeState == SYN_RECVTASK_HANDSHAKE_YES)
//			return 1;
//		else return 0;
//	}
//	else return 0;
//}

void CPingSynRecvTask::SetHandshakeState(int iHandShakeState)
{
	m_nTaskStatus = iHandShakeState;
}

int CPingSynRecvTask::CheckHandshakeState()
{
	return (m_nTaskStatus == HANDSHAKE_STATUS_YES);
}

int CPingSynRecvTask::ProcessPacket(PSERVERCOMMANDPACKET pServerCommandPacket)
{
	if ( !pServerCommandPacket )
	{
		return -1;
	}
	if( ntohs(pServerCommandPacket->usMsgId) == ONLINE_SYN )
	{
		ProcessOnlinePacket(pServerCommandPacket->serverCommandPacketContent, ntohl(pServerCommandPacket->iMsgLen));	
	}
	return 0;
}

int CPingSynRecvTask::ProcessOnlinePacket(char *pBuf , int iLength)
{
	if (!pBuf || iLength < SYN_MSG_HEAD_LEN)
	{
		return -1;
	}
	unsigned char nCmdType = *(unsigned char*)(pBuf + SYN_CMDINFO_OFF);
	CCmdCounter* pCmdHandler = CCmdCounter::GetHandler();
	pCmdHandler->AddCount(nCmdType);
	WriteRunInfo::WriteTrace("Recv PingServer CmdId:[%d]", nCmdType);
	switch (nCmdType)
	{
	case MSG_LONGCONNECT_SYN_PING:
	case MSG_LONGCONNECT_SYN_LOGOUT:
	case MSG_LONGCONNECT_SYN_ONTIME:
		{
			char *pMsg = new char[iLength];
			memcpy(pMsg, pBuf, iLength);
			CMsgBody *pMsgBody = new CBufMsgBody(pMsg, iLength);
			m_pThread->SendMsg(PING_SYN_PROCESS_ENTITY_TYPE, RANDOMENTITYID, 0, PING_SYN_PROCESS_MSG_TYPE, pMsgBody);
			break;
		}
	case MSG_LONGCONNECT_SYN_HANDSHAKE:
		{
			ProcessHandShake(pBuf, iLength);
			break;
		}
	case MSG_LONGCONNECT_SYN_AFFIRM:
		{
			ProcessSynAffirm(pBuf, iLength);
			break;
		}
	default:
		{
			WriteRunInfo::WriteError( "Ping Syn Recv Message type is wrong!" );
			break;
		}
	}
	return 0;		
}

int CPingSynRecvTask::ProcessHandShake(char *pBuf , int iLength)
{
	if (!pBuf || iLength < SYN_MSG_HEAD_LEN)
	{
		return -1;
	}
	THandShakeCmd HandShakeCmd;
	CSynDecodeCommand<THandShakeCmd> DecodeCmd(pBuf, iLength, HandShakeCmd);
	if (DecodeCmd.Decode() != 0)
	{
		return -1;
	}
	SetHandshakeState(HANDSHAKE_STATUS_YES);
	WRITE_TRACE("ProcessHandShake Succceed");
	return 0;
}

int CPingSynRecvTask::ProcessSynAffirm(char *pBuf , int iLength)
{
	if (!pBuf || iLength < SYN_MSG_HEAD_LEN)
	{
		return -1;
	}
	TSynAffirmCmd SynAffirmCmd;
	CSynDecodeCommand<TSynAffirmCmd> DecodeCmd(pBuf, iLength, SynAffirmCmd);
	if (DecodeCmd.Decode() != 0)
	{
		return -1;
	}
	if (CheckHandshakeState())
	{
		return 0;
	}
	int nBufLen =  SynAffirmCmd.GetSize() + TSynBaseCommand::GetSize();
	char *pEncodeBuf = new char[nBufLen];
	CBufMgr tmpBufMgr(pEncodeBuf, nBufLen);
	CSynEncodeCommand<TSynAffirmCmd> EncodeCmd(pEncodeBuf, nBufLen, SynAffirmCmd, MSG_LONGCONNECT_SYN_AFFIRMRES);
	if (EncodeCmd.Encode() == 0)
	{
		SendLongConnectMsg(EncodeCmd.GetPacket(), EncodeCmd.GetPacketLen());
		WRITE_TRACE("ProcessSynAffirm : Send Syn Affirm Res");
	}
	WRITE_TRACE("ProcessSynAffirm Succceed");
	return 0;
}

void CPingSynRecvTask::SendLongConnectMsg(char *pClientCommandPacketBuf, unsigned int uClientCommandPacketLen)
{
	CTmpBufMgr GenPacketHelper;
	int nTmpLen = sizeof(SERVERINNERCOMMANDPACKET) + uClientCommandPacketLen;
	char *pTmpBuf;
	pTmpBuf = new char[nTmpLen];
	GenPacketHelper.AttachBuf(pTmpBuf, nTmpLen);

	SERVERINNERCOMMANDPACKET serverInnerCommandPacket;
	serverInnerCommandPacket.ucDestMachineType = 0;
	serverInnerCommandPacket.usDestMachineIndex = 0;

	serverInnerCommandPacket.serverCommandPacket.usMsgId = (unsigned short)htons(ONLINE_SYN);
	serverInnerCommandPacket.serverCommandPacket.iMsgLen = (int)htonl(uClientCommandPacketLen);
	GenPacketHelper.Put(serverInnerCommandPacket);

	GenPacketHelper.PutCharBuf(pClientCommandPacketBuf, uClientCommandPacketLen);
	
	PutMsgToSendList(GenPacketHelper.GetBuf(), GenPacketHelper.GetRealBufLen());
}

