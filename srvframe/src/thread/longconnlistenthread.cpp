/*
* =======================================================================
* Copyright (c) 2012-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconnlistenthread.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : Wang Tiefeng (), wangtiefeng@baofeng.com
* Company		: 
* Date			: 2012-2-14
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
* -----------------------------------------------------------------------
*		Wang Tiefeng	2012-02-14			1.0			
* =======================================================================
*/

#include "longconnlistenthread.h"

CLongConnListenThread::CLongConnListenThread(int iEntityType, int iEntityId, int nHost, short nPort, int nListenTaskType /* = LONGCONNLISTENTASK */) : CNetProcessThread(iEntityType, iEntityId)
{
	m_nHost = nHost;
	m_nPort = nPort;
	m_nListenTaskType = nListenTaskType;
}

CLongConnListenThread::~CLongConnListenThread()
{

}

int CLongConnListenThread::Init()
{
	int nRet = CNetProcessThread::Init();
	if (nRet != 0)
	{
		return -1;
	}

	//生成一个监听句柄
	int nSocket;
	char szClientclientIp[INET_ADDRSTRLEN];
	int nIp = htonl(m_nHost);
	inet_ntop(AF_INET, &nIp, szClientclientIp, sizeof(szClientclientIp));
	nRet = CMySocket::CreateSock(SOCK_STREAM, szClientclientIp, m_nPort, nSocket);
	if (nRet < 0)
	{
		WriteRunInfo::WriteLog("CLongConnListenThread::Init : Create listen sock fail %d", nRet);
		return -1;
	}
	
	CTcpSocket* pTcpSocket = new CTcpSocket(szClientclientIp, m_nPort, nSocket);
	if (pTcpSocket == NULL)
	{
		WriteRunInfo::WriteLog("CLongConnListenThread::Init : New listen socket object fail");
		return -1;
	}
	
	//起监听端口                    
	CSocketTask* pListenTask = CMyserver::GetInstance()->GetClassFactory()->GenSocketTask(this, pTcpSocket, m_nListenTaskType);
	if (pListenTask == NULL)
	{
		WriteRunInfo::WriteLog("CLongConnListenThread::Init : Gen Listen Task fail");
		delete pTcpSocket;
		return -1;
	}                      
	nRet = pListenTask->Init();
	if (nRet != 0)
	{
		WriteRunInfo::WriteLog("CLongConnListenThread::Init : Init Listen Task fail");
		return -1;
	}
	return 0;
}
