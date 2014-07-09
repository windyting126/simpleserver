/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingCmd.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-05
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-05			1.0			
* =======================================================================
*/
#include "PingCmd.h"
#include "pinglongconnectcmd.h"
#include "CmdCounter.h"


CPingCmd::CPingCmd(char *pBuf, int nLen, int nPublicIp, short nPublicPort, int nEntityType, int nEntityId)
: CPingSrvBaseCommand<TPingCmd>(pBuf, nLen , nPublicIp, nPublicPort, nEntityType, nEntityId)
{
	m_nIfRes = 0;
}

int CPingCmd::ProcessCmd()
{
	TPingPeerInfo PeerInfo;
	PeerInfo.strPeerId = m_in.strPeerId;
	PeerInfo.nInternalIp = m_in.nInternalIp;
//	PeerInfo.nSubMask = m_in.nSubMask;
	PeerInfo.nTcpPort = m_in.nTcpPort;
// 	PeerInfo.nProductFlag = m_in.nProductFlag;
// 	PeerInfo.nProductReleaseId = m_in.nProductReleaseId;
// 	PeerInfo.vecSuperNode = m_in.vecSuperNode;
// 	PeerInfo.nNatType = m_in.nNatType;
	PeerInfo.nUpnpMapdeIp = m_in.nUpnpMapdeIp;
	PeerInfo.nUpnpMapedPort = m_in.nUpnpMapedPort;
// 	PeerInfo.nOnlineTime = m_in.nOnlineTime;
// 	PeerInfo.nDownloadBytes = m_in.nDownloadBytes;
// 	PeerInfo.nUploadBytes = m_in.nUploadBytes;
// 	PeerInfo.nCurUploadResource = m_in.nCurUploadResource;
// 	PeerInfo.nCurUploadTaskNum = m_in.nCurUploadTaskNum;
// 	PeerInfo.nCurDownloadTaskNum = m_in.nCurDownloadTaskNum;
 	PeerInfo.nCurUploadPipeNum = m_in.nCurUploadPipeNum;
// 	PeerInfo.nCurDownloadSpeed = m_in.nCurDownloadSpeed;
// 	PeerInfo.nCurUploadSpeed = m_in.nCurUploadSpeed;
	PeerInfo.nMaxDownloadSpeed = m_in.nMaxDownloadSpeed;
	PeerInfo.nMaxUploadSpeed = m_in.nMaxUploadSpeed;
// 	PeerInfo.nSmartUploadSpeedLimit = m_in.nSmartUploadSpeedLimit;
// 	PeerInfo.nUserUploadSpeedLimit = m_in.nUserUploadSpeedLimit;
// 	PeerInfo.nDownloadSpeedLimit = m_in.nDownloadSpeedLimit;
// 	PeerInfo.nPeerStatus = m_in.nPeerStatus;
	PeerInfo.nLocalBindPort = m_in.nLocalBindPort;
// 	PeerInfo.nExternalNeighbor = m_in.nExternalNeighbor;
// 	PeerInfo.nInternalNeighbor = m_in.nInternalNeighbor;
// 	PeerInfo.strPartnerPeerId = m_in.strPartnerPeerId;

	PeerInfo.nExternalIp = m_nClientIp;
	PeerInfo.nExternalPort = m_nClientPort;

	/////Buck Add statistics//////
	CCmdCounter::GetHandler()->Count(PINGCMD);
	
	int nRet = 0;
	do 
	{
		nRet = m_pHashTable->AddPeer(PeerInfo);
		if (!nRet)
		{
			break;
		}
		else
		{
			nRet = 0;
		}
		TPingSynCmd PingSynCmd;
		PingSynCmd.m_PeerInfo.strPeerId = PeerInfo.strPeerId;
		PingSynCmd.m_PeerInfo.nInternalIp = PeerInfo.nInternalIp;
//		PingSynCmd.m_PeerInfo.nSubMask = PeerInfo.nSubMask;
		PingSynCmd.m_PeerInfo.nTcpPort = PeerInfo.nTcpPort;
// 		PingSynCmd.m_PeerInfo.nProductFlag = PeerInfo.nProductFlag;
// 		PingSynCmd.m_PeerInfo.nProductReleaseId = PeerInfo.nProductReleaseId;
// 		PingSynCmd.m_PeerInfo.vecSuperNode = PeerInfo.vecSuperNode;
// 		PingSynCmd.m_PeerInfo.nNatType = PeerInfo.nNatType;
		PingSynCmd.m_PeerInfo.nUpnpMapdeIp = PeerInfo.nUpnpMapdeIp;
		PingSynCmd.m_PeerInfo.nUpnpMapedPort = PeerInfo.nUpnpMapedPort;
// 		PingSynCmd.m_PeerInfo.nOnlineTime = PeerInfo.nOnlineTime;
// 		PingSynCmd.m_PeerInfo.nDownloadBytes = PeerInfo.nDownloadBytes;
// 		PingSynCmd.m_PeerInfo.nUploadBytes = PeerInfo.nUploadBytes;
// 		PingSynCmd.m_PeerInfo.nCurUploadResource = PeerInfo.nCurUploadResource;
// 		PingSynCmd.m_PeerInfo.nCurUploadTaskNum = PeerInfo.nCurUploadTaskNum;
// 		PingSynCmd.m_PeerInfo.nCurDownloadTaskNum = PeerInfo.nCurDownloadTaskNum;
 		PingSynCmd.m_PeerInfo.nCurUploadPipeNum = PeerInfo.nCurUploadPipeNum;
// 		PingSynCmd.m_PeerInfo.nCurDownloadSpeed = PeerInfo.nCurDownloadSpeed;
// 		PingSynCmd.m_PeerInfo.nCurUploadSpeed = PeerInfo.nCurUploadSpeed;
		PingSynCmd.m_PeerInfo.nMaxDownloadSpeed = PeerInfo.nMaxDownloadSpeed;
		PingSynCmd.m_PeerInfo.nMaxUploadSpeed = PeerInfo.nMaxUploadSpeed;
// 		PingSynCmd.m_PeerInfo.nSmartUploadSpeedLimit = PeerInfo.nSmartUploadSpeedLimit;
// 		PingSynCmd.m_PeerInfo.nUserUploadSpeedLimit = PeerInfo.nUserUploadSpeedLimit;
// 		PingSynCmd.m_PeerInfo.nDownloadSpeedLimit = PeerInfo.nDownloadSpeedLimit;
// 		PingSynCmd.m_PeerInfo.nPeerStatus = PeerInfo.nPeerStatus;
		PingSynCmd.m_PeerInfo.nLocalBindPort = PeerInfo.nLocalBindPort;
// 		PingSynCmd.m_PeerInfo.nExternalNeighbor = PeerInfo.nExternalNeighbor;
// 		PingSynCmd.m_PeerInfo.nInternalNeighbor = PeerInfo.nInternalNeighbor;
// 		PingSynCmd.m_PeerInfo.strPartnerPeerId = PeerInfo.strPartnerPeerId;
		
		PingSynCmd.m_PeerInfo.nExternalIp = PeerInfo.nExternalIp;
		PingSynCmd.m_PeerInfo.nExternalPort = PeerInfo.nExternalPort;

		int nBufLen = PingSynCmd.GetSize() + TSynBaseCommand::GetSize();
		char *pBuf = new char[nBufLen];
		CBufMgr tmpBufMgr(pBuf, nBufLen);
		CSynEncodeCommand<TPingSynCmd> EncodeCmd(pBuf, nBufLen, PingSynCmd, MSG_LONGCONNECT_SYN_PING);
		if (EncodeCmd.Encode() != 0)
		{
			nRet = -1;
			break;
		}
		if (!m_pServerConf)
		{
			nRet = -1;
			break;
		}
		if (m_pServerConf->GetConf().bUseMulticast)
		{
			TransMsg(EncodeCmd.GetPacket(), (unsigned)EncodeCmd.GetPacketLen(), -1, -1);
			WRITE_TRACE("Trans Ping to Rserver&Qserver");
		}
		else
		{
			int nRServerNum = (int)m_pServerConf->GetConf().vecResourceSrv.size();
			for (int iA = 0; iA < nRServerNum; iA ++)
			{
				TransMsg(EncodeCmd.GetPacket(), (unsigned)EncodeCmd.GetPacketLen(), iA, RSERVERMACHINETYPE);
				WRITE_TRACE("Trans Ping to Rserver [%d]", iA);
			}
			int nQserverNum = (int)m_pServerConf->GetConf().vecQuervSrv.size();
			for (int iA = 0; iA < nQserverNum; iA ++)
			{
				TransMsg(EncodeCmd.GetPacket(), (unsigned)EncodeCmd.GetPacketLen(), iA, QSERVERMACHINETYPE);
				WRITE_TRACE("Trans Ping to Qserver [%d]", iA);
			}
		}

		/////Buck Add statistics//////
		CCmdCounter::GetHandler()->Count(SYNPINGCMD);
	} while (0);
	return nRet;
}

