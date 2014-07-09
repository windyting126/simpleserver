/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: LogoutCmd.cpp
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
const int PING_CMDID_PING = 23;

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

struct TLogout
{
	int nInternalIp;
	string strPeerId;
	//////////////////////////////////////////////////////////////////////////
	TLogout()
	{
		nInternalIp = 624076992;
	}
	//////////////////////////////////////////////////////////////////////////
	int Encode(CNewBufMgr &bufMgr)
	{
		bufMgr.PutInt(nInternalIp);
		bufMgr.PutString(strPeerId);
		return 0;
	}
	int GetSize()
	{
		int iLength = 0;
		iLength += sizeof(nInternalIp);
		iLength += sizeof(int) + strPeerId.size();
		return iLength;
	}
};

class CLogoutProcess : public CDefaultProcess
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
	if (argc != 4 && argc != 5) 
	{
		printf("Usage:%s host port peerid [iCycleTime]\n", argv[0]);
		return -1;
	}	

	CUdpTestClient UdpTestClient(argv[1], atoi(argv[2]));
	int nRet = UdpTestClient.Init();
	if (nRet != 0)
	{
		cout << "Init UdpTestClient fail" << endl;
		return -1;
	}


	TLogout Logout;
	if (strlen(argv[3]) == 32)
	{
		Logout.strPeerId = CToolKit::BiToHex(argv[3], strlen(argv[3]));
	}
	else
	{
		Logout.strPeerId = argv[3];
	}	
	
	if(Logout.strPeerId.length() != 16)
	{
		cout << "Wrong PeerId" << endl;
		return -1;
	}

	int iCycleTime = 1;
	if (argc == 5)
	{
		iCycleTime = atoi(argv[4]);
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
		iResult = UdpTestClient.SendOnly<TLogout, CLogoutProcess, TDefaultCmdHead>(PING_PROTOCAL_VER, PING_CMDID_PING, Logout);
		if(iResult == 0)
		{
			cout << "Succeed" << endl;
		}
		char szTmp[Logout.strPeerId.size()];
		memcpy(szTmp, Logout.strPeerId.c_str(), sizeof(szTmp));
		Logout.strPeerId = GenNextPeerId(Logout.strPeerId);
	}
	gettimeofday(&tv1, NULL);
	cout << "cost time : " << (unsigned)(tv1.tv_sec - tv.tv_sec) << " : " << (unsigned)(tv1.tv_usec - tv.tv_usec) << endl;

	return 0;
}





