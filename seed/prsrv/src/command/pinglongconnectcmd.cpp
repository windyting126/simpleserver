/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: pinglongconnectcmd.cpp
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
#include "pinglongconnectcmd.h"
#include "commmsg.h"


//TPingSynCmd////////////////////////////////////////////////////////////////////////
int TPingSynCmd::Decode(CBufMgr &bufMgr)
{
	if (bufMgr.GetString (m_PeerInfo.strPeerId) != 0)
	{
		WRITE_TRACE("Ping, Decode for peerid FAILED!!");
		return -1;
	}
	if (m_PeerInfo.strPeerId.length() != COMMON_PEERID_LEN)
	{
		WRITE_TRACE("Ping, peerid len is invalide, %d", m_PeerInfo.strPeerId.length ());
		return -2;
	}
	if (bufMgr.GetInt(m_PeerInfo.nInternalIp) != 0)
	{
		WRITE_TRACE("Ping, Decode for nInternalIp FAILED!!");
		return -1;
	}
// 	if (bufMgr.GetInt(m_PeerInfo.nSubMask) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nSubMask FAILED!!");
// 		return -1;
// 	}
	if (bufMgr.GetShort(m_PeerInfo.nTcpPort) != 0)
	{
		WRITE_TRACE("Ping, Decode for nTcpPort FAILED!!");
		return -1;
	}
// 	if (bufMgr.GetInt(m_PeerInfo.nProductFlag) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nProductFlag FAILED!!");
// 		return -1;
// 	}
// 	if (bufMgr.GetInt(m_PeerInfo.nProductReleaseId) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nProductReleaseId FAILED!!");
// 		return -1;
// 	}
// 	int nSuperNodeNum = 0;
// 	if (bufMgr.GetInt(nSuperNodeNum) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nSuperNodeNum FAILED!!");
// 		return -1;
// 	}
// 	else
// 	{
// 		if (nSuperNodeNum < 0)
// 		{
// 			WRITE_TRACE("Ping, Decode for nSuperNodeNum(< 0) FAILED!!");
// 			return -2;
// 		}
// 	}
// 	m_PeerInfo.vecSuperNode.clear();
// 	string strSuperNodePeerId;
// 	for (int iA = 0; iA < nSuperNodeNum; iA ++)
// 	{
// 		if (bufMgr.GetString(strSuperNodePeerId) != 0)
// 		{
// 			WRITE_TRACE("Ping, Decode for nSuperNodeNum FAILED!!");
// 			return -1;
// 		}
// 		if (strSuperNodePeerId.length() != COMMON_PEERID_LEN)
// 		{
// 			WRITE_TRACE("Ping, supernode peerid len is invalide, %d", strSuperNodePeerId.length ());
// 			return -2;
// 		}
// 		else
// 		{
// 			m_PeerInfo.vecSuperNode.push_back(strSuperNodePeerId);
// 		}
// 	}
// 	if (bufMgr.GetInt(m_PeerInfo.nNatType) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nNatType FAILED!!");
// 		return -1;
// 	}
	if (bufMgr.GetInt(m_PeerInfo.nUpnpMapdeIp) != 0)
	{
		WRITE_TRACE("Ping, Decode for nUpnpMapdeIp FAILED!!");
		return -1;
	}
	if (bufMgr.GetShort(m_PeerInfo.nUpnpMapedPort) != 0)
	{
		WRITE_TRACE("Ping, Decode for nUpnpMapedPort FAILED!!");
		return -1;
	}
// 	if (bufMgr.GetInt(m_PeerInfo.nOnlineTime) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nOnlineTime FAILED!!");
// 		return -1;
// 	}
// 	if (bufMgr.GetInt(m_PeerInfo.nDownloadBytes) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nDownloadBytes FAILED!!");
// 		return -1;
// 	}
// 	if (bufMgr.GetInt(m_PeerInfo.nUploadBytes) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nUploadBytes FAILED!!");
// 		return -1;
// 	}
// 	if (bufMgr.GetShort(m_PeerInfo.nCurUploadResource) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nCurUploadResource FAILED!!");
// 		return -1;
// 	}
// 	if (bufMgr.GetChar(m_PeerInfo.nCurUploadTaskNum) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nCurUploadTaskNum FAILED!!");
// 		return -1;
// 	}
// 	if (bufMgr.GetChar(m_PeerInfo.nCurDownloadTaskNum) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nCurDownloadTaskNum FAILED!!");
// 		return -1;
// 	}
	if (bufMgr.GetChar((char&)m_PeerInfo.nCurUploadPipeNum) != 0)
	{
		WRITE_TRACE("Ping, Decode for nCurUploadPipeNum FAILED!!");
		return -1;
	}
	if (bufMgr.GetChar(m_PeerInfo.nCurUploadPipeStatus) != 0)
	{
		WRITE_TRACE("Ping, Decode for nCurUploadPipeStatus FAILED!!");
		return -1;
	}
// 	if (bufMgr.GetShort(m_PeerInfo.nCurDownloadSpeed) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nCurUploadPipeNum FAILED!!");
// 		return -1;
// 	}
// 	if (bufMgr.GetShort(m_PeerInfo.nCurUploadSpeed) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nCurUploadSpeed FAILED!!");
// 		return -1;
// 	}
	if (bufMgr.GetShort(m_PeerInfo.nMaxDownloadSpeed) != 0)
	{
		WRITE_TRACE("Ping, Decode for nMaxDownloadSpeed FAILED!!");
		return -1;
	}
	if (bufMgr.GetShort(m_PeerInfo.nMaxUploadSpeed) != 0)
	{
		WRITE_TRACE("Ping, Decode for nMaxDownloadSpeed FAILED!!");
		return -1;
	}
// 	if (bufMgr.GetShort(m_PeerInfo.nSmartUploadSpeedLimit) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nMaxDownloadSpeed FAILED!!");
// 		return -1;
// 	}
// 	if (bufMgr.GetShort(m_PeerInfo.nUserUploadSpeedLimit) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nUserUploadSpeedLimit FAILED!!");
// 		return -1;
// 	}
// 	if (bufMgr.GetShort(m_PeerInfo.nDownloadSpeedLimit) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nDownloadSpeedLimit FAILED!!");
// 		return -1;
// 	}
// 	if (bufMgr.GetShort(m_PeerInfo.nPeerStatus) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nDownloadSpeedLimit FAILED!!");
// 		return -1;
// 	}
	if (bufMgr.GetShort(m_PeerInfo.nLocalBindPort) != 0)
	{
		WRITE_TRACE("Ping, Decode for nLocalBindPort FAILED!!");
		return -1;
	}
// 	if (bufMgr.GetShort(m_PeerInfo.nExternalNeighbor) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nExternalNeighbor FAILED!!");
// 		return -1;
// 	}
// 	if (bufMgr.GetShort(m_PeerInfo.nInternalNeighbor) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for nInternalNeighbor FAILED!!");
// 		return -1;
// 	}
// 	if (bufMgr.GetString(m_PeerInfo.strPartnerPeerId) != 0)
// 	{
// 		WRITE_TRACE("Ping, Decode for strPartnerPeerId FAILED!!");
// 		return -1;
// 	}
	
	//////////////////////////////////////////////////////////////////////////
	if (bufMgr.GetInt(m_PeerInfo.nExternalIp) != 0)
	{
		WRITE_TRACE("Ping, Decode for nExternalIp FAILED!!");
		return -1;
	}
	if (bufMgr.GetShort(m_PeerInfo.nExternalPort) != 0)
	{
		WRITE_TRACE("Ping, Decode for nExternalPort FAILED!!");
		return -1;
	}	
	return 0;
}

