/*
 =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingCmd.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-13
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-13			1.0			
* =======================================================================
*/
#include <iostream>
#include <vector>
#include <string>
#include <sys/time.h>

#include "framecommon/framecommon.h"
#include "udptestclient.h"

using namespace std;
using namespace MYFRAMECOMMON;

const int PING_PROTOCAL_VER = 59;
const int PING_CMDID_PING = 22;

string GenNextPeerId(string& strPeerId)
{
	static int nPos = 0;
	if (strPeerId.length() != 16)
	{
		cout << "GenNextPeerId : strPeerId length is not equal 16" << endl;
		return "";
	}
	char szBuf[16];
	memcpy(szBuf, strPeerId.c_str(), sizeof(szBuf));
	do 
	{
		if (szBuf[nPos] == 57) 
		{
			szBuf[nPos] = 65;
			break;
		}
		if (szBuf[nPos] == 90)
		{
			szBuf[nPos] = 97;
			break;
		}
		if (szBuf[nPos] == 122)
		{
			szBuf[nPos] = 48;
			break;
		}
		if (szBuf[nPos] >= 48 && szBuf[nPos] < 57)
		{
			szBuf[nPos] ++;
			break;
		}
		if (szBuf[nPos] >= 65 && szBuf[nPos] < 90)
		{
			szBuf[nPos] ++;
			break;
		}
		if (szBuf[nPos] >= 97 && szBuf[nPos] < 122)
		{
			szBuf[nPos] ++;
			break;
		}
	} while (0);
	nPos ++;
	nPos %= 16;
	return string(szBuf, 16);
}

struct TPing
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
	TPing()
	{
		nInternalIp = 3232248335;
		nSubMask = 16777215;
		nTcpPort = 8284;
		nProductFlag = 0;
		nProductReleaseId = 0;
		vecSuperNode.clear();
		nNatType = 65797;
		nUpnpMapdeIp = 0;
		nUpnpMapedPort = 0;
		nOnlineTime = 0;
		nDownloadBytes = 0;
		nUploadBytes = 0;
		nCurUploadResource = 0;
		nCurUploadTaskNum = 0;
		nCurDownloadTaskNum = 0;
		nCurUploadPipeNum = 0;
		nCurDownloadSpeed = 0;
		nCurUploadSpeed = 0;
		nMaxDownloadSpeed = 0;
		nMaxUploadSpeed = 0;
		nSmartUploadSpeedLimit = -1;
		nUserUploadSpeedLimit = -1;
		nDownloadSpeedLimit = -1;
		nPeerStatus = 16;
		nLocalBindPort = 0;
		nExternalNeighbor = 0;
		nInternalNeighbor = 0;
		strPartnerPeerId.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	int Encode(CNewBufMgr &bufMgr)
	{
		bufMgr.PutString(strPeerId);
		nInternalIp = htonl(nInternalIp);
		bufMgr.PutInt(nInternalIp);
		bufMgr.PutInt(nSubMask);
		bufMgr.PutShort(nTcpPort);
		bufMgr.PutInt(nProductFlag);
		bufMgr.PutInt(nProductReleaseId);
		int nSuperNodeNum = (int)vecSuperNode.size();
		bufMgr.PutInt(nSuperNodeNum);
		for (int iA = 0; iA < nSuperNodeNum; ++ iA)
		{
			bufMgr.PutString(vecSuperNode[iA]);
		}
		bufMgr.PutInt(nNatType);
		bufMgr.PutInt(nUpnpMapdeIp);
		bufMgr.PutShort(nUpnpMapedPort);
		bufMgr.PutInt(nOnlineTime);
		bufMgr.PutInt(nDownloadBytes);
		bufMgr.PutInt(nUploadBytes);
		bufMgr.PutShort(nCurUploadResource);
		bufMgr.PutChar(nCurUploadTaskNum);
		bufMgr.PutChar(nCurDownloadTaskNum);
		bufMgr.PutChar(nCurUploadPipeNum);
		bufMgr.PutShort(nCurDownloadSpeed);
		bufMgr.PutShort(nCurUploadSpeed);
		bufMgr.PutShort(nMaxDownloadSpeed);
		bufMgr.PutShort(nMaxUploadSpeed);
		bufMgr.PutShort(nSmartUploadSpeedLimit);
		bufMgr.PutShort(nUserUploadSpeedLimit);
		bufMgr.PutShort(nDownloadSpeedLimit);
		bufMgr.PutShort(nPeerStatus);
		bufMgr.PutShort(nLocalBindPort);
		bufMgr.PutShort(nExternalNeighbor);
		bufMgr.PutShort(nInternalNeighbor);
		bufMgr.PutString(strPartnerPeerId);		
		return 0;
	}
	int GetSize()
	{
		int iLength = 0;
		iLength += sizeof(int) + strPeerId.size();
		iLength += sizeof(nInternalIp);
		iLength += sizeof(nSubMask);
		iLength += sizeof(nTcpPort);
		iLength += sizeof(nProductFlag);
		iLength += sizeof(nProductReleaseId);
		int nSuperNodeNum = vecSuperNode.size();
		iLength += sizeof(nSuperNodeNum);
		for (int iA = 0; iA < nSuperNodeNum; ++ iA)
		{
			iLength += sizeof(int) + vecSuperNode[iA].length();
		}
		iLength += sizeof(nNatType);
		iLength += sizeof(nUpnpMapdeIp);
		iLength += sizeof(nUpnpMapedPort);
		iLength += sizeof(nOnlineTime);
		iLength += sizeof(nDownloadBytes);
		iLength += sizeof(nUploadBytes);
		iLength += sizeof(nCurUploadResource);
		iLength += sizeof(nCurUploadTaskNum);
		iLength += sizeof(nCurDownloadTaskNum);
		iLength += sizeof(nCurUploadPipeNum);
		iLength += sizeof(nCurDownloadSpeed);
		iLength += sizeof(nCurUploadSpeed);
		iLength += sizeof(nMaxDownloadSpeed);
		iLength += sizeof(nMaxUploadSpeed);
		iLength += sizeof(nSmartUploadSpeedLimit);
		iLength += sizeof(nUserUploadSpeedLimit);
		iLength += sizeof(nDownloadSpeedLimit);
		iLength += sizeof(nPeerStatus);
		iLength += sizeof(nLocalBindPort);
		iLength += sizeof(nExternalNeighbor);
		iLength += sizeof(nInternalNeighbor);
		iLength += sizeof(int) + strPeerId.length();
		return iLength;
	}
};

