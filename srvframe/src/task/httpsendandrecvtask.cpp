/*
 * =====================================================================================
 *
 *       Filename:  httpsendandrecvtask.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年04月12日 15时13分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wangtiefeng (), wangtf418@163.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <string>

#include "httpsendandrecvtask.h"

CHttpSendAndRecvTask::CHttpSendAndRecvTask(CTcpSocket* pSocket, CNetProcessThread* pWorkThread, int nTaskTimeout /* = HTTP_SENDANDRECV_TASK_TIMEOUT */)
	: CRecvAndSendTask(pSocket, pWorkThread)
{
	m_uReceivePos = 0;
	m_uReceiveSize = 0;
	m_State = ReadHead;
	m_HttpHead.m_HttpMethod = HttpMethod_INIT;
	m_nErrorNum = EHSR_OK;
	m_nContentLen = 0;
	m_nHasRecvBodyLen = 0;
	m_nHttpRetCode = 0;
	m_bIfClose = false; //HTTP/1.1 support keep-alive as default

	m_bIfConnectSuc = false;

	ResetRecvBuffer(0, MAXHTTPHEADLEN);
	m_nTcpSocketTimeout = nTaskTimeout;
}

CHttpSendAndRecvTask::~CHttpSendAndRecvTask()
{

}

int CHttpSendAndRecvTask::DestroySelf()
{
	delete this;
	return 0;
}

int CHttpSendAndRecvTask::Init()
{
	return 0;
}

int CHttpSendAndRecvTask::ConnectServer()
{
	int nSocket = -1;	
	int nRet = CMySocket::CreateNoListenSock(SOCK_STREAM,  nSocket); 
	if (nRet < 0)
	{
		return -1;
	}
	else
	{
		struct sockaddr_in siDestSrv;		
		memset(&siDestSrv, 0, sizeof(siDestSrv));
		siDestSrv.sin_family = AF_INET;
		inet_pton(AF_INET, m_HttpHead.m_strIp.c_str(), &siDestSrv.sin_addr);
		siDestSrv.sin_port = htons(m_HttpHead.m_nPort);
		m_pTcpSocket = new CTcpSocket(siDestSrv, sizeof(siDestSrv), nSocket);
		SetNonBlocking(nSocket);
		connect(nSocket, (sockaddr*)&siDestSrv, sizeof(siDestSrv));
	}
	AddToEpoll();
	m_nStartTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
	return 0;
}

int CHttpSendAndRecvTask::AddToEpoll()
{
	int nEvent = MYPOLLERR | MYPOLLHUP | MYPOLLIN;
	if (m_sendList.GetNum() != 0 || m_pCurSendMsg != NULL || m_bIfConnectSuc == false)
	{
		nEvent = nEvent | MYPOLLOUT;
	}
	CNetProcessThread* pNetThread = (CNetProcessThread*)m_pThread;
	CEpoll* pEoll = pNetThread->GetEpoll();
	pEoll->AddToEpoll(this, nEvent);
	return 0;
}

int CHttpSendAndRecvTask::ModifyEpollAddWrite()
{
	int nEvent = MYPOLLERR | MYPOLLHUP | MYPOLLIN | MYPOLLOUT;
	CNetProcessThread* pNetThread = (CNetProcessThread*)m_pThread;
	CEpoll* pEoll = pNetThread->GetEpoll();
	pEoll->ModifyFromEpoll(this, nEvent);
	return 0;
}

int CHttpSendAndRecvTask::ModifyEpollDelWrite()
{
	if (m_sendList.GetNum() != 0 || m_pCurSendMsg != NULL || m_bIfConnectSuc == false)
	{
		return 0;
	}
	int nEvent = MYPOLLERR | MYPOLLHUP | MYPOLLIN;
	CNetProcessThread* pNetThread = (CNetProcessThread*)m_pThread;
	CEpoll* pEoll = pNetThread->GetEpoll();
	pEoll->ModifyFromEpoll(this, nEvent);
	return 0;
}

