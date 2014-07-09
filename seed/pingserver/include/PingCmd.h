/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingCmd.h
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
#ifndef _PINGCMD_H_A9ED5F5A_6329_4fb9_B077_D72EB80B7559_
#define _PINGCMD_H_A9ED5F5A_6329_4fb9_B077_D72EB80B7559_

#include "PingSrvBaseCommand.h"

struct TPingCmd
{
	string strPeerId;
	int nInternalIp;
	int nSubMask;
	short nTcpPort;
	int nProductFlag;
	int nProductReleaseId;
	vector<string> vecSuperNode;
	int nNatType;
	int nUpnpMapdeIp;
	short nUpnpMapedPort;
	int nOnlineTime;
	int nDownloadBytes;
	int nUploadBytes;
	short nCurUploadResource;
	char nCurUploadTaskNum;
	char nCurDownloadTaskNum;
	char nCurUploadPipeNum;
	short nCurDownloadSpeed;
	short nCurUploadSpeed;
	short nMaxDownloadSpeed;
	short nMaxUploadSpeed;
	short nSmartUploadSpeedLimit;
	short nUserUploadSpeedLimit;
	short nDownloadSpeedLimit;
	short nPeerStatus;
	short nLocalBindPort;
	short nExternalNeighbor;
	short nInternalNeighbor;
	string strPartnerPeerId;

