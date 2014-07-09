#include "mylongconnecttask.h"
#include "insertrctoolsdef.h"

CMyLongConnectTask::CMyLongConnectTask(CTcpSocket* pSocket, CNetProcessThread* pWorkThread, unsigned char ucMachineType, unsigned short usMachineIndex, bool bActive, int nListType)
	: CCommonTask (pSocket, pWorkThread, ucMachineType, usMachineIndex, bActive, nListType)
{
}

CMyLongConnectTask::~CMyLongConnectTask()
{
}

int CMyLongConnectTask::ProcessPacket(PSERVERCOMMANDPACKET pServerCommandPacket)
{
	if (NULL == pServerCommandPacket)
	{
		return -1;
	}


	/*TMsg tMsg;
	tMsg.destEntityType = RECVANDSENDMSGENTITY;
	tMsg.destEntityId = ( int )ntohl( pServerCommandPacket->iDestProcessId );
	tMsg.srcEntityId = 0;
	tMsg.srcEntityType= LONGCONNECTEDENTITY;
	tMsg.srcTaskId = GetTaskId();
	tMsg.taskId = ( unsigned long long )ntohll( pServerCommandPacket->ullDestTaskId );
	tMsg.msgType = CLIENTMSGTYPE;

	int iLength = ( int )ntohl( pServerCommandPacket->iMsgLen);
	char *pTmpBuf = new char[iLength + EXTRA_BUFLEN_FOR_AES];   //加密扩展预留长度
	memcpy( pTmpBuf , pServerCommandPacket->serverCommandPacketContent , iLength );

	CMsgBody * pMsgBody = new CClientMsgBody( pTmpBuf , iLength );

	CMsg *pMsg = new CMsg( tMsg, pMsgBody );

	CMyserver::GetInstance()->PutMsgToThread( pMsg );*/

	return 0;
}