int TPingSynCmd::Encode (CTmpBufMgr &bufMgr)
{
	int nRet = 0;
	nRet += bufMgr.Put(m_PeerInfo.strPeerId);
	nRet += bufMgr.Put(m_PeerInfo.nInternalIp);
//	nRet += bufMgr.Put(m_PeerInfo.nSubMask);
	nRet += bufMgr.Put(m_PeerInfo.nTcpPort);
// 	nRet += bufMgr.Put(m_PeerInfo.nProductFlag);
// 	nRet += bufMgr.Put(m_PeerInfo.nProductReleaseId);
// 	int iSupernodeNum = (int)m_PeerInfo.vecSuperNode.size();
// 	nRet += bufMgr.Put(iSupernodeNum);
// 	for (int iA = 0; iA < iSupernodeNum; iA ++)
// 	{
// 		nRet += bufMgr.Put(m_PeerInfo.vecSuperNode[iA]);
// 	}
/*	nRet += bufMgr.Put(m_PeerInfo.nNatType);*/
	nRet += bufMgr.Put(m_PeerInfo.nUpnpMapdeIp);
	nRet += bufMgr.Put(m_PeerInfo.nUpnpMapedPort);
// 	nRet += bufMgr.Put(m_PeerInfo.nOnlineTime);
// 	nRet += bufMgr.Put(m_PeerInfo.nDownloadBytes);
// 	nRet += bufMgr.Put(m_PeerInfo.nUploadBytes);
// 	nRet += bufMgr.Put(m_PeerInfo.nCurUploadResource);
// 	nRet += bufMgr.Put(m_PeerInfo.nCurUploadTaskNum);
// 	nRet += bufMgr.Put(m_PeerInfo.nCurDownloadTaskNum);
 	nRet += bufMgr.Put((char)m_PeerInfo.nCurUploadPipeNum);
	nRet += bufMgr.Put(m_PeerInfo.nCurUploadPipeStatus);
// 	nRet += bufMgr.Put(m_PeerInfo.nCurDownloadSpeed);
// 	nRet += bufMgr.Put(m_PeerInfo.nCurUploadSpeed);
	nRet += bufMgr.Put(m_PeerInfo.nMaxDownloadSpeed);
	nRet += bufMgr.Put(m_PeerInfo.nMaxUploadSpeed);
//	nRet += bufMgr.Put(m_PeerInfo.nSmartUploadSpeedLimit);
// 	nRet += bufMgr.Put(m_PeerInfo.nUserUploadSpeedLimit);
// 	nRet += bufMgr.Put(m_PeerInfo.nDownloadSpeedLimit);
// 	nRet += bufMgr.Put(m_PeerInfo.nPeerStatus);
	nRet += bufMgr.Put(m_PeerInfo.nLocalBindPort);
// 	nRet += bufMgr.Put(m_PeerInfo.nExternalNeighbor);
// 	nRet += bufMgr.Put(m_PeerInfo.nInternalNeighbor);
// 	nRet += bufMgr.Put(m_PeerInfo.strPartnerPeerId);
	//////////////////////////////////////////////////////////////////////////
	nRet += bufMgr.Put(m_PeerInfo.nExternalIp);
	nRet += bufMgr.Put(m_PeerInfo.nExternalPort);
	return nRet;
}

