#include "UdpMSendThread.h"
#include "PingServer.h"
#include "PingServerConf.h"

CUdpMSendThread::CUdpMSendThread(int iEntityType, int iEntityId, const string& strIp, unsigned short usPort)
	: CUdpSendThread(iEntityType, iEntityId, strIp, usPort), m_nMIP(0), m_nPort(0)
{
}

int CUdpMSendThread::Init()
{
	int nRet;
	nRet = CUdpSendThread::Init();
	if (nRet < 0)
	{
		return -1;
	}

	TPingServerConf &tConf = ((CPingServer*)CMyserver::GetInstance())->GetConf().GetConf();
	m_nMIP = inet_addr(tConf.strMAddr.c_str());
	m_nMIP = ntohl(m_nMIP);
	m_nPort = tConf.nMRecvPort;

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
	
	//set multicast loop
	unsigned char nLoop = tConf.nMLoop;
	nRet = setsockopt(m_UdpSocket.GetSocket(), IPPROTO_IP, IP_MULTICAST_LOOP, &nLoop, sizeof(nLoop));
	if (nRet != 0)
	{
		WriteRunInfo::WriteError("setsockopt IP_MULTICAST_LOOP fail: errno %d loop %u", errno, nLoop);
		return -1;
	}

	//set multicast packet ttl
	unsigned char nTtl = tConf.nMTtl;
	nRet = setsockopt(m_UdpSocket.GetSocket(), IPPROTO_IP, IP_MULTICAST_TTL, &nTtl, sizeof(nTtl));
	if (nRet != 0)
	{
		WriteRunInfo::WriteError("setsockopt IP_MULTICAST_TTL fail: errno %d ttl %u", errno, nTtl);
		return -1;
	}
	
	//set multicast outbound interface address
	struct in_addr addr;
	addr.s_addr = inet_addr(tConf.strMOutIFAddr.c_str());
	nRet = setsockopt(m_UdpSocket.GetSocket(), IPPROTO_IP, IP_MULTICAST_IF, &addr, sizeof(addr));
	if (nRet != 0)
	{
		WriteRunInfo::WriteError("setsockopt IP_MULTICAST_IF fail: errno %d address %s", errno, tConf.strMOutIFAddr.c_str());
		return -1;
	}

	return 0;
}

int CUdpMSendThread::Process(CMsg *pMsg)
{
	if (!pMsg)
	{
		WriteRunInfo::WriteError("null CMsg");
		return -1;
	}

	TMsg *pTMsg = pMsg->GetTMsg();
	CClientMsgBody *pMsgBody = (CClientMsgBody*)pMsg->GetMsgBody();
	if (!pTMsg || !pMsgBody)
	{
		WriteRunInfo::WriteError("null CMsg head or body");
		return -1;
	}

	if (pTMsg->msgType == SYN_MSGTYPE)
	{
		PSERVERINNERCOMMANDPACKET pSrvInnerCmdPacket = (PSERVERINNERCOMMANDPACKET)pMsgBody->GetBuffer();
		int nLen = sizeof(pSrvInnerCmdPacket->serverInnerCommandPacketHead);
		if (nLen >= pMsgBody->GetLen())
		{
			WriteRunInfo::WriteError("invalid message length %d", pMsgBody->GetLen());
			return -1;
		}

		m_UdpSocket.UdpSend(pMsgBody->GetBuffer() + nLen, pMsgBody->GetLen() - nLen, m_nMIP, m_nPort);
	}

	return 0;
}

