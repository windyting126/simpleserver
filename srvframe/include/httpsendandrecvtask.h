/*
 * =====================================================================================
 *
 *       Filename:  httpsendandrecvtask.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年04月12日 15时12分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wangtiefeng (), wangtf418@163.com
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _HTTPSENDANDRECVTASK_H_B61CCBB6_2234_4FCE_9250_5A63F21564AE_
#define _HTTPSENDANDRECVTASK_H_B61CCBB6_2234_4FCE_9250_5A63F21564AE_

#include "framecommon/framecommon.h"
#include "recvandsendtask.h"
#include "netprocessthread.h"

const int HTTP_SENDANDRECV_TASK_TIMEOUT = 5;

enum EHttpMethod
{
	HttpMethod_INIT = 0,
	HttpMethod_HEAD = 1,
	HttpMethod_GET = 2,
	HttpMethod_POST = 3
};

struct THttpHead
{
	EHttpMethod m_HttpMethod;
	string m_strFilePath;
	string m_strHost;
	string m_strIp;
	unsigned short m_nPort;
	unsigned int long long m_nContentLen;
	string m_strPostData;
	string m_strHttpHeadUserAppend;

	string m_strHttpHead;

	THttpHead()
	{
		m_HttpMethod = HttpMethod_INIT;
		m_nPort = 0;
		m_nContentLen = 0;
		m_strFilePath = "/";
	}
};

class CHttpSendAndRecvTask : public CRecvAndSendTask
{
public:
	CHttpSendAndRecvTask(CTcpSocket* pSocket, CNetProcessThread* pWorkThread, int nTaskTimeout = HTTP_SENDANDRECV_TASK_TIMEOUT);
	virtual ~CHttpSendAndRecvTask();
public:
	virtual int DestroySelf();
	virtual int Init();
	virtual int HandleProcess(int nEvent);
	virtual int CheckTimeOut();
	virtual int Process(CMsg* pMsg);
	int SetUrl(const THttpHead& HttpHead);
protected:
	virtual int SendRequest();
	virtual int PutMsgToSendList(const char* pBuf, int nLen);
	virtual int ConnectServer();
protected:
	virtual int RecvProcess();
	virtual int SendProcess();
protected:
	virtual int ReadHttpHead();
	virtual int ReadHttpBody();
	virtual int ProcessHttpHead();
	virtual int ProcessHttpBody(char* pBuf, int nBufLen) = 0;
protected:
	virtual int AddToEpoll();
	int ModifyEpollAddWrite();
	int ModifyEpollDelWrite();

	virtual int GenHttpHead();
	void ResetRecvBuffer(unsigned int uReceivePos, unsigned int uReceiveSize);
protected:
	enum EState
	{
		ReadHead = 0,
		ReadBody = 1
	};
	enum EError
	{
		EHSR_OK = 0,
		EHSR_ConnectFail = 1,
		EHSR_Epoll = 2,
		EHSR_RecvFail = 3,
		EHSR_SendFail = 4,
		EHSR_Timeout = 5,
		EHSR_Exception = 6,
		EHSR_Finish = 7
	};
protected:
	unsigned int m_uReceivePos;	
	unsigned int m_uReceiveSize;
	EState m_State;
	THttpHead m_HttpHead;
protected:
	int m_nSrcEntityType;
	int m_nSrcEntityId;
	long long m_nSrcTaskId;

protected:
	char m_szRecvBuf[RECVBODYMAXLEN];
	bool m_bIfConnectSuc;
	int m_nErrorNum;
	unsigned long long m_nContentLen;
	unsigned long long m_nHasRecvBodyLen;
	int m_nHttpRetCode;
	bool m_bIfClose;
	string m_strRetHttpHead;

protected:
	int m_nTcpSocketTimeout;
};

#endif