int TPingSynCmd::GetSize()
{
	int iLength = 0;
	iLength += sizeof(int) + m_PeerInfo.strPeerId.length();
	iLength += sizeof(m_PeerInfo.nInternalIp);
//	iLength += sizeof(m_PeerInfo.nSubMask);
	iLength += sizeof(m_PeerInfo.nTcpPort);
// 	iLength += sizeof(m_PeerInfo.nProductFlag);
// 	iLength += sizeof(m_PeerInfo.nProductReleaseId);
// 	int iSupernodeNum = (int)m_PeerInfo.vecSuperNode.size();
// 	iLength += sizeof(iSupernodeNum);
// 	for (int iA = 0; iA < iSupernodeNum; iA ++)
// 	{
// 		iLength += sizeof(int) + m_PeerInfo.vecSuperNode[iA].length();
// 	}	
//	iLength += sizeof(m_PeerInfo.nNatType);
	iLength += sizeof(m_PeerInfo.nUpnpMapdeIp);
	iLength += sizeof(m_PeerInfo.nUpnpMapedPort);
// 	iLength += sizeof(m_PeerInfo.nOnlineTime);
// 	iLength += sizeof(m_PeerInfo.nDownloadBytes);
// 	iLength += sizeof(m_PeerInfo.nUploadBytes);
// 	iLength += sizeof(m_PeerInfo.nCurUploadResource);
// 	iLength += sizeof(m_PeerInfo.nCurUploadTaskNum);
// 	iLength += sizeof(m_PeerInfo.nCurDownloadTaskNum);
 	iLength += sizeof(m_PeerInfo.nCurUploadPipeNum);
	iLength += sizeof(m_PeerInfo.nCurUploadPipeStatus);
// 	iLength += sizeof(m_PeerInfo.nCurDownloadSpeed);
// 	iLength += sizeof(m_PeerInfo.nCurUploadSpeed);
	iLength += sizeof(m_PeerInfo.nMaxDownloadSpeed);
	iLength += sizeof(m_PeerInfo.nMaxUploadSpeed);
// 	iLength += sizeof(m_PeerInfo.nSmartUploadSpeedLimit);
// 	iLength += sizeof(m_PeerInfo.nUserUploadSpeedLimit);
// 	iLength += sizeof(m_PeerInfo.nDownloadSpeedLimit);
// 	iLength += sizeof(m_PeerInfo.nPeerStatus);
	iLength += sizeof(m_PeerInfo.nLocalBindPort);
// 	iLength += sizeof(m_PeerInfo.nExternalNeighbor);
// 	iLength += sizeof(m_PeerInfo.nInternalNeighbor);
// 	iLength += sizeof(int) + m_PeerInfo.strPartnerPeerId.length();

	iLength += sizeof(m_PeerInfo.nExternalIp);
	iLength += sizeof(m_PeerInfo.nExternalPort);
	return iLength;
}

