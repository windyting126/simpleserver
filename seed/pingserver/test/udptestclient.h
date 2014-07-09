/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: UdpProcAndSendThread.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-07-24
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-07-24			1.0			
* =======================================================================
*/
#ifndef _UDPTESTCLIENT_H_47E4CFF1_844B_45ff_AE94_E200E5791F7C_
#define _UDPTESTCLIENT_H_47E4CFF1_844B_45ff_AE94_E200E5791F7C_

#include <iostream>
#include <vector>
#include <string>
#include <sys/time.h>


#include "framecommon/framecommon.h"

using namespace std;
using namespace MYFRAMECOMMON;

struct TDefaultCmdHead
{
	int m_nProtocolVersion; //协议版本
	char m_nCmd;			//消息id（命令号）

	//Translate byte order flag
	bool m_bIfNatTrans;

	TDefaultCmdHead(bool bIfNatTrans = false)
	{
		m_nProtocolVersion = 0;
		m_nCmd = 0;
		m_bIfNatTrans = bIfNatTrans;
	}
	TDefaultCmdHead(int nVersion, int nCmd, bool bIfNatTrans = false)
	{
		m_nProtocolVersion = nVersion;
		m_nCmd = nCmd;

		m_bIfNatTrans = bIfNatTrans;
	}
	int Encode(CNewBufMgr &bufMgr)
	{
		if(!m_bIfNatTrans)
			bufMgr.SetIfNetTrans(0);
		int ret = 0;
		ret += bufMgr.PutInt(m_nProtocolVersion);
		ret += bufMgr.PutChar(m_nCmd);
		return ret;
	}
	int Decode(CNewBufMgr &bufMgr)
	{
		if(!m_bIfNatTrans)
			bufMgr.SetIfNetTrans(0);
		int ret = 0;
		ret += bufMgr.GetInt(m_nProtocolVersion);
		ret += bufMgr.GetChar(m_nCmd);
		return ret;
	}
	int GetSize()
	{
		int iLength = 0;
		iLength += sizeof(m_nProtocolVersion);
		iLength += sizeof(m_nCmd);
		return iLength;
	}
	void Print()
	{
		cout << "m_nProtocolVersion = " << m_nProtocolVersion << endl;
		unsigned int nCmd = (unsigned char)m_nCmd;
		cout << "m_nCmd = " << nCmd << endl;
	}
};

struct TDefaultResCmdHead : public TDefaultCmdHead
{
	char m_nResult;			//回包结果

	TDefaultResCmdHead(bool bIfNatTrans = false) : TDefaultCmdHead(bIfNatTrans)
	{
		m_nResult = 0;
	}
	int Encode(CNewBufMgr &bufMgr)
	{
		int ret = 0;
		ret += TDefaultCmdHead::Encode(bufMgr);
		ret += bufMgr.PutChar(m_nResult);
		return ret;
	}
	int Decode(CNewBufMgr &bufMgr)
	{
		int ret = 0;
		ret += TDefaultCmdHead::Decode(bufMgr);
		ret += bufMgr.GetChar(m_nResult);
		return ret;
	}
	int GetSize()
	{
		int iLength = 0;
		iLength += TDefaultCmdHead::GetSize();
		iLength += sizeof(m_nResult);
		return iLength;
	}
	void Print()
	{
		TDefaultCmdHead::Print();
		cout << "m_nResult = " << (unsigned)m_nResult << endl;
	}
	int GetResult()
	{
		return m_nResult;
	}
};

struct TDefaultResCmdHeadWithoutRet : public TDefaultCmdHead
{
	int GetResult()
	{
		return 1;
	}
};

class CDefaultProcess
{
public:
	virtual ~CDefaultProcess(){}
public:
	virtual char *ProcessSendPacket(char *pSendPacket, int iPacketLen, int &iNewPacketLen)
	{
		char *pNewPacket = pSendPacket;
		iNewPacketLen = iPacketLen;
		return pNewPacket;
	}
	virtual char *ProcessRecvPacket(char *pRecvPacket, int iPacketLen, int &iNewPacketLen)
	{
		char *pNewPacket = pRecvPacket;
		iNewPacketLen = iPacketLen;
		return pNewPacket;
	}
};