int CHttpSendAndRecvTask::HandleProcess(int nEvent)
{
	if ((nEvent & MYPOLLERR) == MYPOLLERR || (nEvent & MYPOLLHUP) == MYPOLLHUP)
	{
		WriteRunInfo::WriteError("http sr socket error, event=%d socket=%d error=%d [threadtype = %d]", nEvent, GetSocket(), errno, m_pThread->GetEntityType());
		CMyserver::GetInstance()->GetCommonStat().AddNormalStat(SOCKETERRORTYPE);
		m_nErrorNum = EHSR_Epoll;
		DestroySelf();
		return -1;
	}

	m_nStartTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
	
	int nRet = 0;
	do 
	{
		if ((nEvent & MYPOLLIN) == MYPOLLIN)
		{
			nRet = RecvProcess();
			if (nRet == -1)
			{
				m_nErrorNum = EHSR_RecvFail;
				break;
			}
		}
		
		if ((nEvent & MYPOLLOUT) == MYPOLLOUT)
		{
			if (m_bIfConnectSuc)
			{
				nRet = SendProcess(); 
				if (nRet == -1)
				{
					m_nErrorNum = EHSR_SendFail;
					break;
				}
			}
			else
			{
				int nSocket = GetSocket();
				int nError = 0;;
				socklen_t nSockLen = sizeof(nError);
				getsockopt(nSocket, SOL_SOCKET, SO_ERROR, &nError, &nSockLen);
				if (nError != 0)
				{
					m_nErrorNum = EHSR_ConnectFail;
					break;
				}
				m_bIfConnectSuc = true;
				ModifyEpollDelWrite();
			}
		} 
	} while(0);
	if (m_nErrorNum != EHSR_OK)
	{
		DestroySelf();
	}
	return 0;
}

void CHttpSendAndRecvTask::ResetRecvBuffer(unsigned int uReceivePos, unsigned int uReceiveSize)
{
	m_uReceivePos = uReceivePos;
	m_uReceiveSize = uReceiveSize;
}

int CHttpSendAndRecvTask::RecvProcess()
{
	int nRet = 0;
	if (m_HttpHead.m_HttpMethod == HttpMethod_INIT)
	{
		WriteRunInfo::WriteError("CHttpSendAndRecvTask::RecvProcess : no Method");
		m_nErrorNum = EHSR_Exception;
		return -1;
	}

	switch (m_State)
	{
		case ReadHead:
			{
				nRet = ReadHttpHead();
				break;
			}
		case ReadBody:
			{
				nRet = ReadHttpBody();
				break;
			}
	}
	return nRet;
}

int CHttpSendAndRecvTask::SendProcess()
{
	int nRet = 0;
	if (m_pCurSendMsg != NULL)
	{
		nRet = SendData();
	}
	else
	{
		m_pCurSendMsg = GetMsgFromSendList();
		if (m_pCurSendMsg != NULL)
		{
			nRet = SendData(); 
		} 
		else
		{
			ModifyEpollDelWrite();
		}
	}
	return nRet;
}

int CHttpSendAndRecvTask::ReadHttpHead()
{
	int nRet = 0;
	int nNeedReadLen = m_uReceiveSize - m_uReceivePos;
	if (nNeedReadLen <= 0)	
	{
		WriteRunInfo::WriteError("CHttpSendAndRecvTask::ReadHttpHead : the httphead is too large"); 
		return -1;
	}
	if (m_uReceivePos == 0)
	{
		memset(&m_szRecvBuf[0], 0, RECVBODYMAXLEN);
	}
	int nRecvLen = recv(GetSocket(), &m_szRecvBuf[0] + m_uReceivePos, nNeedReadLen, MSG_DONTWAIT);
	if (nRecvLen == 0)
	{
		WriteRunInfo::WriteTrace("CHttpSendAndRecvTask::ReadHttpHead : recv client close , close the socket");
		return -1;
	}

	if (nRecvLen < 0)
	{
		if (errno != EINTR && errno != EAGAIN)
		{
			WriteRunInfo::WriteLog("CHttpSendAndRecvTask::ReadHttpHead : recv error(%d) ", errno);
			return -1;
		}
		return 0;
	}

	m_uReceivePos += nRecvLen;
	//GetHttpHead
	char* pHttpHeadEnd = (char*)strstr(m_szRecvBuf, "\r\n\r\n");
	if (!pHttpHeadEnd)
	{
		return 0;
	}
	m_strRetHttpHead = string(m_szRecvBuf, pHttpHeadEnd - m_szRecvBuf + 4);
	WriteRunInfo::WriteTrace("CHttpSendAndRecvTask::ReadHttpHead : Res HttpHead : %s", m_strRetHttpHead.c_str());
	if (m_HttpHead.m_HttpMethod == HttpMethod_GET || m_HttpHead.m_HttpMethod == HttpMethod_POST)
	{
		m_State = ReadBody;
	}

	string strTmp;
	//GetHttpRetCode
	int nTmp = CToolKit::GetStringByLabel(m_strRetHttpHead, " ", " ", strTmp);
	if (nTmp != 0)
	{
		WriteRunInfo::WriteError("CHttpSendAndRecvTask::ReadHttpHead : Get Http Ret Code fail");
		return -1;	
	}
	m_nHttpRetCode = atoi(strTmp.c_str());
	
	//GetConnectionOption
	 nTmp = CToolKit::GetStringByLabel(m_strRetHttpHead, "Connection:", "\r\n", strTmp); 
	 if (nTmp == 0 && strTmp.find("close") != string::npos)
	 {
		 m_bIfClose = true;	
	 }

	//GetContentLen
	m_nContentLen = 0;
	nTmp = CToolKit::GetStringByLabel(m_strRetHttpHead, "Content-Length:", "\r\n", strTmp);
	if (nTmp == 0) //有Content-Length
	{
		m_nContentLen = strtoull(strTmp.c_str(), NULL, 10);
		m_nHasRecvBodyLen = m_uReceivePos - m_strRetHttpHead.length();
	}
	else //没有Content-Length
	{
		nTmp = CToolKit::GetStringByLabel(m_strRetHttpHead, "Transfer-Encoding:", "\r\n", strTmp); 
		if (nTmp == 0)
		{
			CToolKit::StringTrim(strTmp);
			if (strTmp == "chunked")
			{
				WriteRunInfo::WriteError("CHttpSendAndRecvTask::ReadHttpHead : not support chunked");
				return -1;
			}
		}
	}

	//ProcessHttpHead
	nRet = ProcessHttpHead();		
	if (nRet > 0)
	{
		//重定向
		return 0;
	}
	else if (nRet < 0)
	{
		return -1;
	}

	if (m_HttpHead.m_HttpMethod == HttpMethod_HEAD)
	{
		ResetRecvBuffer(0, MAXHTTPHEADLEN);
		m_HttpHead.m_HttpMethod = HttpMethod_INIT;
		return 0;
	}

	if (m_nContentLen)
	{
		if (m_nHasRecvBodyLen > 0)
		{
			nRet = ProcessHttpBody((char*)(pHttpHeadEnd + 4), m_nHasRecvBodyLen);
			if (nRet != 0)
			{
				return -1;
			}
		}

		if (m_nHasRecvBodyLen == m_nContentLen)
		{
			ResetRecvBuffer(0, MAXHTTPHEADLEN);
			m_HttpHead.m_HttpMethod = HttpMethod_INIT;
			m_State = ReadHead;
		}
		else
		{
			ResetRecvBuffer(0, RECVBODYMAXLEN);
		}
	}
	else
	{
		ResetRecvBuffer(0, MAXHTTPHEADLEN);
		m_HttpHead.m_HttpMethod = HttpMethod_INIT;
		m_State = ReadHead;
	}
	
	return 0;
}

