/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: pingpeerdef.h
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
#ifndef _PINGPEERDEF_H_DC8ADFF3_B4A4_4712_ADD5_DA24DA9DEF10_
#define _PINGPEERDEF_H_DC8ADFF3_B4A4_4712_ADD5_DA24DA9DEF10_

#include <string>
#include <vector>
#include "framecommon/framecommon.h"

using namespace std;
using namespace MYFRAMECOMMON;

const int SYN_VERSION = 256;

const int SYN_MSG_HEAD_LEN = sizeof(int) + sizeof(char);
const int SYN_CMDINFO_OFF = sizeof(int);

const int LOGOUT_FLAG_YES = 1;
const int LOGOUT_FLAG_NO = 0;	//这个值必须是0，他是PEER_INFO中成员的缺省值，也是Clear后的值

const int SYNSEND_RESPONSE_WAIT = 0;
const int SYNSEND_RESPONSE = 1;

const int MSG_LONGCONNECT_SYN_PING = 0;
const int MSG_LONGCONNECT_SYN_LOGOUT = 2;
const int MSG_LONGCONNECT_SYN_ONTIME = 4;
const int MSG_LONGCONNECT_SYN_HANDSHAKE = 6;
const int MSG_LONGCONNECT_SYN_AFFIRM = 8;
const int MSG_LONGCONNECT_SYN_AFFIRMRES = 9;

enum EDiffType
{
	DIF_SAME = 0,
	DIF_ONLINE_TIME,
	DIF_PEERDATA,
	DIF_UPLOADPIPENUM
};

enum EUploadPipeState
{
	UPS_FREE = 0,
	UPS_NORMAL,
	UPS_BUSY	
};

struct TPingPeerIntData
{
	int nInternalIp;
	//int nSubMask;
	short nTcpPort;
	//int nProductFlag;
	//int nProductReleaseId;

	//int nNatType;
	int nUpnpMapdeIp;
	short nUpnpMapedPort;
	
// 	int nDownloadBytes;
// 	int nUploadBytes;
// 	short nCurUploadResource;
// 	char nCurUploadTaskNum;
// 	char nCurDownloadTaskNum;
	char nCurUploadPipeStatus;
// 	short nCurDownloadSpeed;
// 	short nCurUploadSpeed;
	short nMaxDownloadSpeed;
	short nMaxUploadSpeed;
// 	short nSmartUploadSpeedLimit;
// 	short nUserUploadSpeedLimit;
// 	short nDownloadSpeedLimit;
// 	short nPeerStatus;
	short nLocalBindPort;
// 	short nExternalNeighbor;
// 	short nInternalNeighbor;

	int nExternalIp;
	short nExternalPort;


	TPingPeerIntData()
	{
		memset(this, 0, sizeof(*this));
		nCurUploadPipeStatus = -1;		
	}

	TPingPeerIntData(const TPingPeerIntData &BasicPeerInfo)
	{
		*this = BasicPeerInfo;
	}

	TPingPeerIntData & operator=(const TPingPeerIntData &BasicPeerInfo)
	{
		if (this != &BasicPeerInfo)
		{
			memcpy(this, &BasicPeerInfo, sizeof(BasicPeerInfo));
		}
		return *this;
	}

	int Differ(const TPingPeerIntData &BasicPeerInfo) const
	{
		return memcmp(this, &BasicPeerInfo, sizeof(BasicPeerInfo));
	}
};

struct TPingPeerInfo : public TPingPeerIntData
{
	string strPeerId;
// 	vector<string> vecSuperNode;
// 	string strPartnerPeerId;
// 
// 	int nOnlineTime;

	unsigned long long  ullLastReportTime;
	unsigned char nCurUploadPipeNum;
	char cLogoutFlag;

	TPingPeerInfo()
	{
//		nOnlineTime = 0;
		ullLastReportTime = 0;
		nCurUploadPipeNum = 0;
		cLogoutFlag = LOGOUT_FLAG_NO;
	}

	unsigned int GetIndex() const
	{
		return CToolKit::shortELFhash(strPeerId.c_str(), (unsigned)strPeerId.length());
	}
	TPingPeerInfo & operator=(const TPingPeerInfo &BasicPeerInfo)
	{
		TPingPeerIntData::operator=(BasicPeerInfo);
		strPeerId = BasicPeerInfo.strPeerId;
// 		nOnlineTime = BasicPeerInfo.nOnlineTime;
// 		vecSuperNode = BasicPeerInfo.vecSuperNode;
// 		strPartnerPeerId = BasicPeerInfo.strPartnerPeerId;
		nCurUploadPipeNum = BasicPeerInfo.nCurUploadPipeNum;
		ullLastReportTime = BasicPeerInfo.ullLastReportTime;
		cLogoutFlag = BasicPeerInfo.cLogoutFlag;
		return *this;
	}
	EDiffType Differ(const TPingPeerInfo &BasicPeerInfo) const
	{
		EDiffType eDiff = DIF_SAME;
		do 
		{
			if (TPingPeerIntData::Differ(BasicPeerInfo))
			{
				eDiff = DIF_PEERDATA;
				break;
			}			
// 			if (vecSuperNode != BasicPeerInfo.vecSuperNode)
// 			{
// 				eDiff = DIF_PEERDATA;
// 				break;
// 			}
// 			if (strPartnerPeerId != BasicPeerInfo.strPartnerPeerId)
// 			{
// 				eDiff = DIF_PEERDATA;
// 				break;
// 			}
			if (cLogoutFlag != BasicPeerInfo.cLogoutFlag)
			{
				eDiff = DIF_PEERDATA;
				break;
			}			
// 			if (nOnlineTime != BasicPeerInfo.nOnlineTime)
// 			{
// 				eDiff = DIF_ONLINE_TIME;
// 				break;
// 			}
			if (nCurUploadPipeNum != BasicPeerInfo.nCurUploadPipeNum)
			{
				eDiff = DIF_UPLOADPIPENUM;
				break;
			}			

		} while (0);
		return eDiff;
	}
	bool operator==(const TPingPeerInfo &BasicPeerInfo) const
	{
		return (strPeerId == BasicPeerInfo.strPeerId);
	}
	bool operator!=(const TPingPeerInfo &BasicPeerInfo) const
	{
		return !(operator==(BasicPeerInfo));
	}
};

#endif


