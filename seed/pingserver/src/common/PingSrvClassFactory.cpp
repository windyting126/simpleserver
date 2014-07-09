/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingSrvClassFactory.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-04
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-04			1.0			
* =======================================================================
*/
#include "PingSrvClassFactory.h"


#include "PingCmd.h"
#include "LogoutCmd.h"

#include "LongConnectTask.h"
#include "PingLongConnectThread.h"
#include "OntimeSynThread.h"
#include "TimeoutCheckThread.h"
#include "UdpProcAndSendThread.h"
#include "UdpRecvThread.h"
#include "UdpMSendThread.h"
#include "ConfigUpdateThread.h"
#include "PingServerDef.h"
#include "ExternalCheckCmd.h"
#include "pingpeerhashtable.h"

#include "PingServer.h"


CSocketTask* CPingSrvClassFactory::GenSocketTask(CNetProcessThread *pWorkThread, CTcpSocket *pTcpSocket, int iTaskType, CTaskArg * pTaskArg/* = NULL*/)
{
	CSocketTask *pSocketTask = NULL;
	switch(iTaskType)
	{
	case COMMONTASK:
		{
			if (pTaskArg)
			{
				CCommonTaskArg *pCommonTaskArg = (CCommonTaskArg *)pTaskArg;
				pSocketTask = new CLongConnectTask(pTcpSocket, pWorkThread, pCommonTaskArg->GetDestMachineType(), 
					pCommonTaskArg->GetDestMachineIndex(), pCommonTaskArg->IsActive(), pCommonTaskArg->GetListType());
			}
			break;
		}
	default:
		return CClassFactory::GenSocketTask(pWorkThread, pTcpSocket, iTaskType);
	}          
	return pSocketTask;
}

CWorkThread* CPingSrvClassFactory::GenWorkThread(int nEntityType, int nEntityId, string sAnnexData, void *arg)
{
	CWorkThread *pWorkThread = NULL;
	CPingServer *pServer = (CPingServer *)CMyserver::GetInstance();
	if (!pServer)
	{
		return pWorkThread;
	}
	CBaseConf &BaseConf = pServer->GetBaseConf();

	switch(nEntityType)
	{
	case LONGCONNECTEDENTITY:
		{
			pWorkThread = new CPingLongConnectThread(nEntityType, nEntityId);
			break;
		}
	case UDPSENDANDPROCESSENTITY:
		{
			pWorkThread = new CUdpProcAndSendThread(nEntityType, nEntityId);
			break;
		}
	case UDPRECVENTITY:
		{			
			pWorkThread = new CUdpRecvThread(nEntityType, nEntityId, BaseConf.GetConfigItem().sServerAddr, BaseConf.GetConfigItem().nUdpPort);
			break;
		}
	case PINGONTIMESYN_ENTITY_TYPE:
		{

			pWorkThread = new COntimeSynThread(nEntityType, nEntityId);
			break;
		}
	case CHECKTIMEOUT_ENTITY_TYPE:
		{
			pWorkThread = new CTimeoutCheckThread(nEntityType, nEntityId);
			break;
		}
	case UDP_MSEND_ENTITY_TYPE:
		{
            int nPort;
            memcpy(&nPort, sAnnexData.c_str(), 4);
            string strHost = sAnnexData.substr(4);
            pWorkThread = new CUdpMSendThread(nEntityType, nEntityId, strHost, nPort);			
			break;
		}
	case CONFIG_UPDATE_ENTITY_TYPE:
		{
			pWorkThread = new CConfigUpdateThread(nEntityType, nEntityId);
			break;
		}
	default:
		return CClassFactory::GenWorkThread( nEntityType , nEntityId , sAnnexData , arg );
	}          
	return pWorkThread;
}

CCommand* CPingSrvClassFactory::GenBaseCommand(char *pBuf, int nLen, int nClientIp, short nClientPort, int nEntityType, int nEntityId)
{
	CCommand *pCmd = NULL;
	if(nLen < PINGSERVER_MSG_HEAD_LEN) 
	{
		WriteRunInfo::WriteLog("The cmd buf len is less than %d, "
			"too short", PINGSERVER_MSG_HEAD_LEN);
		return pCmd;
	}

	unsigned char sCmdId = CPingSrvBaseCommand<CDefault,CDefaultRes>::GetCommandId(pBuf, nLen);

	char *tmpBuf = new char[nLen];
	memcpy(tmpBuf, pBuf, nLen);

	switch(sCmdId)
	{
	case PINGSERVER_CMD_PING:
		{                
			pCmd = new CPingCmd(tmpBuf, nLen, nClientIp, nClientPort, nEntityType, nEntityId);
			break;
		}
	case PINGSERVER_CMD_LOGOUT:
		{
			pCmd = new CLogoutCmd(tmpBuf, nLen, nClientIp, nClientPort, nEntityType, nEntityId);
			break;
		}
	default:
		{
			WRITE_TRACE("invalid cmd type %d", sCmdId);
			delete [] tmpBuf;
			break;
		}
	}
	if (pCmd != NULL)
	{
		WRITE_TRACE("Recv from client a cmd(%u)", sCmdId);
	}
	return pCmd;
}


CBaseMaintainCmd* CPingSrvClassFactory::GenMaintainCmd(int nMsgType, int nBufLen, char *pBuf)
{
	CBaseMaintainCmd* pCmd = NULL;
	if (nMsgType == ONLINE_CHECK_PEERSTORAGEINFO || nMsgType == ONLINE_CHECK_PEERINFO 
		|| nMsgType == ONLINE_DUMP_HASHTABLE || nMsgType == ONLINE_LOAD_HASHTABLE)
	{
		pCmd = new CExternalCheckCmd(nMsgType, nBufLen, pBuf);
	}
	else
	{
		pCmd = CClassFactory::GenMaintainCmd(nMsgType, nBufLen, pBuf);
	}
	return pCmd;
}