	//////////////////////////////////////////////////////////////////////////
	int Decode(CBufMgr &bufMgr)
	{
		if (bufMgr.GetString (strPeerId) != 0)
		{
			WRITE_TRACE("Ping, Decode for peerid FAILED!!");
			return -1;
		}
		if (strPeerId.length() != PEERLENGTH)
		{
			WRITE_TRACE("Ping, peerid len is invalide, %d", strPeerId.length ());
			return -2;
		}
		if (bufMgr.GetInt(nInternalIp) != 0)
		{
			WRITE_TRACE("Ping, Decode for nInternalIp FAILED!!");
			return -1;
		}
		nInternalIp = ntohl(nInternalIp);
		if (bufMgr.GetInt(nSubMask) != 0)
		{
			WRITE_TRACE("Ping, Decode for nSubMask FAILED!!");
			return -1;
		}
		nSubMask = ntohl(nSubMask);
		if (bufMgr.GetShort(nTcpPort) != 0)
		{
			WRITE_TRACE("Ping, Decode for nTcpPort FAILED!!");
			return -1;
		}
		if (bufMgr.GetInt(nProductFlag) != 0)
		{
			WRITE_TRACE("Ping, Decode for nProductFlag FAILED!!");
			return -1;
		}
		if (bufMgr.GetInt(nProductReleaseId) != 0)
		{
			WRITE_TRACE("Ping, Decode for nProductReleaseId FAILED!!");
			return -1;
		}
		int nSuperNodeNum = 0;
		if (bufMgr.GetInt(nSuperNodeNum) != 0)
		{
			WRITE_TRACE("Ping, Decode for nSuperNodeNum FAILED!!");
			return -1;
		}
		else
		{
			if (nSuperNodeNum < 0)
			{
				WriteRunInfo::WriteError ("Ping, Decode for nSuperNodeNum(< 0) FAILED!!");
				return -2;
			}
			
		}
		string strSuperNodePeerId;
		for (int iA = 0; iA < nSuperNodeNum; iA ++)
		{
			if (bufMgr.GetString(strSuperNodePeerId) != 0)
			{
				WRITE_TRACE("Ping, Decode for nSuperNodeNum FAILED!!");
				return -1;
			}
			if (strSuperNodePeerId.length() != PEERLENGTH)
			{
				WRITE_TRACE("Ping, supernode peerid len is invalide, %d", strSuperNodePeerId.length ());
				return -2;
			}
			else
			{
				vecSuperNode.push_back(strSuperNodePeerId);
			}
		}
		if (bufMgr.GetInt(nNatType) != 0)
		{
			WRITE_TRACE("Ping, Decode for nNatType FAILED!!");
			return -1;
		}
		if (bufMgr.GetInt(nUpnpMapdeIp) != 0)
		{
			WRITE_TRACE("Ping, Decode for nUpnpMapdeIp FAILED!!");
			return -1;
		}
		nUpnpMapdeIp = ntohl(nUpnpMapdeIp);
		if (bufMgr.GetShort(nUpnpMapedPort) != 0)
		{
			WRITE_TRACE("Ping, Decode for nUpnpMapedPort FAILED!!");
			return -1;
		}
		if (bufMgr.GetInt(nOnlineTime) != 0)
		{
			WRITE_TRACE("Ping, Decode for nOnlineTime FAILED!!");
			return -1;
		}
		if (bufMgr.GetInt(nDownloadBytes) != 0)
		{
			WRITE_TRACE("Ping, Decode for nDownloadBytes FAILED!!");
			return -1;
		}
		if (bufMgr.GetInt(nUploadBytes) != 0)
		{
			WRITE_TRACE("Ping, Decode for nUploadBytes FAILED!!");
			return -1;
		}
		if (bufMgr.GetShort(nCurUploadResource) != 0)
		{
			WRITE_TRACE("Ping, Decode for nCurUploadResource FAILED!!");
			return -1;
		}
		if (bufMgr.GetChar(nCurUploadTaskNum) != 0)
		{
			WRITE_TRACE("Ping, Decode for nCurUploadTaskNum FAILED!!");
			return -1;
		}
		if (bufMgr.GetChar(nCurDownloadTaskNum) != 0)
		{
			WRITE_TRACE("Ping, Decode for nCurDownloadTaskNum FAILED!!");
			return -1;
		}
		if (bufMgr.GetChar(nCurUploadPipeNum) != 0)
		{
			WRITE_TRACE("Ping, Decode for nCurUploadPipeNum FAILED!!");
			return -1;
		}
		if (bufMgr.GetShort(nCurDownloadSpeed) != 0)
		{
			WRITE_TRACE("Ping, Decode for nCurUploadPipeNum FAILED!!");
			return -1;
		}
		if (bufMgr.GetShort(nCurUploadSpeed) != 0)
		{
			WRITE_TRACE("Ping, Decode for nCurUploadSpeed FAILED!!");
			return -1;
		}
		if (bufMgr.GetShort(nMaxDownloadSpeed) != 0)
		{
			WRITE_TRACE("Ping, Decode for nMaxDownloadSpeed FAILED!!");
			return -1;
		}
		if (bufMgr.GetShort(nMaxUploadSpeed) != 0)
		{
			WRITE_TRACE("Ping, Decode for nMaxDownloadSpeed FAILED!!");
			return -1;
		}
		if (bufMgr.GetShort(nSmartUploadSpeedLimit) != 0)
		{
			WRITE_TRACE("Ping, Decode for nMaxDownloadSpeed FAILED!!");
			return -1;
		}
		if (bufMgr.GetShort(nUserUploadSpeedLimit) != 0)
		{
			WRITE_TRACE("Ping, Decode for nUserUploadSpeedLimit FAILED!!");
			return -1;
		}
		if (bufMgr.GetShort(nDownloadSpeedLimit) != 0)
		{
			WRITE_TRACE("Ping, Decode for nDownloadSpeedLimit FAILED!!");
			return -1;
		}
		if (bufMgr.GetShort(nPeerStatus) != 0)
		{
			WRITE_TRACE("Ping, Decode for nDownloadSpeedLimit FAILED!!");
			return -1;
		}
		if (bufMgr.GetShort(nLocalBindPort) != 0)
		{
			WRITE_TRACE("Ping, Decode for nLocalBindPort FAILED!!");
			return -1;
		}
		if (bufMgr.GetShort(nExternalNeighbor) != 0)
		{
			WRITE_TRACE("Ping, Decode for nExternalNeighbor FAILED!!");
			return -1;
		}
		if (bufMgr.GetShort(nInternalNeighbor) != 0)
		{
			WRITE_TRACE("Ping, Decode for nInternalNeighbor FAILED!!");
			return -1;
		}
		if (bufMgr.GetString(strPartnerPeerId) != 0)
		{
			WRITE_TRACE("Ping, Decode for strPartnerPeerId FAILED!!");
			return -1;
		}
		return 0;
	}
};

class CPingCmd : public CPingSrvBaseCommand<TPingCmd>
{
public:
	CPingCmd(char *pBuf, int nLen, int nPublicIp, short nPublicPort,  int nEntityType, int nEntityId);
	~CPingCmd(){}
protected:
	virtual int ProcessCmd();
};

#endif

