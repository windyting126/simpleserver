#include "udpmrecvthread.h"
#include "rsrvdef.h"
#include "myconfig.h"
#include "rserver.h"
#include "protocol.h"
#include "statistic.h"
#include <string.h>

CUdpMRecvThread::CUdpMRecvThread(int iEntityType, int iEntityId, string& strIp, unsigned short usPort)
	: CUdpCommonRecvThread(iEntityType, iEntityId, strIp, usPort)
{
}

int CUdpMRecvThread::Init()
{
	int nRet = CUdpCommonRecvThread::Init();
	if (nRet < 0)
	{
		return -1;
	}

	//set socket send buffer length
	int nSockBufLen = 8 * 1024 * 1024;
	nRet = setsockopt(m_UdpSocket.GetSocket(), SOL_SOCKET, SO_SNDBUF, &nSockBufLen, sizeof(nSockBufLen));
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

	//add multicast address and set its inbound interface address
	CMyConfig &Conf = ((CPRServer*)CMyserver::GetInstance())->GetMyConf();	
	struct ip_mreq group;
	group.imr_multiaddr.s_addr = inet_addr(Conf.GetMAddr().c_str());
	group.imr_interface.s_addr = inet_addr(Conf.GetMInIFAddr().c_str());
	nRet = setsockopt(m_UdpSocket.GetSocket(), IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group));
	if (nRet < 0)
	{
		WriteRunInfo::WriteError("setsockopt IP_ADD_MEMBERSHIP fail: %d", errno);
		return -1;
	}

	return 0;
}

int CUdpMRecvThread::Run()
{
	int nPacketLen;
	unsigned nMsgLen;
	while (m_nRunFlag)
	{
		nPacketLen = m_UdpSocket.UdpRecv();
		if (nPacketLen < (int)(sizeof(SERVERCOMMANDPACKET) + SYN_MSG_HEAD_LEN))
		{
			WriteRunInfo::WriteTrace("invalid udp packet length %d, errno %d", nPacketLen, errno);
			continue;
		}
		PSERVERCOMMANDPACKET pServerCommandPacket = (PSERVERCOMMANDPACKET)m_UdpSocket.GetRecvBuf();
		if (!pServerCommandPacket || (nMsgLen = ntohl(pServerCommandPacket->iMsgLen)) > (unsigned)MAX_RT_PING_SYN_MESSAGE_LENGTH)
		{
			WriteRunInfo::WriteError("null packet %p or message length", pServerCommandPacket);
			continue;
		}
		if (ntohs(pServerCommandPacket->usMsgId) == ONLINE_SYN)
		{
			char *pMsg = new char[nMsgLen];
			memcpy(pMsg, pServerCommandPacket->serverCommandPacketContent, nMsgLen);
			unsigned char nCmdType = *(unsigned char*)(pMsg + SYN_CMDINFO_OFF);
			WRITE_TRACE("Receive udp mrecv msg : msgId = %u", nCmdType);
			CCmdCounter::GetHandler()->AddCount(nCmdType);
			CMsgBody *pMsgBody = new CBufMsgBody(pMsg, nMsgLen);
			SendMsg(PING_SYN_PROCESS_ENTITY_TYPE, RANDOMENTITYID, 0, PING_SYN_PROCESS_MSG_TYPE, pMsgBody);
		}
	}
	
	return 0;
}

