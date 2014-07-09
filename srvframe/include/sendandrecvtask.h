/*
 * =====================================================================================
 *
 *       Filename:  sendandrecvtask.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年06月29日 15时15分11秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wangtiefeng (), wangtf418@163.com
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _SENDANDRECVTASK_H_D81840F9_7C1D_4A95_B0CA_1E67110779BE_
#define _SENDANDRECVTASK_H_D81840F9_7C1D_4A95_B0CA_1E67110779BE_

#include "recvandsendtask.h" 
#include "netprocessthread.h" 

const int SENDANDRECV_TASK_TIMEOUT = 5; 

struct TDstServerInfo
{
	string m_strIp;
	unsigned short m_nPort;
};

class CSendAndRecvTask : public CRecvAndSendTask
{
public:
	CSendAndRecvTask(CTcpSocket* pSocket, CNetProcessThread* pWorkThread, int nTaskTimeout = SENDANDRECV_TASK_TIMEOUT);
	virtual ~CSendAndRecvTask();
public:
	virtual int DestroySelf();
	virtual int Init();
	virtual int HandleProcess(int nEvent);
	virtual int CheckTimeOut();
	virtual int Process(CMsg* pMsg);
	virtual int SetServerInfo(TDstServerInfo& DstServerInfo);
protected:
	virtual int PutMsgToSendList(const char* pBuf, int nLen);
	virtual int ConnectServer();
protected:
	virtual int RecvProcess();
	virtual int SendProcess();
	virtual int ProcessHead() = 0;
	virtual int ProcessBody() = 0;
protected:
	virtual int ProcessPacket() = 0;
protected:
	virtual int AddToEpoll();
	int ModifyEpollAddWrite();
	int ModifyEpollDelWrite();
	void ResetRecvBuffer(unsigned int uReceivePos, unsigned int uReceiveSize);
protected:
	enum EState
	{
		ReadHead = 0,
		ReadBody = 1
	};
	enum EError
	{
		ESR_OK = 0,	
		ESR_ConnectFail = 1,
		ESR_Epoll = 2,
		ESR_RecvFail = 3,
		ESR_SendFail = 4,
		ESR_Timeout = 5,
		ESR_Exception = 6,
		ESR_Finish = 7 
	};
protected:
	TDstServerInfo m_DstServerInfo;
	unsigned int m_uReceivePos;
	unsigned int m_uReceiveSize;
	EState m_State;
protected:
	bool m_bIfConnectSuc;
	int m_nErrorNum;
	char* m_pRecvBuf;
protected:
	int m_nSrcEntityType;
	int m_nSrcEntityId;
	long long m_nSrcTaskId;
protected:
	int m_nTcpSocketTimeout;
};


#endif