//TLogoutSynCmd////////////////////////////////////////////////////////////////////////
int TLogoutSynCmd::Decode(CBufMgr &bufMgr)
{
	if (bufMgr.GetInt(nInternalIp) != 0)
	{
		WRITE_TRACE("Ping, Decode for nInternalIp FAILED!!");
		return -1;
	}
	if (bufMgr.GetString(strPeerId) != 0)
	{
		WRITE_TRACE("Ping, Decode for strPeerId FAILED!!");
		return -1;
	}
	if ((int)strPeerId.length() != COMMON_PEERID_LEN)
	{
		WRITE_TRACE("Ping, Partner peerid len is invalide, %d",strPeerId.length());
		return -2;
	}
	return 0;
}

int TLogoutSynCmd::Encode(CTmpBufMgr &bufMgr)
{
	int nRet = 0;
	nRet += bufMgr.Put(nInternalIp);
	nRet += bufMgr.Put(strPeerId);
	return nRet;
}

int TLogoutSynCmd::GetSize()
{
	int iLength = 0;
	iLength += sizeof(nInternalIp);
	iLength += sizeof(int) + strPeerId.length();
	return iLength;
}

//THandShakeCmd////////////////////////////////////////////////////////////////////////
int THandShakeCmd::Decode(CBufMgr &bufMgr)
{
	if (bufMgr.GetChar(nMachineType) != 0)
	{
		WRITE_TRACE("Ping, Decode for nMachineType FAILED!!");
		return -1;
	}
	if (bufMgr.GetShort(nMachineIndex) != 0)
	{
		WRITE_TRACE("Ping, Decode for nMachineIndex FAILED!!");
		return -1;
	}
	return 0;
}