int CHttpSendAndRecvTask::ReadHttpBody()
{
	int nRestBodyLen = m_nContentLen - m_nHasRecvBodyLen;
	nRestBodyLen = (nRestBodyLen > (int)RECVBODYMAXLEN)? (int)RECVBODYMAXLEN:nRestBodyLen;
	if (nRestBodyLen <= 0)
	{
		WriteRunInfo::WriteError("CHttpSendAndRecvTask::ReadHttpBody : nRestBodyLen invalid"); 
		return -1;
	}
	int nRecvLen = recv(GetSocket(), &m_szRecvBuf[0], nRestBodyLen, MSG_DONTWAIT);
	if (nRecvLen == 0)
	{
		WriteRunInfo::WriteTrace("CHttpSendAndRecvTask::ReadHttpBody : recv client close , close the socket");
		return -1;
	}

	if (nRecvLen < 0)
	{
		if (errno != EINTR && errno != EAGAIN)
		{
			WriteRunInfo::WriteLog("CHttpSendAndRecvTask::ReadHttpBody : recv error(%d) ", errno);
			return -1;
		}
		return 0;
	}

	m_nHasRecvBodyLen += nRecvLen;
	return ProcessHttpBody((char*)(&m_szRecvBuf[0]), nRecvLen);	
}

int CHttpSendAndRecvTask::ProcessHttpHead()
{
	if (m_nHttpRetCode == 301 || m_nHttpRetCode == 302)	
	{
		string strHost;
		unsigned short nPort = m_HttpHead.m_nPort;
		string strTmp;
		int nTmp = CToolKit::GetStringByLabel(m_strRetHttpHead, "Location:", "\r\n", strTmp);
		if (nTmp == 0)
		{
			CToolKit::StringTrim(strTmp);
			size_t nBeginPos = strTmp.find("//");		
			if (nBeginPos != string::npos)
			{
				strTmp = strTmp.substr(nBeginPos + 2);	
			}
			else
			{
				return -1;
			}
			size_t nEndHostPortPos = strTmp.find("/");
			if (nEndHostPortPos != string::npos)
			{
				m_HttpHead.m_strFilePath = strTmp.substr(nEndHostPortPos);				
				strTmp = strTmp.substr(0, nEndHostPortPos);
			}
			else
			{
				m_HttpHead.m_strFilePath = "/";
			}

			size_t nSplitPos = strTmp.find(":");
			if (nSplitPos == string::npos)
			{
				strHost = strTmp;
			}
			else
			{
				strHost = strTmp.substr(0, nSplitPos);
				nPort = (unsigned short)atoi(strTmp.substr(nSplitPos + 1).c_str());
			}

			string strIp;
			if (CLocalDomainParser::GetInstance()->ParseDomain(strHost, strIp) != 0)
			{
				return -1;
			}

			m_bIfConnectSuc = false;
			DelFromEpoll();
			delete m_pTcpSocket;
			m_pTcpSocket = NULL;
			m_HttpHead.m_strIp = strIp;
			m_HttpHead.m_nPort = nPort;
			ConnectServer();

			ResetRecvBuffer(0, MAXHTTPHEADLEN);
			m_State = ReadHead;
			SendRequest();
			return 1;
		}
	}
	return 0;
}