class CPingProcess : public CDefaultProcess
{
public:
	virtual char *ProcessSendPacket(char *pSendPacket, int iPacketLen, int &iNewPacketLen)
	{
		if(iPacketLen <= 0)
			return NULL;

		iNewPacketLen = iPacketLen + 20;
		char *pNewBuf = new char[iNewPacketLen];
		mhxy_udp::encrypt(1, (unsigned char*)pSendPacket, iPacketLen, (unsigned char*)pNewBuf, (unsigned&)iNewPacketLen);
		return pNewBuf;
	}
	virtual char *ProcessRecvPacket(char *pRecvPacket, int iPacketLen, int &iNewPacketLen)
	{
		if(iPacketLen <= 0)
			return NULL;
		iNewPacketLen = iPacketLen;
		char *pNewBuf = new char[iNewPacketLen];
		mhxy_udp::decrypt((unsigned char*)pRecvPacket, iPacketLen,(unsigned char*) pNewBuf, (unsigned&)iNewPacketLen);
		return pNewBuf;
	}
};

int main(int argc, char *argv[])
{
	if (argc != 7 && argc != 8 && argc != 9) 
	{
		printf("Usage:%s host port peerid  localip localport uploadpipenum  [bindport] [iCycleTime]\n", argv[0]);
		return -1;
	}	

	int nTmpPort = -1;
	if (argc > 7)
	{
		nTmpPort = atoi(argv[7]);
	}

	CUdpTestClient UdpTestClient(argv[1], atoi(argv[2]), nTmpPort);
	int nRet = UdpTestClient.Init();
	if (nRet != 0)
	{
		cout << "Init UdpTestClient fail" << endl;
		return -1;
	}


	TPing PingCmd;
	if (strlen(argv[3]) == 32)
	{
		PingCmd.strPeerId = CToolKit::BiToHex(argv[3], strlen(argv[3]));
	}
	else
	{
		PingCmd.strPeerId = argv[3];
	}
	
	if(PingCmd.strPeerId.length() != 16)
	{
		cout << "Wrong PeerId" << endl;
		return -1;
	}
	PingCmd.nInternalIp = atoi(argv[4]);
	PingCmd.nLocalBindPort = (short)atoi(argv[5]);
	PingCmd.nCurUploadPipeNum = atoi(argv[6]);
	int iCycleTime = 1;
	if (argc == 9)
	{
		iCycleTime = atoi(argv[8]);
		if(iCycleTime <= 0)
		{
			cout << "iCycletime is less than 0" << endl;
			return -1;
		}
	}


	int iResult = 0;

	struct timeval tv, tv1; 
	gettimeofday(&tv, NULL);
	for (int iA = 0; iA < iCycleTime; iA++)
	{
		iResult = UdpTestClient.SendOnly<TPing, CPingProcess, TDefaultCmdHead>(PING_PROTOCAL_VER, PING_CMDID_PING, PingCmd);
		if(iResult == 0)
		{
			cout << "Succeed" << endl;
		}
		PingCmd.strPeerId = GenNextPeerId(PingCmd.strPeerId);
	}
	gettimeofday(&tv1, NULL);
	cout << "cost time : " << (unsigned)(tv1.tv_sec - tv.tv_sec) << " : " << (unsigned)(tv1.tv_usec - tv.tv_usec) << endl;

	return 0;
}





