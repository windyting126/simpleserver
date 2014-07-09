//
// File: TcpSocketHelper.h
// Description: CTcpClientSocketHelper,CTcpServerSocketHelper 类的声明文件
// Purpose: CTcpClientSocketHelper-以TCP的方式连接到指定的服务器,并进行数据的发送与接收.
//          CTcpServerSocketHelper-建立TCP服务器
// Copyright: 暴风网际
// Created by: Zhang Zhou
// Created on: Oct. 22, 2007
// Last Updated: Nov. 15, 2007
//

#pragma once

#include "Winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#include <vector>
using namespace std;

#define MAX_IP_ADDR_LENG 50

class CTcpSocketHelper
{
public:
	CTcpSocketHelper(void);
	~CTcpSocketHelper(void);

protected:

	SOCKET m_Socket;
	BOOL   m_bWinSockInitSucceed;
	BOOL   m_bBlock;

	BOOL InitWinSocket();
	BOOL CreateTcpSocket(BOOL bBlock=TRUE);
	BOOL CloseSocket(SOCKET hSocket=INVALID_SOCKET);
	BOOL UninitWinSocket();

public:

	BOOL IsSocketReadable(DWORD dwTimeOutMS, BOOL &bError, SOCKET hSocket=INVALID_SOCKET);
	BOOL IsSocketWritable(DWORD dwTimeOutMS, SOCKET hSocket=INVALID_SOCKET);
	BOOL SendData(const BYTE *pcBuf, DWORD *pdwBufSize, SOCKET hSocket=INVALID_SOCKET, BOOL bBlock=TRUE);
	BOOL ReceiveData(BYTE *pBuf, DWORD *pdwBufSize, SOCKET hSocket=INVALID_SOCKET, BOOL bBlock=TRUE);
};

//////////////////////////////////////////////////////////////////////////

class CTcpClientSocketHelper : public CTcpSocketHelper
{
public:
	CTcpClientSocketHelper(void);
	~CTcpClientSocketHelper(void);

	//return values:
	// 0 - succeed
	// 1 - non-block,结果pending,调用者随后需要调用IsSocketWritable来查询结果;
	//其它 - 失败
	DWORD Init(LPCTSTR lpcszIp, DWORD dwPort, WORD wBindLocalPort=0, BOOL bBlock=TRUE, DWORD dwConnectTimeOut=0);

	//return values:
	// 0 - succeed
	// 1 - non-block,结果pending,调用者随后需要调用IsSocketWritable来查询结果;
	//其它 - 失败
	DWORD Init(DWORD dwIp, DWORD dwPort, WORD wBindLocalPort=0, BOOL bBlock=TRUE, DWORD dwConnectTimeOut=0);
	BOOL Uninit();

	SOCKET GetSocket();
	SOCKET DumpSocket();

	BYTE *RecvResponseDataPack(DWORD *pdwBufSize,SOCKET hSocket=INVALID_SOCKET);
	VOID ReleaseResponseDataPackBuf(BYTE *pBuf);

	static BOOL IsSocketConnected(SOCKET hSocket,BOOL &bError);
	static BOOL CloseThisSocket(SOCKET hSocket);
	static BOOL SetSocketBlock(SOCKET hSocket,BOOL bBlock=TRUE);

private:
	DWORD  m_dwServerIp;
	WORD   m_dwServerPort;
	BOOL   m_bNoAutoCloseSocket;

	//0 - 连接成功; 1 - non-block,结果pending,调用者随后需要调用IsSocketWritable来查询结果;
	//其它 - 失败
	DWORD ConnectToServer(WORD wBindLocalPort=0, DWORD dwConnectTimeOut=0);
	
};

//////////////////////////////////////////////////////////////////////////

class ITcpServerSocketHelper_Event
{
public:
	virtual void OnAccept(SOCKET acceptSocket, WORD wLocalPart, DWORD dwRemoteIp, WORD wRemotePort)=0;
	virtual void OnDataIn(SOCKET inSocket, const BYTE *pData, DWORD dwDataSize) = 0;
	virtual void OnSocketClose(SOCKET closedSocket, DWORD dwCloseReason) = 0;
};

class CTcpServerSocketHelper : public CTcpSocketHelper 
{
public:
	CTcpServerSocketHelper(void);
	~CTcpServerSocketHelper(void);


	DWORD Start(ITcpServerSocketHelper_Event *pEvent, 
		DWORD dwListenPort=0, BOOL bReadData=TRUE); //成功时，返回监听的端口号；返回-1表示失败
	BOOL Stop();

	DWORD SendDataToClient(SOCKET clientSocket, const BYTE *pData, DWORD dwDataSize);
	BOOL CloseClient(SOCKET clientSocket);

	BOOL CreateListenSocket(DWORD dwListenPort=0);
	SOCKET Accept(DWORD dwTimeOut, DWORD &dwRemoteIp, WORD &wRemotePort);

private:
	ITcpServerSocketHelper_Event *m_pEvent;
	DWORD m_dwListenPort;
	vector<SOCKET> m_AcceptSocketVector;
	CRITICAL_SECTION m_cs;

	HANDLE m_hAcceptThreadHandle;
	HANDLE m_hReadDataThreadHandle;
	HANDLE m_hTerminateAcceptThreadEvent;
	HANDLE m_hTerminateReadDataThreadEvent;

	static DWORD WINAPI _AcceptThreadProc(LPVOID lpParam);
	static DWORD WINAPI _ReadDataThreadProc(LPVOID lpParam);
	

	BOOL Bind();
	BOOL Listen();
};


//////////////////////////////////////////////////////////////////////////

/*---
class ITcpSocketManager_Event
{
public:
	virtual void OnData(SOCKET hSocket, LPVOID lpParam)=0;
	virtual void OnClose()=0;
};

class CTcpSocketManager
{
public:
	CTcpSocketManager();
	~CTcpSocketManager();

	void SetCallback();
	void AddSocket(SOCKET hSocket, LPVOID lpParam);




};
*/
