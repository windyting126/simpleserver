#ifndef __MY_LONG_CONNECT_TASK_H
#define __MY_LONG_CONNECT_TASK_H

#include "CommonTask.h"

class CMyLongConnectTask
	: public CCommonTask
{
public:
	CMyLongConnectTask(CTcpSocket* pSocket, CNetProcessThread* pWorkThread, unsigned char ucMachineType, unsigned short usMachineIndex, bool bActive, int nListType);
	~CMyLongConnectTask();

protected:
	virtual int ProcessPacket(PSERVERCOMMANDPACKET pServerCommandPacket);
};




#endif


