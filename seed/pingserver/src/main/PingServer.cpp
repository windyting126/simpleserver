/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingServer.h
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
#include "PingServer.h"
#include "PingSrvClassFactory.h"
#include "commmsg.h"
#include "CmdCounter.h"
#include "PingServerDef.h"
#include "PingLongConnectThread.h"
#include "ConfigUpdateTask.h"


CPingServer::CPingServer() : CMyserver()
{
	m_pTimerThreadGrp = NULL;
	m_pMaintainThreadGrp = NULL;
	m_pMaintainUpdThread = NULL;
	m_pUdpMSendThreadGrp = NULL;
	m_pLongConnectThreadGrp = NULL;
	m_pUdpRecvThreadGrp = NULL;
	m_pUdpSendThreadGrp = NULL;
	m_pOntimeSynThreadGrp = NULL;
	m_pTimeoutCheckThreadGrp = NULL;
	m_pConfigUpdateThreadGrp = NULL;
	
	m_pPeerHashTable = NULL;
	sem_init(&m_semSynSend, 0, 0);
}

CPingServer::~CPingServer()
{
	sem_destroy(&m_semSynSend);
}

int CPingServer::InitServer()
{
	int iRet = CMyserver::InitServer();
	if (iRet != 0)
		return -1;
	iRet = 	m_PingSrvConf.Init();
	if (iRet != 0)
	{
		WriteRunInfo::WriteLog("Load conf fail ...");
		return -1;
	}
	CConfigUpdateTask::GetInstance().Init();

	iRet = CPingPeerHashTable::CreateHashTable(
		m_PingSrvConf.GetConf().nHashBucketLen,
		m_PingSrvConf.GetConf().nClientMaxUploadConns,
		m_PingSrvConf.GetConf().fUploadConnsBusyPercent,
		m_PingSrvConf.GetConf().fUploadConnsNormalPercent);
	if (iRet != 0)
	{
		WriteRunInfo::WriteLog("Create Peer Hash Table fail ...");
		return -1;
	}
	m_pPeerHashTable = CPingPeerHashTable::GetHashTable();
	if (!m_pPeerHashTable)
	{
		WriteRunInfo::WriteLog("Get Peer Hash Table fail ...");
		return -1;
	}
	iRet = m_pPeerHashTable->Init();
	if (iRet != 0)
	{
		WriteRunInfo::WriteLog("Init PeerHashTable fail ...");
		return -1;
	}
	
	CCmdCounter::Create();
	CCmdCounter::GetHandler()->Init("Cmd", MAXCMDINDEX);
	
	m_pClassFactory = new CPingSrvClassFactory;
	return 0;
}