int CHttpSendAndRecvTask::Process(CMsg* pMsg)
{
	return 0;
}

int CHttpSendAndRecvTask::GenHttpHead()
{
	m_HttpHead.m_strHttpHead = "";
	string strMethod;
	if (m_HttpHead.m_HttpMethod == HttpMethod_HEAD)
	{
		strMethod = "HEAD";
	}
	else if (m_HttpHead.m_HttpMethod == HttpMethod_GET)
	{
		strMethod = "GET";
	}
	else if (m_HttpHead.m_HttpMethod == HttpMethod_POST)
	{
		strMethod = "POST";
	}
	else
	{
		WriteRunInfo::WriteError("CHttpSendAndRecvTask::SendRequest : unknow method");
		return -1;
	}
	m_HttpHead.m_strHttpHead += strMethod + " " + m_HttpHead.m_strFilePath + " " + "HTTP/1.1\r\n";
	m_HttpHead.m_strHttpHead += "Accept: */* \r\n";
	m_HttpHead.m_strHttpHead += "Accept-Language: zh-cn \r\n"; 
	if (m_HttpHead.m_strHost.length())
	{
		m_HttpHead.m_strHttpHead += "Host: " + m_HttpHead.m_strHost + "\r\n";
	}
	m_HttpHead.m_strHttpHead += "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0;windows NT 5.1; SV1)\r\n";
	m_HttpHead.m_strHttpHead += "Connection: keep-alive\r\n";
	if (m_HttpHead.m_nContentLen > 0)
	{
		char szBuf[20];
		snprintf(szBuf, sizeof(szBuf), "%llu", m_HttpHead.m_nContentLen);
		m_HttpHead.m_strHttpHead += "Content-Length: " + string(szBuf) + "\r\n";
	}

	if (m_HttpHead.m_strHttpHeadUserAppend.length())
	{
		m_HttpHead.m_strHttpHead += m_HttpHead.m_strHttpHeadUserAppend;
	}
	m_HttpHead.m_strHttpHead += "\r\n";
	return 0;
}

int CHttpSendAndRecvTask::SetUrl(const THttpHead& HttpHead)
{
	m_HttpHead = HttpHead;
	return 0;
}

int CHttpSendAndRecvTask::SendRequest()
{
	int nRet = GenHttpHead();
	if (nRet != 0)
	{
		return -1;
	}
	int nBufLen = m_HttpHead.m_strHttpHead.length() + m_HttpHead.m_strPostData.length();
	char* pBuf = new char[nBufLen];	
	memcpy(pBuf, m_HttpHead.m_strHttpHead.c_str(), m_HttpHead.m_strHttpHead.length());
	if (m_HttpHead.m_strPostData.length())
	{
		memcpy(pBuf + m_HttpHead.m_strHttpHead.length(), m_HttpHead.m_strPostData.c_str(), m_HttpHead.m_strPostData.length());
	}
	PutMsgToSendList(pBuf, nBufLen);
	return 0;
}

int CHttpSendAndRecvTask::PutMsgToSendList(const char* pBuf, int nLen)
{
	if (pBuf == NULL || nLen == 0)
	{
		return -1;
	}

	CSendMsg* pSendMsg = new CSendMsg(nLen, (char*)pBuf);
	m_sendList.push_back(pSendMsg);
	ModifyEpollAddWrite();
	return 0;
}

int CHttpSendAndRecvTask::CheckTimeOut()
{
	int nCurTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
	bool bIfTimeout = false;
	if ((nCurTime - m_nStartTime) >= m_nTcpSocketTimeout)
	{
		bIfTimeout = true;
	}

	if (bIfTimeout)
	{
		CMyserver::GetInstance()->GetCommonStat().AddNormalStat(SOCKETTIMEOUTSTAT);
		m_nErrorNum = EHSR_Timeout;
		DestroySelf();
		return 1;
	}
	return 0;
}









