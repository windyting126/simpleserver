/**
 * =====================================================================================
 *    @file       udprecvthread.cpp
 *
 *    @brief      base udp recv thread
 *    @details
 *
 *    @version    1.0
 *    @date       10/31/2012 01:23:24 PM
 *
 *    @author     wangtiefeng (), wangtf418@163.com
 *    @copyright  
 *
 * =====================================================================================
 */
#include "udprecvthread.h"
#include "threadgroup.h" 


CUdpRecvThread::CUdpRecvThread(int iEntityType,  int iEntityId, CUdpCommonSock* pUdpRecvSocket)
	: CWorkThread(iEntityType, iEntityId), m_pUdpSocket(pUdpRecvSocket)
{

}

CUdpRecvThread::~CUdpRecvThread()
{

}

int CUdpRecvThread::Init()
{
	if (!m_pUdpSocket || (m_pUdpSocket->GetSocket() == INVALID_SOCKET))
	{
		WriteRunInfo::WriteLog("CUdpRecvThread::Init : no udp socket");
		return -1;
	}
	return 0;
}

int CUdpRecvThread::Run()
{
	int iRecvLen = 0;
	while(m_nRunFlag)
	{
		iRecvLen = m_pUdpSocket->UdpRecv(m_pUdpSocket->GetSocket(), m_RecvBuf, MAX_UDP_RECV_PACKET_LEN, m_pUdpSocket->GetCurSrcAddr());
		if(iRecvLen > 0)
		{
			char* pTmpBuf = new char[iRecvLen];
			memcpy(pTmpBuf , m_RecvBuf, iRecvLen);
			CUdpCommonBufMsgBody* pUdpBufMsgBody = new CUdpCommonBufMsgBody(pTmpBuf, iRecvLen, m_pUdpSocket->GetSocket(),	ntohl(m_pUdpSocket->GetCurSrcAddr().sin_addr.s_addr), ntohs(m_pUdpSocket->GetCurSrcAddr().sin_port));
			SendMsg(UDPSENDANDPROCESSENTITY, RANDOMENTITYID, 0 ,UDPCLIENTMSG, pUdpBufMsgBody, 0);
		}
	}
	return 0;
}

int CUdpRecvThread::PutMsgToInputList(CMsg* pMsg)
{
	if(pMsg != NULL)
	{
		delete pMsg;
	}
	return 0;
}

int CUdpRecvThread::Process(CMsg* pMsg)
{
	return 0;
}



