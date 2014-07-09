#include "multilistenthread.h"

int CMultiListenThread::Init()
{
	int retValue = CListenThread::Init();
	if (retValue != 0)
	{
		return -1;
	}
	CThreadLock ThreadLock(&m_ListenListLock);
	list<TListenInfo>::iterator itrListen = m_lstListenInfo.begin();
	while (itrListen != m_lstListenInfo.end())
	{
		string sHost = itrListen->m_strHost;
		unsigned short nPort = itrListen->m_nPort;
		itrListen->ListenBlock.mutex.Lock();
		if (itrListen->ListenBlock.nSock == -1)
		{
			//生成一个监听句柄
			int nSocket;                           
			retValue = CMySocket::CreateSock(SOCK_STREAM, sHost, nPort, nSocket);
			if (retValue < 0)
			{
				WriteRunInfo::WriteLog("Create listen sock fail %d", retValue);
				itrListen->ListenBlock.mutex.UnLock();
				return -1;
			}
			itrListen->ListenBlock.nSock=nSocket;
		}
		itrListen->ListenBlock.mutex.UnLock();

		CTcpSocket *pTcpSocket = new CTcpSocket(sHost, nPort, itrListen->ListenBlock.nSock);
		if (pTcpSocket == NULL)
		{
			WriteRunInfo::WriteLog("New listen socket object fail");
			return -1;
		}

		//起监听端口                    
		CListenTask* pListenTask = (CListenTask*)CMyserver::GetInstance()->GetClassFactory()->GenSocketTask(this, pTcpSocket,  LISTENTASK);        
		
		if (pListenTask == NULL)
		{
			WriteRunInfo::WriteLog("Malloc memory fail or listen fail");
			return -1;
		}
		retValue = pListenTask->Init();
		if (retValue != 0)
		{
			WriteRunInfo::WriteLog("Init Listen Task Fail(%s:%d)", sHost.c_str(), nPort);
			return -1;
		}
		itrListen ++;
	}
	return 0;
}

//静态变量
CThreadMutex CMultiListenThread::m_ListenListLock;
list<CMultiListenThread::TListenInfo> CMultiListenThread::m_lstListenInfo;

//静态函数
int CMultiListenThread::AddListen(const string& sHost, unsigned short nPort)
{
	CThreadLock ThreadLock(&m_ListenListLock);

	m_lstListenInfo.push_back(TListenInfo(sHost, nPort));
	return 0;
}

//汪铁丰 2011-04-14这种添加方式，会导致多线程操作epoll，是服务器崩溃
// int CMultiListenThread::AddListen(const string &sHost, unsigned short nPort)
// {
// 	int nSocket = -1;
// 	int retValue = CMySocket::CreateSock(SOCK_STREAM, sHost, nPort, nSocket);
// 	if (retValue < 0)
// 	{
// 		WriteRunInfo::WriteLog("Create listen sock(%s:%d) fail %d", sHost.c_str(), nPort, retValue);
// 		return retValue;
// 	}
// 	CTcpSocket *pTcpSocket = new CTcpSocket(sHost, nPort, nSocket);
//        if (pTcpSocket == NULL)
//        {
//                 WriteRunInfo::WriteLog("New listen socket object fail");
// 		   close(nSocket);
//                 return -1;
//      }
//         
//起监听端口                    
//	   CListenTask *pListenTask = (CListenTask*)CMyserver::GetInstance()->GetClassFactory()->GenSocketTask(this, pTcpSocket,  LISTENTASK);        
// 	 if (pListenTask == NULL)
//	{
//              WriteRunInfo::WriteLog("Malloc memory fail or listen fail");
//               return -1;
//  }                 
//			return pListenTask->Init();            
// }
