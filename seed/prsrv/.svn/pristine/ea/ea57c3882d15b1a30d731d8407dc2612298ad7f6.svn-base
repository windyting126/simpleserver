//
// File: TcpSocketHelper.cpp
// Description: CTcpClientSocketHelper 类的实现文件
// Purpose: 以TCP的方式连接到指定的服务器,并进行数据的发送与接收.
// Notes: 该类只支持作为client端的TCP的数据的收发，即它没有主动的listen功能,不能工作为server端.
// Copyright: 暴风网际
// Created by: Zhang Zhou
// Created on: Oct. 22, 2007
// Last Updated: Nov. 14, 2007
//

#include "stdafx.h"
#include ".\tcpsockethelper.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////////
// CTcpSocketHelper

CTcpSocketHelper::CTcpSocketHelper(void)
{
	m_Socket = INVALID_SOCKET;
	m_bWinSockInitSucceed = FALSE;
	m_bBlock = TRUE;
	InitWinSocket();
}

CTcpSocketHelper::~CTcpSocketHelper(void)
{
	UninitWinSocket();

}

BOOL CTcpSocketHelper::CreateTcpSocket(BOOL bBlock/*=TRUE*/)
{
	m_bBlock = bBlock;

	if (INVALID_SOCKET != m_Socket)
	{ 
		return TRUE;
	}

	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	if (m_Socket==INVALID_SOCKET)
	{
		return FALSE;
	}

	unsigned long lArgp = 1; //NoBlock
	if (bBlock)
	{
		lArgp = 0;
	}

	ioctlsocket(m_Socket, FIONBIO, &lArgp);

	BOOL bOpt = TRUE;
	::setsockopt (m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bOpt, sizeof (bOpt));

	return TRUE;
}

BOOL CTcpSocketHelper::CloseSocket(SOCKET hSocket/*=INVALID_SOCKET*/)
{
	BOOL bResult = FALSE;

	SOCKET _Socket = hSocket==INVALID_SOCKET?m_Socket:hSocket;

	if (INVALID_SOCKET != _Socket)
	{
		//---------------------------
		//加入这段代码，避免关闭后用netstat看还是TIME_WAIT的问题
		struct linger lig;
		int iLen;
		lig.l_onoff=1;
		lig.l_linger=0;
		iLen=sizeof(struct linger);
		::setsockopt(_Socket,SOL_SOCKET,SO_LINGER,(char *)&lig,iLen);
		//----------------------------

		bResult = (::closesocket(_Socket)==SOCKET_ERROR)?FALSE:TRUE;
	}


	return bResult;  
}