class CUdpTestClient
{
public:
	CUdpTestClient(const string &strDestIp, unsigned short usDestPort, int nSrcPort = -1)
	{
		m_strDestIp = strDestIp;
		m_usDestPort= usDestPort;
		m_iUdpSocket = 0;
		m_nSrcPort = nSrcPort;
	}
	~CUdpTestClient(){}

	int Init()
	{
		timeval tm1;
		gettimeofday( &tm1 , NULL );
		srand( (unsigned)tm1.tv_usec );

		int iResult = CreateUdpSocket();
		if( iResult <= 0 )
		{
			printf( "Create udp Socket Failed\n" );
			return -1;
		}

		if( m_strDestIp != "" )
		{
			if( inet_pton(  AF_INET , m_strDestIp.c_str() , &m_addrDest.sin_addr ) <= 0 )	
			{
				printf( "The Dest Ip is not right!\n" );
				return -1;
			}
		}
		m_addrDest.sin_family = AF_INET;
		m_addrDest.sin_port = htons( m_usDestPort );
		return 0;
	}

	template<class T, class T1, class CProcess, class TCmdHead,  class TResCmdHead>
	int SendAndRecv(int nVersion, int nCmd, T &in, T1 &out, TResCmdHead &resHead)
	{
		//创建请求包
		CNewBufMgr bufMgr;
		TCmdHead head(nVersion, nCmd);
		head.Encode(bufMgr);
		in.Encode(bufMgr);

		//处理发送包：加密或压缩
		CProcess Process;
		int iNewRealBodyLen;
		char *pNewBodyBuf = Process.ProcessSendPacket((char*)bufMgr.GetBuf().c_str() , bufMgr.GetBuf().length(), iNewRealBodyLen);
		if(pNewBodyBuf == NULL)
		{
			//cout << "ProcessSendPacket Failed" << endl;
			return -1;
		}	

		int ret = UdpSend(pNewBodyBuf, iNewRealBodyLen, m_addrDest);
		if (ret != iNewRealBodyLen)
		{   
			//cout << "Send Packet Failed" << endl;
			return -1;
		}


		//接受回包
		char RecvBuf[10000] = {0};
		sockaddr_in addrRecvFrom ;
		ret = UdpRecv(RecvBuf, 10000, addrRecvFrom);
		if (ret <= 0)
		{
			//cout << "Recv Packet fail" << endl;
			return -1;
		}

		//处理接收包：加密或压缩
		int iNewPacketLen;
		char *pNewRecvBuf = Process.ProcessRecvPacket(RecvBuf, ret, iNewPacketLen);
		if(pNewRecvBuf == NULL)
		{
			//cout << "Process Recv Packet Failed" << endl;
			return -1;
		}
		CBufMgr NewRecvBufMgr;
		if(pNewRecvBuf != RecvBuf)
		{
			NewRecvBufMgr.AttachBuf(pNewRecvBuf, iNewPacketLen);
		}
		CNewBufMgr tmpBufMgr(string(pNewRecvBuf, iNewPacketLen));

		ret = resHead.Decode(tmpBufMgr);
		if(ret != 0)
		{
			//cout << "Decode Msg Head Failed" << endl;
			return -1;
		}

		if(resHead.GetResult())
		{
			ret = out.Decode(tmpBufMgr);
			if(ret != 0)
			{
				//cout << "Decode Msg Body Failed" << endl;
				return -1;
			}
		}
		else
		{
			cout << "Return Result = " << resHead.GetResult() << endl;
		}

		return 0;    
	}

