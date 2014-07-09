/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: UdpRecvThread.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-03
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-03			1.0			
* =======================================================================
*/
#include "framecommon/framecommon.h"
#include "UdpRecvThread.h"
#include "threadgroup.h"

using namespace MYFRAMECOMMON;

CUdpRecvThread::CUdpRecvThread(int iEntityType, int iEntityId, string &strServerIp, unsigned short usPort)
	: CUdpCommonRecvThread(iEntityType, iEntityId, strServerIp, usPort)
{

}

int CUdpRecvThread::Init()
{
	if (CUdpCommonRecvThread::Init() < 0)
	{
		return -1;
	}

	//set socket send buffer length
	int nSockBufLen = 8 * 1024 * 1024;
	int nRet = setsockopt(m_UdpSocket.GetSocket(), SOL_SOCKET, SO_SNDBUF, &nSockBufLen, sizeof(nSockBufLen)); 
	if (nRet != 0)
	{
		WriteRunInfo::WriteError("setsockopt SO_SNDBUF fail: errno %d", errno);
		return -1;
	}

	//set socket recv buffer length
	nSockBufLen = 8 * 1024 * 1024;
	nRet = setsockopt(m_UdpSocket.GetSocket(), SOL_SOCKET, SO_RCVBUF, &nSockBufLen, sizeof(nSockBufLen)); 
	if (nRet != 0)
	{
		WriteRunInfo::WriteError("setsockopt SO_RCVBUF fail: errno %d", errno);
		return -1;
	}

	return 0;
}

int CUdpRecvThread::Run()
{
	int iRecvLen = 0;
	while( m_nRunFlag )
	{
		iRecvLen = m_UdpSocket.UdpRecv();
		if(iRecvLen > 0)
		{
			char *pTmpBuf = new char[iRecvLen];
			int iDecryptLen = iRecvLen;
			mhxy_udp::decrypt ((unsigned char*)m_UdpSocket.GetRecvBuf(), iRecvLen, (unsigned char*)pTmpBuf, (unsigned&)iDecryptLen);
			CUdpCommonBufMsgBody * pUdpBufMsgBody = new CUdpCommonBufMsgBody(pTmpBuf, iDecryptLen, m_UdpSocket.GetSocket(), 
				ntohl(m_UdpSocket.GetCurSrcAddr().sin_addr.s_addr), ntohs(m_UdpSocket.GetCurSrcAddr().sin_port));
			WRITE_DAT(pTmpBuf, iDecryptLen, "////Decrypted Packet////");
			WRITE_TRACE("Recv A Udp Packet : Decrypted packet len=%d", iDecryptLen);
			SendMsg(UDPSENDANDPROCESSENTITY, RANDOMENTITYID, 0 ,UDPCLIENTMSG, pUdpBufMsgBody, 0);
		}
	}
	return 0;
}

