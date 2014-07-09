#ifndef __LONGCONNLISTENTASK_H
#define __LONGCONNLISTENTASK_H

#include "sockettask.h"
#include "framecommon/framecommon.h"
using namespace std;

class CLongConnListenTask : public CSocketTask
{
public:
	CLongConnListenTask(CTcpSocket *pSocket, CNetProcessThread* pWorkThread, int nRecvAndSendThreadType = ENTITYTYPE_LONGCONN_RECVANDSEND);
	~CLongConnListenTask();
public:
	virtual int Init();
	int Process(CMsg *pMsg);        
	int HandleProcess(int event);
protected:
	int DestroySelf();
	int PutMsgToSendList(const char* pBuf, int nLen);		
	int AddToEpoll();
	int m_nRecvAndSendThreadType;
};
#endif