BOOL CTcpSocketHelper::InitWinSocket()
{
	WSAData	wsaData;
	int	high	= 2;
	int	low		= 2;
	int ret		= 0;

	do
	{
		ret = ::WSAStartup(MAKEWORD(high, low), &wsaData);
		if (ret == 0)
		{
			break;
		}

		if (low > 0)
		{
			low --;
		}
		else
		{
			high --;
			low = high;	// low version seems would not large than high version.
		}

	}while(ret != 0);

	if (!ret)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CTcpSocketHelper::UninitWinSocket()
{
	//WSACleanUp();
	return (::WSACleanup()==SOCKET_ERROR)?FALSE:TRUE;
	//return TRUE;
}

BOOL CTcpSocketHelper::IsSocketReadable(DWORD dwTimeOutMS, BOOL &bError, SOCKET hSocket/*=INVALID_SOCKET*/)
{
	BOOL bResult = FALSE;
	bError = FALSE;

	SOCKET _Socket = (hSocket==INVALID_SOCKET)?m_Socket:hSocket;

	fd_set readfds;
	fd_set expfds;
	FD_ZERO(&readfds);
	FD_ZERO(&expfds);

	FD_SET(_Socket, &readfds);
	FD_SET(_Socket, &expfds);

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = dwTimeOutMS;

	int	rCode = ::select((int)_Socket+1, &readfds, NULL, &expfds, &timeout);
	switch (rCode) 
	{
	case 0:
		//TimeOut;
		break;
	case SOCKET_ERROR:	// 程序BUG
		/*{
		DWORD dw = GetLastError();
		assert(false);
		return false;
		}*/
		break;
	default:
		if (FD_ISSET(_Socket, &readfds))
		{
			bResult = TRUE;
		}

		if (FD_ISSET(_Socket, &expfds))
		{
			bError = TRUE;
		}
	}

	return bResult;
}

BOOL CTcpSocketHelper::IsSocketWritable(DWORD dwTimeOutMS,SOCKET hSocket/*=INVALID_SOCKET*/)
{
	BOOL bResult = FALSE;

	SOCKET _Socket = (hSocket==INVALID_SOCKET)?m_Socket:hSocket;

	fd_set writefds;
	fd_set expfds;
	FD_ZERO(&writefds);
	FD_ZERO(&expfds);

	FD_SET(_Socket, &writefds);
	FD_SET(_Socket, &expfds);

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = dwTimeOutMS;

	int	rCode = ::select((int)_Socket+1, NULL, &writefds, &expfds, &timeout);
	switch (rCode) 
	{
	case 0:
		//TimeOut;
		break;
	case SOCKET_ERROR:	// 程序BUG
		/*{
		DWORD dw = GetLastError();
		assert(false);
		return false;
		}*/
		break;
	default:
		if (FD_ISSET(_Socket, &writefds))
		{
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL CTcpSocketHelper::SendData(const BYTE *pcBuf, DWORD *pdwBufSize,SOCKET hSocket/*=INVALID_SOCKET*/, BOOL bBlock/*=TRUE*/)
{
    BOOL bResult = FALSE;

	int nCurPos = 0;
	DWORD dwSizeRemaining = *pdwBufSize;
	DWORD dwCurrentTotalSent = 0;
	BOOL _bBlock = (hSocket==INVALID_SOCKET)?m_bBlock:bBlock;
	SOCKET _Socket = (hSocket==INVALID_SOCKET)?m_Socket:hSocket;

	assert(pcBuf);
	assert(pdwBufSize);
	assert(*pdwBufSize); //cannot send zero byte of data
	if (!pcBuf || !pdwBufSize || *pdwBufSize==0)
	{
		goto Exit0;
	}

	if (_bBlock)
	{
		while (TRUE)
		{
			int nRet = ::send(_Socket, (const char*)pcBuf+nCurPos, dwSizeRemaining, 0);
			if (SOCKET_ERROR == nRet)
			{
				goto Exit0;
			}

			dwCurrentTotalSent += nRet;
			nCurPos += nRet;

			if (dwCurrentTotalSent >= *pdwBufSize)
			{//done
				break;
			}

			dwSizeRemaining = *pdwBufSize-dwCurrentTotalSent;
		}

		*pdwBufSize = dwCurrentTotalSent;
	}
	else
	{
		int nRet = ::send(_Socket, (const char*)pcBuf, dwSizeRemaining, 0);
		if (SOCKET_ERROR != nRet)
		{
			*pdwBufSize = (DWORD)nRet;
		}
		else
		{
			int nErr = ::WSAGetLastError();

			if (nErr==WSAEINPROGRESS || nErr==WSAEWOULDBLOCK)
			{
				*pdwBufSize = 0;
			}
			else
			{
				goto Exit0;
			}
		}
	}

	bResult = TRUE;
Exit0:
	return bResult;
}

BOOL CTcpSocketHelper::ReceiveData(BYTE *pBuf, DWORD *pdwBufSize,SOCKET hSocket/*=INVALID_SOCKET*/, BOOL bBlock/*=TRUE*/)
{
	BOOL bResult = FALSE;
	int nCurPos = 0;
	DWORD dwSizeRemaining = *pdwBufSize;
	DWORD dwCurrentTotalRecved = 0;
	BOOL _bBlock = (hSocket==INVALID_SOCKET)?m_bBlock:bBlock;
	SOCKET _Socket = (hSocket==INVALID_SOCKET)?m_Socket:hSocket;

	assert(pBuf);
	assert(pdwBufSize);
	if (!pBuf || !pdwBufSize)
	{
		goto Exit0;
	}

	if (_bBlock)
	{
		while (TRUE)
		{
			int nRet = ::recv(_Socket,(char*)pBuf+nCurPos,dwSizeRemaining,0);
			if (SOCKET_ERROR == nRet)
			{
				DWORD dwErr = ::WSAGetLastError();//WSAEWOULDBLOCK

				if (WSAEWOULDBLOCK == dwErr)
				{//no more data for a non-block socket
					break;
				}

				goto Exit0;
			}

			if (0==nRet)
			{ //the connection has been gracefully closed??
				break;
			}

			dwCurrentTotalRecved += nRet;
			nCurPos += nRet;

			//!!!!!!!!!if (dwCurrentTotalRecved >= *pdwBufSize)
			{ //done
				break;
			}

			dwSizeRemaining = *pdwBufSize-dwCurrentTotalRecved;
		}

		*pdwBufSize = dwCurrentTotalRecved;
	}
	else
	{
		int nRet = ::recv(_Socket,(char*)pBuf,dwSizeRemaining,0);
		if (SOCKET_ERROR != nRet)
		{
			*pdwBufSize = (DWORD)nRet;
		}
		else
		{
			int nErr = ::WSAGetLastError();

			if (nErr==WSAEINPROGRESS || nErr==WSAEWOULDBLOCK)
			{
				*pdwBufSize = 0;
			}
			else
			{
				goto Exit0;
			}
		}
	}

	bResult = TRUE;
Exit0:
	return bResult;
}


//////////////////////////////////////////////////////////////////////////
// CTcpClientSocketHelper

CTcpClientSocketHelper::CTcpClientSocketHelper(void)
{
	m_dwServerIp = 0;
	m_dwServerPort = 0;
	m_bNoAutoCloseSocket = FALSE;
}

CTcpClientSocketHelper::~CTcpClientSocketHelper(void)
{
	if (!m_bNoAutoCloseSocket)
	{
		Uninit();
	}

}

BOOL CTcpClientSocketHelper::SetSocketBlock(SOCKET hSocket,BOOL bBlock/*=TRUE*/)
{
	unsigned long lArgp = 1; //NoBlock
	if (bBlock)
	{
		lArgp = 0;
	}

	return  (SOCKET_ERROR != ioctlsocket(hSocket, FIONBIO, &lArgp));
}

BOOL CTcpClientSocketHelper::CloseThisSocket(SOCKET hSocket)
{
	if (SOCKET_ERROR == ::closesocket(hSocket))
	{
		return FALSE;
	}

	return TRUE; 
}

BOOL CTcpClientSocketHelper::IsSocketConnected(SOCKET hSocket, BOOL &bError)
{
	BOOL bResult = FALSE;

	bError = FALSE;

	fd_set writefds;
	fd_set expfds;
	FD_ZERO(&writefds);
	FD_ZERO(&expfds);

	FD_SET(hSocket, &writefds);
	FD_SET(hSocket, &expfds);

	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int	rCode = ::select((int)hSocket+1, NULL, &writefds, &expfds, &timeout);
	switch (rCode) 
	{
	case 0:
		//TimeOut;
		break;
	case SOCKET_ERROR:	// 程序BUG
		/*{
		DWORD dw = GetLastError();
		assert(false);
		return false;
		}*/
		bError = TRUE;
		break;
	default:
		if (FD_ISSET(hSocket, &writefds))
		{
			bResult = TRUE;
		}

		if (FD_ISSET(hSocket,&expfds))
		{

			int errorcode;
			int len = sizeof(len);
			if (SOCKET_ERROR != ::getsockopt(hSocket , SOL_SOCKET , SO_ERROR , (char*)&errorcode , &len))
			{
				::WSASetLastError(errorcode);
			}

			bError = TRUE;
		}
	}

	return bResult;
}

//return values:
// 0 - succeed
// 1 - non-block,结果pending,调用者随后需要调用IsSocketWritable来查询结果;
//其它 - 失败
DWORD CTcpClientSocketHelper::Init(DWORD dwIp, DWORD dwPort, WORD wBindLocalPort/*=0*/,
								   BOOL bBlock/*=TRUE*/,DWORD dwConnectTimeOut/*=0*/)
{
	m_dwServerIp = dwIp;
	m_dwServerPort = (WORD)dwPort;
	
	if (!CreateTcpSocket(bBlock))
	{
		return 2;
	}

	return ConnectToServer(wBindLocalPort, dwConnectTimeOut);
}

//return values:
// 0 - succeed
// 1 - non-block,结果pending,调用者随后需要调用IsSocketWritable来查询结果;
//其它 - 失败
DWORD CTcpClientSocketHelper::Init(LPCTSTR lpcszIp, DWORD dwPort,WORD wBindLocalPort/*=0*/,BOOL bBlock/*=TRUE*/, DWORD dwConnectTimeOut/*=0*/)
{
	assert(lpcszIp);

	if (!lpcszIp)
	{
		return -1;
	}

	DWORD dwIp = ::inet_addr(lpcszIp);
	if (dwIp == INADDR_NONE || dwIp == 0)
	{
		return 4;
	}

	return Init(dwIp, dwPort,wBindLocalPort, bBlock, dwConnectTimeOut);
}

SOCKET CTcpClientSocketHelper::DumpSocket()
{
	m_bNoAutoCloseSocket = TRUE;
	return m_Socket;
}

SOCKET CTcpClientSocketHelper::GetSocket()
{
	return m_Socket;
}

BOOL CTcpClientSocketHelper::Uninit()
{
	BOOL bResult = TRUE;

	/*
	if (INVALID_SOCKET != m_Socket)
	{
	bResult = (::closesocket(m_Socket)==SOCKET_ERROR)?FALSE:TRUE;

	if (bResult)
	{
	m_Socket = INVALID_SOCKET;
	}
	}
	*/
	if (CloseSocket())
	{
		m_Socket = INVALID_SOCKET;
	}

	/*
	if (m_bWinSockInitSucceed)
	{
	UninitWinSocket();
	m_bWinSockInitSucceed = FALSE;
	}
	*/	

	return bResult;
}


//0 - 连接成功; 1 - non-block,结果pending,调用者随后需要调用IsSocketWritable来查询结果;
//其它 - 失败
DWORD CTcpClientSocketHelper::ConnectToServer(WORD wBindLocalPort/*=0*/, DWORD dwConnectTimeOut/*=0*/)
{
	DWORD dwResult = -1;

	struct sockaddr_in inAddr = {0};
	inAddr.sin_addr.s_addr = m_dwServerIp;
	inAddr.sin_family = AF_INET;
	inAddr.sin_port = ::htons(m_dwServerPort);

	//---------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	if (wBindLocalPort!=0)
	{
		sockaddr_in service = {0};
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = htonl(ADDR_ANY);   //inet_addr("127.0.0.1");
		service.sin_port = htons(wBindLocalPort);

		BOOL bOpt = TRUE;

		::setsockopt (m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bOpt, sizeof (bOpt));

		if (::bind( m_Socket, 
			(SOCKADDR*) &service, 
			sizeof(service)) == SOCKET_ERROR) 
		{
			goto Exit0;
		}
	}
	//---------

	int flags = 64*1024; //64k
	setsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, (char *)&flags, sizeof( flags) );

	int nRet = ::connect(m_Socket, (PSOCKADDR)&inAddr, sizeof(inAddr));
	if (nRet != SOCKET_ERROR)
	{ //connection succeed.
		dwResult = 0;
		goto Exit0;
	}

	int nErr = ::WSAGetLastError();
	if (nErr != WSAEWOULDBLOCK && nErr != WSAEINPROGRESS)
	{
		dwResult = 2;
		goto Exit0;
	}

	fd_set writefds;
	fd_set expfds;
	FD_ZERO(&writefds);
	FD_ZERO(&expfds);

	FD_SET(m_Socket, &writefds);
	FD_SET(m_Socket, &expfds);

	timeval timeout;
	timeout.tv_sec = dwConnectTimeOut / 1000;
	timeout.tv_usec = dwConnectTimeOut % 1000;

	dwResult = 1;
	int	rCode = ::select((int)m_Socket+1, NULL, &writefds, &expfds, &timeout);
	switch (rCode) 
	{
	case 0:
		//TimeOut;
		break;
	case SOCKET_ERROR:	// 程序BUG
		/*{
		DWORD dw = GetLastError();
		assert(false);
		return false;
		}*/
		dwResult = 3;
		break;
	default:
		if (FD_ISSET(m_Socket, &writefds))
		{
			dwResult = 0;
		}

		if (FD_ISSET(m_Socket, &expfds))
		{
			dwResult = 4;
		}
	}

Exit0:
	return dwResult;
}


BYTE *CTcpClientSocketHelper::RecvResponseDataPack(DWORD *pdwBufSize, SOCKET hSocket/*=INVALID_SOCKET*/)
{
	int nCurPos = 0;
	DWORD dwSizeRemaining = 0;
	DWORD dwCurrentTotalRecved = 0;
	BYTE *pResultBuf = NULL;
	DWORD dwBufSize = 0;
	BOOL bRecvSuccesseed = FALSE;

	SOCKET _hSocket = (hSocket==INVALID_SOCKET)?m_Socket:hSocket;

	int nRet = ::recv(_hSocket,(char*)&dwBufSize,sizeof(DWORD),0);
	if (SOCKET_ERROR == nRet)
	{
		goto Exit0;
	}

	dwSizeRemaining = dwBufSize;
	if (dwBufSize==0)
	{
		goto Exit0;
	}

	assert(dwBufSize < 1024*1024);//1m
	if (dwBufSize > 1024*1024)
	{
		goto Exit0;
	}

	pResultBuf = new BYTE [sizeof(DWORD)+dwBufSize];
	if (!pResultBuf)
	{
		goto Exit0;
	}
	*((DWORD*)pResultBuf) = dwBufSize; 
	nCurPos = sizeof(DWORD);

	while (TRUE)
	{
		int nRet = ::recv(_hSocket,(char*)pResultBuf+nCurPos,dwSizeRemaining,0);
		if (SOCKET_ERROR == nRet)
		{
			goto Exit0;
		}

		if (0==nRet)
		{ //the connection has been gracefully closed??
			break;
		}

		dwCurrentTotalRecved += nRet;
		nCurPos += nRet;

		if (dwCurrentTotalRecved >= dwBufSize)
		{ //done
			bRecvSuccesseed = TRUE;
			break;
		}

		//dwSizeRemaining = *pdwBufSize-dwCurrentTotalRecved;
		dwSizeRemaining = dwBufSize-dwCurrentTotalRecved;
	}


	if (pdwBufSize)
	{
		*pdwBufSize = sizeof(DWORD)+dwBufSize;
	}
Exit0:
	if (!bRecvSuccesseed)
	{
		if (pResultBuf)
		{
			delete [] pResultBuf;
			pResultBuf = NULL;
		}
	}

	return pResultBuf;
}

VOID CTcpClientSocketHelper::ReleaseResponseDataPackBuf(BYTE *pBuf)
{
	if (pBuf)
	{
		delete [] pBuf;
	}
}


//////////////////////////////////////////////////////////////////////////
// CTcpServerSocketHelper

CTcpServerSocketHelper::CTcpServerSocketHelper(void)
{
	m_hAcceptThreadHandle = NULL;
	m_hReadDataThreadHandle = NULL;
	m_hTerminateAcceptThreadEvent = NULL;
	m_hTerminateReadDataThreadEvent = NULL;

	InitializeCriticalSection(&m_cs);
}

CTcpServerSocketHelper::~CTcpServerSocketHelper(void)
{
	Stop();
	DeleteCriticalSection(&m_cs);
}

BOOL CTcpServerSocketHelper::Bind()
{
	assert(m_Socket!=INVALID_SOCKET);

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(ADDR_ANY);   //inet_addr("127.0.0.1");
	service.sin_port = htons((WORD)m_dwListenPort);

	if (::bind( m_Socket, 
		(SOCKADDR*) &service, 
		sizeof(service)) == SOCKET_ERROR) 
	{
		return FALSE;
	}

	struct sockaddr_in socket_info;
	int nsize = sizeof(socket_info);
	if (SOCKET_ERROR == ::getsockname(m_Socket, (struct sockaddr FAR *)&socket_info, &nsize))
	{
		return FALSE;
	} 

	m_dwListenPort = (DWORD)::ntohs(socket_info.sin_port);

	return TRUE;
}

BOOL CTcpServerSocketHelper::Listen()
{
	assert(m_Socket!=INVALID_SOCKET);

	//----------------------
	// Listen for incoming connection requests.
	// on the created socket

	if (::listen( m_Socket, /*SOMAXCONN*/5 ) == SOCKET_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}

SOCKET CTcpServerSocketHelper::Accept(DWORD dwTimeOut, DWORD &dwRemoteIp, WORD &wRemotePort)
{
	sockaddr_in clientAddr;
	int nSize = sizeof(clientAddr);

	BOOL bError = FALSE;
	if (IsSocketReadable(dwTimeOut, bError, m_Socket))
	{
		SOCKET AcceptSocket = ::accept( m_Socket, (struct sockaddr* )&clientAddr, &nSize );

		if (AcceptSocket !=INVALID_SOCKET)
		{
			dwRemoteIp = clientAddr.sin_addr.s_addr;
			wRemotePort = clientAddr.sin_port;
			return AcceptSocket; 

		}
	}

	return NULL;
}

BOOL CTcpServerSocketHelper::CreateListenSocket(DWORD dwListenPort/*=0*/)
{
	m_dwListenPort = dwListenPort;

	if (!CreateTcpSocket(TRUE))
	{
		return FALSE;
	}

	BOOL bOpt = TRUE;
	::setsockopt (m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bOpt, sizeof (bOpt));

	if (!Bind())
	{
		return FALSE;
	}

	if (!Listen())
	{
		return FALSE;
	}

	return TRUE;
}

DWORD CTcpServerSocketHelper::Start(ITcpServerSocketHelper_Event *pEvent,
									DWORD dwListenPort/*=0*/,BOOL bReadData/*=TRUE*/)
{
	DWORD dwResult = -1;
	DWORD threadID = 0;

	m_pEvent = pEvent;
	m_dwListenPort = dwListenPort;

	if (!CreateTcpSocket(TRUE))
	{
		dwResult = 1;
		goto Exit0;
	}

	BOOL bOpt = TRUE;
	::setsockopt (m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bOpt, sizeof (bOpt));

	if (!Bind())
	{
		dwResult = 2;
		goto Exit0;
	}

	if (!Listen())
	{
		dwResult = 3;
		goto Exit0;
	}

	m_hTerminateAcceptThreadEvent = ::CreateEvent(NULL, TRUE, FALSE,NULL);
	if (!m_hTerminateAcceptThreadEvent)
	{
		dwResult = 4;
		goto Exit0;
	}

	m_hAcceptThreadHandle = (HANDLE)::CreateThread( NULL, 0, 
		(LPTHREAD_START_ROUTINE)_AcceptThreadProc, this, 0, &threadID );
	if (!m_hAcceptThreadHandle)
	{
		dwResult = 5;
		goto Exit0;
	}


	if (bReadData)
	{
		m_hTerminateReadDataThreadEvent = ::CreateEvent(NULL, TRUE, FALSE,NULL);
		if (!m_hTerminateReadDataThreadEvent)
		{
			dwResult = 6;
			goto Exit0;
		}

		m_hReadDataThreadHandle = (HANDLE)::CreateThread( NULL, 0, 
			(LPTHREAD_START_ROUTINE)_ReadDataThreadProc, this, 0, &threadID );
		if (!m_hReadDataThreadHandle)
		{
			goto Exit0;
		}
	}

	dwResult = 0;
Exit0:
	if (0 != dwResult)
	{
		Stop();
	}

	if (dwResult == 0)
	{
		return m_dwListenPort;
	}
	return (DWORD)-1;
}

BOOL CTcpServerSocketHelper::Stop()
{
	CloseSocket();
			
	if (m_hAcceptThreadHandle)
	{
		::SetEvent(m_hTerminateAcceptThreadEvent);

		if (::WaitForSingleObject(m_hAcceptThreadHandle, 3*1000) == WAIT_OBJECT_0)
		{
			//do nothing
		}
		else 
		{
			::TerminateThread(m_hAcceptThreadHandle, -1);
		}

		::CloseHandle(m_hAcceptThreadHandle);
		m_hAcceptThreadHandle = NULL;
		::CloseHandle(m_hTerminateAcceptThreadEvent);
		m_hTerminateAcceptThreadEvent = NULL;
	}

	if (m_hReadDataThreadHandle)
	{
		::SetEvent(m_hTerminateReadDataThreadEvent);

		if (::WaitForSingleObject(m_hReadDataThreadHandle, 3*1000) == WAIT_OBJECT_0)
		{
			//do nothing
		}
		else 
		{
			::TerminateThread(m_hReadDataThreadHandle, -1);
		}

		::CloseHandle(m_hReadDataThreadHandle);
		m_hReadDataThreadHandle = NULL;
		::CloseHandle(m_hTerminateReadDataThreadEvent);
		m_hTerminateReadDataThreadEvent = NULL;
	}

	m_Socket = INVALID_SOCKET;

/*-------------------------
	vector<SOCKET>::iterator sckVect = m_AcceptSocketVector.begin();

	//从m_AcceptSocketVector erase this socket
	for (;sckVect!=m_AcceptSocketVector.end();++sckVect)
	{
		CloseSocket(*sckVect);
	}
	m_AcceptSocketVector.clear();
-----------------------------*/

	return TRUE;
}

DWORD CTcpServerSocketHelper::SendDataToClient(SOCKET clientSocket, 
											   const BYTE *pData, 
											   DWORD dwDataSize)
{
	DWORD dwSize = dwDataSize;
	if (!SendData(pData,&dwSize,clientSocket))
	{
		return -1;
	}
	return dwSize;
}

BOOL CTcpServerSocketHelper::CloseClient(SOCKET clientSocket)
{
	

	CloseSocket(clientSocket);

	//1) 从m_AcceptSocketVector erase this socket
	EnterCriticalSection(&m_cs);

	vector<SOCKET>::iterator sckVect = m_AcceptSocketVector.begin();

	for (;sckVect!=m_AcceptSocketVector.end();)
	{
		if (*sckVect==clientSocket)
		{
			sckVect = m_AcceptSocketVector.erase(sckVect);
			break;
		}
		else
		{
			++sckVect;
		}
	}

	LeaveCriticalSection(&m_cs);

	// 2) m_pEvent->OnSocketClose(...)
	m_pEvent->OnSocketClose(clientSocket,2);

	return FALSE;
}


DWORD WINAPI CTcpServerSocketHelper::_AcceptThreadProc(LPVOID lpParam)
{
	CTcpServerSocketHelper *This = (CTcpServerSocketHelper *)lpParam;

	DWORD dwResult = -1;

	while(TRUE) 
	{
		sockaddr_in clientAddr;
		int nSize = sizeof(clientAddr);

		BOOL bErr = FALSE;
		if (This->IsSocketReadable(50,bErr, This->m_Socket))
		{
			SOCKET AcceptSocket = ::accept( This->m_Socket, (struct sockaddr* )&clientAddr, &nSize );

			if (AcceptSocket !=INVALID_SOCKET)
			{
				This->m_pEvent->OnAccept(AcceptSocket, (WORD)This->m_dwListenPort, 
					clientAddr.sin_addr.s_addr, clientAddr.sin_port);
				EnterCriticalSection(&This->m_cs);
				This->m_AcceptSocketVector.push_back(AcceptSocket);
				LeaveCriticalSection(&This->m_cs);
			}
		}

		if (::WaitForSingleObject(This->m_hTerminateAcceptThreadEvent, 50)==WAIT_OBJECT_0)
		{
			break;
		}
	}

	dwResult = 0;
	return dwResult;
}

DWORD WINAPI CTcpServerSocketHelper::_ReadDataThreadProc(LPVOID lpParam)
{
	CTcpServerSocketHelper *This = (CTcpServerSocketHelper *)lpParam;

	DWORD dwResult = -1;
	DWORD dwIndex = 0;
	DWORD dwVectorSize = 0;

	while (TRUE)
	{
		EnterCriticalSection(&This->m_cs);
		vector<SOCKET> tempVector(This->m_AcceptSocketVector);
		LeaveCriticalSection(&This->m_cs);

		dwVectorSize = (DWORD)tempVector.size();
		for (dwIndex=0; dwIndex < dwVectorSize; ++dwIndex)
		{
			SOCKET hSock = tempVector[dwIndex];

			BOOL bErr = FALSE;
			if (This->IsSocketReadable(0,bErr,hSock))
			{
				DWORD dwSize = 64*1024;
				BYTE *pyBuf = new BYTE[dwSize];

				assert(pyBuf);
				if (This->ReceiveData(pyBuf,&dwSize,hSock))
				{
					//This->m_pEvent->OnDataIn(hSock, pyBuf, dwSize);
					if (dwSize==0)
					{
						//接收失败！对方已经关闭了连接？
						

						This->CloseSocket(hSock);

						//1) 从This->m_AcceptSocketVector erase this socket
						EnterCriticalSection(&This->m_cs);

						vector<SOCKET>::iterator sckVect = This->m_AcceptSocketVector.begin();

						for (;sckVect!=This->m_AcceptSocketVector.end();)
						{
							if (*sckVect==hSock)
							{
								sckVect = This->m_AcceptSocketVector.erase(sckVect);
								break;
							}
							else
							{
								++sckVect;
							}
						}

						LeaveCriticalSection(&This->m_cs);

						// 2) This->m_pEvent->OnSocketClose(...)
						This->m_pEvent->OnSocketClose(hSock,1);

					}
					else
					{
						This->m_pEvent->OnDataIn(hSock, pyBuf, dwSize);
					}
				}
				else
				{ //接收失败！对方已经关闭了连接？

					

					This->CloseSocket(hSock);

					//1) 从This->m_AcceptSocketVector erase this socket
					EnterCriticalSection(&This->m_cs);

					vector<SOCKET>::iterator sckVect = This->m_AcceptSocketVector.begin();

					for (;sckVect!=This->m_AcceptSocketVector.end();)
					{
						if (*sckVect==hSock)
						{
							sckVect = This->m_AcceptSocketVector.erase(sckVect);
							break;
						}
						else
						{
							++sckVect;
						}
					}

					LeaveCriticalSection(&This->m_cs);

					// 2) This->m_pEvent->OnSocketClose(...)
					This->m_pEvent->OnSocketClose(hSock,1);
				}

				delete [] pyBuf;
			}
			else
			{
				if (bErr)
				{
					//接收失败！对方已经关闭了连接？

					

					This->CloseSocket(hSock);

					//1) 从This->m_AcceptSocketVector erase this socket
					EnterCriticalSection(&This->m_cs);

					vector<SOCKET>::iterator sckVect = This->m_AcceptSocketVector.begin();

					for (;sckVect!=This->m_AcceptSocketVector.end();)
					{
						if (*sckVect==hSock)
						{
							sckVect = This->m_AcceptSocketVector.erase(sckVect);
							break;
						}
						else
						{
							++sckVect;
						}
					}

					LeaveCriticalSection(&This->m_cs);

					// 2) This->m_pEvent->OnSocketClose(...)
					This->m_pEvent->OnSocketClose(hSock,1);

				}

			}
		}

		if (::WaitForSingleObject(This->m_hTerminateReadDataThreadEvent, 500)==WAIT_OBJECT_0)
		{
			break;
		}
	}

	dwResult = 0;
	return dwResult;
}