	template<class T, class CProcess, class TCmdHead>
	int SendOnly(int nVersion, int nCmd, T &in)
	{
		//创建请求包
		CNewBufMgr bufMgr;
		TCmdHead head(nVersion, nCmd);
		head.Encode(bufMgr);
		in.Encode(bufMgr);

		//处理发送包：加密或压缩
		CProcess Process;
		int iNewRealBodyLen;
		char *pNewBodyBuf = Process.ProcessSendPacket((char*)bufMgr.GetBuf().c_str() , bufMgr.GetBuf().length(), iNewRealBodyLen);
		if(pNewBodyBuf == NULL)
		{
			//cout << "ProcessSendPacket Failed" << endl;
			return -1;
		}	

		int ret = UdpSend(pNewBodyBuf, iNewRealBodyLen, m_addrDest);
		if (ret != iNewRealBodyLen)
		{   
			//cout << "Send Packet Failed" << endl;
			return -1;
		}

		return 0;    
	}

	template<class T1, class CProcess,  class TResCmdHead>
	int RecvOnly(T1 &out, TResCmdHead &resHead)
	{
		//接受回包
		char RecvBuf[10000] = {0};
		sockaddr_in addrRecvFrom ;
		int ret = UdpRecv(RecvBuf, 10000, addrRecvFrom);
		if (ret <= 0)
		{
			//cout << "Recv Packet fail" << endl;
			return -1;
		}

		//处理接收包：加密或压缩
		CProcess Process;
		int iNewPacketLen;
		char *pNewRecvBuf = Process.ProcessRecvPacket(RecvBuf, ret, iNewPacketLen);
		if(pNewRecvBuf == NULL)
		{
			//cout << "Process Recv Packet Failed" << endl;
			return -1;
		}
		CBufMgr NewRecvBufMgr;
		if(pNewRecvBuf != RecvBuf)
		{
			NewRecvBufMgr.AttachBuf(pNewRecvBuf, iNewPacketLen);
		}
		CNewBufMgr tmpBufMgr(string(pNewRecvBuf, iNewPacketLen));

		ret = resHead.Decode(tmpBufMgr);
		if(ret != 0)
		{
			//cout << "Decode Msg Head Failed" << endl;
			return -1;
		}

		if(resHead.GetResult())
		{
			ret = out.Decode(tmpBufMgr);
			if(ret != 0)
			{
				//cout << "Decode Msg Body Failed" << endl;
				return -1;
			}
		}
		else
		{
			cout << "Return Result = " << resHead.GetResult() << endl;
		}

		return 0;    
	}


public:
	int CreateUdpSocket()
	{
		struct sockaddr_in sockaddrServ;
		memset( &sockaddrServ, 0, sizeof( sockaddrServ ) );

		sockaddrServ.sin_addr.s_addr = htonl(INADDR_ANY);
		sockaddrServ.sin_family = AF_INET;
		unsigned short usPort;
		if (m_nSrcPort == -1)
		{
			usPort = random()%10000;
			usPort += 40000;
			sockaddrServ.sin_port = htons( usPort );
		}
		else
		{
			usPort = m_nSrcPort;
			sockaddrServ.sin_port = htons(m_nSrcPort);
		}

		m_iUdpSocket = socket(AF_INET, SOCK_DGRAM, 0);
		if( m_iUdpSocket < 0 )
			return -1;
		int iAddrReuseFlag = 1;
		setsockopt( m_iUdpSocket , SOL_SOCKET, SO_REUSEADDR, &iAddrReuseFlag,  sizeof( iAddrReuseFlag ) );
		if( bind( m_iUdpSocket , (struct sockaddr *) &sockaddrServ, sizeof( sockaddrServ) ) < 0) 
		{
			close( m_iUdpSocket );
			return -1;
		}
		printf( "Create a Udp  socket and bind port %u to it!\n " ,   usPort );
		return m_iUdpSocket;
	}

