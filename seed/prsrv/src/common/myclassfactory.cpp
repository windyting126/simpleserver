/*
 * FileName:       
 * Author:         wangqifeng  Version: 1.0  Date: 2009-1-4
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version >   <desc>
 */
#include "myclassfactory.h"
#include "LongConnectedListenThread.h"
#include "LongConnectedProcessThread.h"
#include "timerthread.h"
#include "dbupdatethread.h"
#include "udpmrecvthread.h"
#include "pingsynprocessthread.h"
#include "CommonTask.h"
#include "LongConnectedListenTask.h"
#include "RServerRecvAndSendTask.h"
#include "rserver.h"
#include "externalcheckcmd.h"

#include "isrconlinecmd.h"
#include "insertrccmd.h"
#include "reportrclistcmd.h"
#include "deleterclistcmd.h"
#include "deleterclistnewcmd.h"


CSocketTask *CMyClassFactory::GenSocketTask(CNetProcessThread *pWorkThread, CTcpSocket *pTcpSocket,
        int iTaskType,CTaskArg * pTaskArg/* = NULL*/)
{
	CSocketTask *pSocketTask = NULL;

	switch( iTaskType )
	{
	case COMMONTASK:
		{
			int iUploadInterval = ((CPRServer*)(CMyserver::GetInstance()))->GetMyConf().GetUploadInterval();
			if ( pTaskArg )
			{
				CCommonTaskArg * pCommonTaskArg = ( CCommonTaskArg * )pTaskArg;
				pSocketTask = new RServerRecvAndSendTask( pTcpSocket, pWorkThread, pCommonTaskArg->GetDestMachineType(), 
					pCommonTaskArg->GetDestMachineIndex(), pCommonTaskArg->IsActive(),
					pCommonTaskArg->GetListType(), iUploadInterval);
			}
			break;
		}
	case LONGCONNECTEDLISTENTASK:
		{
			pSocketTask = new CLongConnectedListenTask( pTcpSocket, pWorkThread );
			break;
		}
	default:
		return CClassFactory::GenSocketTask(pWorkThread, pTcpSocket, iTaskType);
	}  

	return pSocketTask;
}

CWorkThread *CMyClassFactory::GenWorkThread(int nEntityType, int nEntityId, string sAnnexData, 
						   void *arg/*=NULL*/)
{
	CWorkThread *pWorkThread = NULL;
	switch(nEntityType)
	{
	case LONGCONNECTEDLISTENENTITY:
		{

			int nPort;
			memcpy(&nPort, sAnnexData.c_str(), 4);
			string sHost = sAnnexData.substr(4); 
			//TListenBlock *pListenBlock = NULL;
			LISTENBLOCK *pListenBlock = NULL;
			if (arg == NULL)
				pListenBlock = new LISTENBLOCK;//TListenBlock;
			else
				pListenBlock = (LISTENBLOCK*)arg;//(TListenBlock*)arg;

			pWorkThread = new CLongConnectedListenThread(nEntityType, nEntityId, sHost, nPort, pListenBlock);

			break;
		}
	case LONGCONNECTEDPROCESSENTITY:
		{
			pWorkThread = new CLongConnectedProcessThread(nEntityType, nEntityId);
			break;
		}
	case MAINTAINENTITY:
		{
			pWorkThread = new CDBUpdateThread(nEntityType, nEntityId);
			break;
		}
	case UDP_MRECV_ENTITY_TYPE:
		{
			int nPort;
			memcpy(&nPort, sAnnexData.c_str(), 4);
			string sHost = sAnnexData.substr(4); 
			pWorkThread = new CUdpMRecvThread(nEntityType, nEntityId, sHost, nPort);
			break;
		}
	case PING_SYN_PROCESS_ENTITY_TYPE:
		{
			pWorkThread = new CPingSynProcessThread(nEntityType, nEntityId);
			break;
		}
	default:
		{                        
			pWorkThread = CClassFactory::GenWorkThread( nEntityType, nEntityId, sAnnexData, arg );
		}
	}          

	return pWorkThread;
}

CBaseMaintainCmd * CMyClassFactory::GenMaintainCmd( int nMsgType, int nBufLen, char *pBuf )
{
	CBaseMaintainCmd *pCmd = NULL;
	if ( nMsgType == CMD_DEBUGINFO || nMsgType == PRINTSTATINFO )
	{
		pCmd = new CBaseMaintainCmd( nMsgType, nBufLen, pBuf );
	}
	else if ( nMsgType == ONLINE_CHECK_PEERINFO || nMsgType == ONLINE_CHECK_PEERSTORAGEINFO || nMsgType == ONLINE_DUMP_HASHTABLE)
	{
		pCmd = new CExternalCheckCmd( nMsgType, nBufLen, pBuf );
	}

	return pCmd;
}

CCommand* CMyClassFactory::GenBaseCommand(char* pBuf, int nLen, int nIp)
{
	CCommand* pCmd = NULL;
	int nCmdId = CSeedBaseCmd<int>::GetCommandId(pBuf, nLen);
	WriteRunInfo::WriteTrace("Recv PConSrv CmdId:[%d]", nCmdId);
	switch (nCmdId)
	{
	case SEEDCMD_ISRC_ONLINE:
		{
			char* pInBuf = new char[nLen];
			memcpy (pInBuf, pBuf, nLen);
			pCmd = new CIsRcOnlieCmd(pInBuf, nLen);
			pCmd->SetIp(nIp);
			break;
		}
	case SEEDCMD_INSERT_RC:
		{
			char* pInBuf = new char[nLen];
			memcpy (pInBuf, pBuf, nLen);
			pCmd = new CInsertRcCmd(pInBuf, nLen);
			pCmd->SetIp(nIp);
			break;
		}
	case SEEDCMD_REPORT_RCLIST:
		{
			char* pInBuf = new char[nLen];
			memcpy (pInBuf, pBuf, nLen);
			pCmd = new CReportRcListCmd(pInBuf, nLen);
			pCmd->SetIp(nIp);
			break;
		}
	case SEEDCMD_DELETE_RCLIST:
		{
			char* pInBuf = new char[nLen];
			memcpy (pInBuf, pBuf, nLen);
			pCmd = new CDeleteRcListCmd(pInBuf, nLen);
			pCmd->SetIp(nIp);
			break;
		}
	case SEEDCMD_DELETE_RCLISTNEW:
		{
			char* pInBuf = new char[nLen];
			memcpy (pInBuf, pBuf, nLen);
			pCmd = new CDeleteRcListNewCmd(pInBuf, nLen);
			pCmd->SetIp(nIp);
			break;
		}
	default:
		{
			WriteRunInfo::WriteError ("CMyClassFactory::GenBaseCommand : invalid cmd type %d", nCmdId);
			break;
		}
	}
	return pCmd;
}