int CPingServer::StartServer()
{
	int iResult = 0;

	////timer thread//////////////////////////////////////////////////////////////////////////
	m_pTimerThreadGrp = new CThreadGroup(1, TIMERENTITY);
	iResult = m_pTimerThreadGrp->Init();
	if ( iResult != 0 )
	{       
		WriteRunInfo::WriteLog("Init timer thread fail");
		return -1;
	}

	////Start Maintain Thread///////////////////////////////////////////////////////////////
	m_pMaintainThreadGrp = new CThreadGroup(1, MAINTAINENTITY);
	iResult = m_pMaintainThreadGrp->Init();
	if (iResult != 0)
	{       
		WriteRunInfo::WriteLog("%s:%d:%s : Init MAINTAINENTITY fail!", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}       
	WriteRunInfo::WriteLog("Start MAINTAINENTITY succeed!");


	////Stat Maintain UDP Thread///////////////////////////////////////////////////////////
	m_pMaintainUpdThread = new CMaintainUdpThread(m_baseConf.GetConfigItem().sMaintainIp, m_baseConf.GetConfigItem().nMaintainPort);
	iResult = m_pMaintainUpdThread->Start();
	if ( iResult != 0 )
	{
		WriteRunInfo::WriteLog( "%s:%d:%s : Init MAINTAINUDPENTITY fail!" , __FILE__ , __LINE__ , __FUNCTION__ );
		return -1;
	}        
	WriteRunInfo::WriteLog("Start MAINTAINUDPENTITY succeed!"); 

	////Start UDP multicast send thread/////////////////////////////////////////////////////////
	if (m_PingSrvConf.GetConf().bUseMulticast)
	{
		int nPort = m_PingSrvConf.GetConf().nMSendPort;
		string strHost= "";
		string strTmpData = string((char*)(&nPort), 4);
		strTmpData.append(strHost);		
		m_pUdpMSendThreadGrp = new CThreadGroup(1, UDP_MSEND_ENTITY_TYPE, strTmpData);
		iResult = m_pUdpMSendThreadGrp->Init();
		if (iResult != 0)
		{       
			WriteRunInfo::WriteError("%s:%d:%s : Init UDP_MSEND_ENTITY_TYPE Failed!", __FILE__, __LINE__, __FUNCTION__);
			return -1;
		}
		WriteRunInfo::WriteLog("Start UDP_MSEND_ENTITY_TYPE succeed!");
	}
	
	////Start Long Connect thread/////////////////////////////////////////////////////////
	m_pLongConnectThreadGrp = new CThreadGroup(1, LONGCONNECTEDENTITY);	
	iResult = m_pLongConnectThreadGrp->Init();
	if (iResult != 0)
	{       
		WriteRunInfo::WriteError("%s:%d:%s : Init LONGCONNECTEDENTITY Failed!", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	WriteRunInfo::WriteLog("Start LONGCONNECTEDENTITY succeed!");

	////Send building long connect message
	vector<TSeedServer> vecSrv;
	vecSrv.clear();
	m_PingSrvConf.GetQuerySrv(vecSrv);
	for ( int iA = 0; iA < (int)vecSrv.size(); iA ++ )
	{
		WriteRunInfo::WriteLog( "Send Connect to Machine %d" , iA );
		SendConnectMsg(vecSrv[iA].strIp.c_str(), vecSrv[iA].nPort, QSERVERMACHINETYPE, iA);
	}
	vecSrv.clear();
	m_PingSrvConf.GetResourceSrv(vecSrv);	
	for ( int iA = 0; iA < (int)vecSrv.size(); iA ++ )
	{
		WriteRunInfo::WriteLog( "Send Connect to Machine %d" , iA );
		SendConnectMsg(vecSrv[iA].strIp.c_str(), vecSrv[iA].nPort, RSERVERMACHINETYPE, iA);		
	}

	////Start Udp Send and Process thread///////////////////////////////////////////////
	m_pUdpSendThreadGrp = new CThreadGroup((int)m_PingSrvConf.GetConf().nUdpProcAndSendThreadNum, UDPSENDANDPROCESSENTITY);
	iResult = m_pUdpSendThreadGrp->Init();
	if(iResult != 0)
	{
		WriteRunInfo::WriteError("%s:%d:%s : Init UDPSENDANDPROCESSENTITY Failed!", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	WriteRunInfo::WriteLog("Start UDPSENDANDPROCESSENTITY succeed!");

	////Start Udp Recv Thread/////////////////////////////////////////////////////////
	m_pUdpRecvThreadGrp = new CThreadGroup(1, UDPRECVENTITY);
	iResult = m_pUdpRecvThreadGrp->Init();
	if(iResult != 0)
	{
		WriteRunInfo::WriteError( "%s:%d:%s : Init UDPRECVENTITY Failed!" , __FILE__ , __LINE__ , __FUNCTION__ );
		return -1;
	}
	WriteRunInfo::WriteLog("Start UDPRECVENTITY succeed!");

	////Start Ontime Syn Thread/////////////////////////////////////////////////////
	m_pOntimeSynThreadGrp = new CThreadGroup(1, PINGONTIMESYN_ENTITY_TYPE);
	iResult = m_pOntimeSynThreadGrp->Init();
	if(iResult != 0)
	{
		WriteRunInfo::WriteError( "%s:%d:%s : Init PINGSYN_ENTITY_TYPE Failed!" , __FILE__ , __LINE__ , __FUNCTION__ );
		return -1;
	}
	WriteRunInfo::WriteLog("Start PINGSYN_ENTITY_TYPE succeed!");

	////Start Time Check Thread////////////////////////////////////////////////////
	m_pTimeoutCheckThreadGrp = new CThreadGroup(1, CHECKTIMEOUT_ENTITY_TYPE);
	iResult = m_pTimeoutCheckThreadGrp->Init();
	if(iResult != 0)
	{
		WriteRunInfo::WriteError( "%s:%d:%s : Init CHECKTIMEOUT_ENTITY_TYPE Failed!" , __FILE__ , __LINE__ , __FUNCTION__ );
		return -1;
	}
	WriteRunInfo::WriteLog("Start CHECKTIMEOUT_ENTITY_TYPE succeed!");

	////Start config update Thread////////////////////////////////////////////////////
	m_pConfigUpdateThreadGrp = new CThreadGroup(1, CONFIG_UPDATE_ENTITY_TYPE);
	iResult = m_pConfigUpdateThreadGrp->Init();
	if(iResult != 0)
	{
		WriteRunInfo::WriteError( "%s:%d:%s : Init CONFIG_UPDATE_ENTITY_TYPE Failed!" , __FILE__ , __LINE__ , __FUNCTION__ );
		return -1;
	}
	WriteRunInfo::WriteLog("Start CONFIG_UPDATE_ENTITY_TYPE succeed!");
	
	return  0;
}

int CPingServer::SendConnectMsg(const char *pcszRemoteIp, unsigned short usRemotePort, 
								  unsigned char ucMachineType, unsigned short usMachineIndex)
{
	TMsg tMsg;
	tMsg.destEntityType = LONGCONNECTEDENTITY;
	tMsg.destEntityId = 0;
	tMsg.srcEntityId = 0;
	tMsg.srcEntityType= MAINENTITY;
	tMsg.srcTaskId = 0;
	tMsg.taskId = 0;
	tMsg.msgType = TOCONNECTMSGTYPE;
	CMsgBody * pMsgBody = new CToConnectMsgBody(pcszRemoteIp, usRemotePort, ucMachineType, usMachineIndex);
	if (!pMsgBody) 
	{
		return -1;
	}
	CMsg *pMsg = new CMsg(tMsg, pMsgBody);
	if (!pMsg)
	{
		delete pMsgBody;
		return -1;
	}
	CMyserver::GetInstance()->PutMsgToThread(pMsg);
	return 0;
}

void CPingServer::WriteSendListLog(int nLogType)
{
	stringstream statStr;
	statStr.str("");
	statStr << "<<server sendlist len>>:\n";	

	statStr << "\t<<EntityType:" << LONGCONNECTEDENTITY << ">>:\n";
	vector<CWorkThread *>* pVec = m_threadGroupList[LONGCONNECTEDENTITY]->GetThreadList();
	int nThreadNum = (int)pVec->size();
	for (int nB = 0; nB < nThreadNum; nB ++)
	{
		statStr << "\t\t<EntityIndex:" << nB << ">:" << ((CPingLongConnectThread*)(*pVec)[nB])->GetSendListLog() << "\n";
	}
	WRITE_ALL_LOG(nLogType, "%s", statStr.str().c_str());
}