	static int CreateAUdpSocket()
	{
		struct sockaddr_in sockaddrServ;
		memset( &sockaddrServ, 0, sizeof( sockaddrServ ) );

		sockaddrServ.sin_addr.s_addr = htonl(INADDR_ANY);
		sockaddrServ.sin_family = AF_INET;
		unsigned short usPort = random()%10000;
		usPort += 40000;
		sockaddrServ.sin_port = htons( usPort );

		int iSocket = -1;
		iSocket = socket(AF_INET, SOCK_DGRAM, 0);
		if( iSocket < 0 )
			return -1;
		int iAddrReuseFlag = 1;
		setsockopt( iSocket , SOL_SOCKET, SO_REUSEADDR, &iAddrReuseFlag,  sizeof( iAddrReuseFlag ) );
		if( bind( iSocket , (struct sockaddr *) &sockaddrServ, sizeof( sockaddrServ) ) < 0) 
		{
			close( iSocket );
			return -1;
		}
		printf( "Create a Udp  socket and bind port %u to it!\n " ,   usPort );
		return iSocket;
	}

	int UdpRecv(char *pRecvBuf, unsigned uiRecvBufLen, sockaddr_in &addrRecvFrom)
	{
		if(pRecvBuf == NULL)
			return -1;
		socklen_t sockLen = sizeof(addrRecvFrom );
		int iResult = recvfrom(m_iUdpSocket, pRecvBuf, uiRecvBufLen, 0, (sockaddr *)&addrRecvFrom , &sockLen);
		if (iResult < 0)
		{
			printf("%s:%d:%s : Udp Recv failed!\n", __FILE__, __LINE__, __FUNCTION__);
		}
		return iResult;
	}

	static int UdpRecv(int iSock, char *pRecvBuf, unsigned uiRecvBufLen, sockaddr_in &addrRecvFrom)
	{
		if (pRecvBuf == NULL)
			return -1;
		socklen_t sockLen = sizeof(addrRecvFrom);
		int iResult = recvfrom(iSock, pRecvBuf, uiRecvBufLen, 0, ( sockaddr * )&addrRecvFrom, &sockLen);
		if(iResult < 0)
		{
			printf("%s:%d:%s : Udp Recv failed!\n", __FILE__, __LINE__, __FUNCTION__);
		}
		return iResult;
	}

	int UdpSend(const char *pBuf, unsigned int uiBufLen, const sockaddr_in &addrDest)
	{
		int iResult = 0;
		if(pBuf == NULL)
		{
			printf("Error : %s:%d:%s : Wrong parameter in UdpSend\n", __FILE__ , __LINE__ , __FUNCTION__);
			return -1;
		}
		iResult = sendto(m_iUdpSocket, pBuf, uiBufLen, 0, (sockaddr*)&addrDest, sizeof(addrDest));
		if ((unsigned)iResult < uiBufLen)
		{                             
			printf("Error : %s:%d:%s : UdpSend failed\n", __FILE__, __LINE__, __FUNCTION__);
			return -1;
		}
		return iResult;
	}

	static int UdpSend(int iSocket, const char *pBuf, unsigned int uiBufLen, const sockaddr_in &addrDest)
	{
		int iResult = 0;
		if(pBuf == NULL)
		{
			printf("Error : %s:%d:%s : Wrong parameter in UdpSend\n", __FILE__, __LINE__, __FUNCTION__);
			return -1;
		}

		//	iResult = sendto( m_iUdpSocket , "Hello" , iBufLen , 0 ,( sockaddr * )&addrClient , sizeof( addrClient ) );
		iResult = sendto(iSocket, pBuf, uiBufLen, 0, (sockaddr*)&addrDest, sizeof(addrDest));
		if ((unsigned)iResult < uiBufLen)
		{                             
			printf( "Error : %s:%d:%s : UdpSend failed\n" , __FILE__ , __LINE__ , __FUNCTION__ );
			return -1;
		}
		return iResult;
	}


private:
	int m_iUdpSocket;
	string m_strDestIp;
	unsigned short m_usDestPort;
	sockaddr_in m_addrDest;
	int m_nSrcPort;
};

#endif