int THandShakeCmd::Encode(CTmpBufMgr &bufMgr)
{
	int nRet = 0;
	nRet += bufMgr.Put(nMachineType);
	nRet += bufMgr.Put(nMachineIndex);
	return nRet;
}

int THandShakeCmd::GetSize()
{
	int iLength = 0;
	iLength += sizeof(nMachineType);
	iLength += sizeof(nMachineIndex);
	return iLength;
}

//TSynAffirmCmd////////////////////////////////////////////////////////////////////////
int TSynAffirmCmd::Decode(CBufMgr &bufMgr)
{
	if (bufMgr.GetChar(nMachineType) != 0)
	{
		WRITE_TRACE("Ping, Decode for nMachineType FAILED!!");
		return -1;
	}
	if (bufMgr.GetShort(nMachineIndex) != 0)
	{
		WRITE_TRACE("Ping, Decode for nMachineIndex FAILED!!");
		return -1;
	}
	return 0;
}

int TSynAffirmCmd::Encode(CTmpBufMgr &bufMgr)
{
	int nRet = 0;
	nRet += bufMgr.Put(nMachineType);
	nRet += bufMgr.Put(nMachineIndex);
	return nRet;
}

int TSynAffirmCmd::GetSize()
{
	int iLength = 0;
	iLength += sizeof(nMachineType);
	iLength += sizeof(nMachineIndex);
	return iLength;
}

//TOntimeSynCmd////////////////////////////////////////////////////////////////////////
int TOntimeSynCmd::Decode(CBufMgr &bufMgr)
{
	if (bufMgr.GetInt(nHashBucketLen) != 0)
	{
		WRITE_TRACE("Ping, Decode for nHashBucketLen FAILED!!");
		return -1;
	}
	if (bufMgr.GetInt(nListNum) != 0)
	{
		WRITE_TRACE("Ping, Decode for nListNum FAILED!!");
		return -1;
	}
	return 0;
}

int TOntimeSynCmd::DecodeGetPeerList(CBufMgr &bufMgr, int &nHashNum, int &PeerNum)
{
	if(bufMgr.GetInt(nHashNum) != 0)
	{
		WRITE_TRACE("Ping, Decode for nHashNum FAILED!!");
		return -1;
	}
	if(bufMgr.GetInt(PeerNum) != 0)
	{
		WRITE_TRACE("Ping, Decode for PeerNum FAILED!!");
		return -1;
	}
	lstPeerList.clear();
	for (int iA = 0; iA < PeerNum; iA ++)
	{
		if(m_PingSynCmd.Decode(bufMgr) != 0)
		{
			WRITE_TRACE("Ping, Decode for Peer FAILED!!");
			return -1;
		}
		else lstPeerList.push_back(m_PingSynCmd.m_PeerInfo);
	}
	return 0;
}

int TOntimeSynCmd::Encode (CTmpBufMgr &bufMgr)
{
	int nRet = 0;
	nRet += bufMgr.Put(nHashBucketLen);
	nRet += bufMgr.Put(nListNum);
	return nRet;
}

int TOntimeSynCmd::EncodeAddPeerList(CTmpBufMgr &bufMgr, int nHashNum, list<TPingPeerInfo> &lstPeerList)
{
	int nPeerNum = (int)lstPeerList.size();
	if (bufMgr.Put(nHashNum) != 0)
	{
		return -1;
	}
	if (bufMgr.Put(nPeerNum) != 0)
	{
		return -1;
	}
	list<TPingPeerInfo>::iterator itrPeerInfo = lstPeerList.begin();
	for (; itrPeerInfo != lstPeerList.end(); ++ itrPeerInfo)
	{
		m_PingSynCmd.m_PeerInfo = *itrPeerInfo;
		if (m_PingSynCmd.Encode(bufMgr) != 0)
			return -1;
	}
	return 0;
}



