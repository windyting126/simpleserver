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

#include "rserver.h"
#include "myclassfactory.h"
#include "statistic.h"
#include "configupdatetask.h"

void CPRServer::InitLogSystem()
{
	TLogType LogType;
	LogType.nLogtype = ONLINE_LOG_TYPE;
	LogType.nFileNum = 0;
	LogType.nLogOff = 0;
	strncpy( LogType.sFileNameSuffix , ".online", sizeof(LogType.sFileNameSuffix) );
	WriteRunInfo::LOGTYPEARRAY.InsertLogType( LogType );

	LogType.nLogtype = RSERVER_LOG_TYPE;
	LogType.nFileNum = 0;
	LogType.nLogOff = 0;
	strncpy(LogType.sFileNameSuffix, ".prserver", sizeof(LogType.sFileNameSuffix));
	WriteRunInfo::LOGTYPEARRAY.InsertLogType(LogType);

	LogType.nLogtype = STATISTIC_LOG_TYPE;
	LogType.nFileNum = 0;
	LogType.nLogOff = 1;
	strncpy(LogType.sFileNameSuffix, ".status", sizeof(LogType.sFileNameSuffix));
	WriteRunInfo::LOGTYPEARRAY.InsertLogType(LogType);

	WriteRunInfo::Init("logs/prsrv");
	//when debug
	//WriteRunInfo::SetLogFlag(2, 1);
	//WriteRunInfo::SetLogFlag(ONLINE_LOG_TYPE, 1);
	//WriteRunInfo::SetLogFlag(RSERVER_LOG_TYPE, 1);
	//WriteRunInfo::SetLogFlag(RUNLOGTYPE, 1);
}

int CPRServer::InitServer()
{
	int ret = m_MyConfig.Init();
	if ( ret != 0 )
	{
		WriteRunInfo::WriteLog("Init MyConfig in PRServer fail");
		return -1;
	}
	CConfigUpdateTask::GetInstance().UpdateConfig();

	int iHashBucketLen = m_MyConfig.GetHashLeng(); 
	int iResult = CPingPeerHashTable::CreateHashTable(iHashBucketLen);
	if (iResult != 0)
	{
		WriteRunInfo::WriteLog("Init server fail : Create PingPeerHashTable failed");
		return -1;
	}
	m_pPingPeerHashTable = CPingPeerHashTable::GetHashTable();	
	iResult = m_pPingPeerHashTable->Init();
	if (iResult != 0)
	{
		WriteRunInfo::WriteLog("Init PeerHashTable fail");
		return -1;
	}
	
	ret = CMyserver::InitServer();
	if (ret != 0)
	{
		WriteRunInfo::WriteLog("Init server fail");
		return -1;
	}
	m_pClassFactory = new CMyClassFactory();

	CCmdCounter::Create();
	CCmdCounter* pCmdHandler = CCmdCounter::GetHandler();
	ret = pCmdHandler->Init("cmd", STATISTIC_LOG_TYPE);
	if (ret != 0)
	{
		WriteRunInfo::WriteLog("Init CCmdCounter fail");
		return -1;
	}
	pCmdHandler->AddItem(SEEDCMD_REPORT_RCLIST, "reportrclist");
	pCmdHandler->AddItem(SEEDCMD_INSERT_RC, "insertrc");
	pCmdHandler->AddItem(SEEDCMD_DELETE_RCLIST, "deleterclist");
	pCmdHandler->AddItem(SEEDCMD_DELETE_RCLISTNEW, "deleterclistnew");

	pCmdHandler->AddItem(MSG_LONGCONNECT_SYN_PING, "ping");
	pCmdHandler->AddItem(MSG_LONGCONNECT_SYN_LOGOUT, "logout");
	pCmdHandler->AddItem(MSG_LONGCONNECT_SYN_ONTIME, "ontime");
	pCmdHandler->AddItem(MSG_LONGCONNECT_SYN_HANDSHAKE, "handshake");
	pCmdHandler->AddItem(MSG_LONGCONNECT_SYN_AFFIRM, "affirm");

	pCmdHandler->AddItem(INSERTDB_COUNT, "insertdb");
	pCmdHandler->AddItem(DELETEDB_COUNT, "deletedb");

	return ret;
}

int CPRServer::StartServer()
{
	int ret = StartThreads();
	if (ret == 0)
	{
		while (1)//Ö÷Ïß³ÌÐÝÃß
		{
			sleep(100);
		}
	}
	return ret;
}

int CPRServer::StartThreads()
{
	int ret = 0;

	//////////////////////////////////////////////////////////////////////////
	//timer thread
	m_pTimerThreadGrp = new CThreadGroup(1, TIMERENTITY);
	ret = m_pTimerThreadGrp->Init();
	if (ret != 0)
	{       
		WriteRunInfo::WriteLog("Init timer thread fail");
		return -1;
	}
	WriteRunInfo::WriteLog("Init timer thread succeed");


	//////////////////////////////////////////////////////////////////////////
	//receive and send thread
	int nTcpThreadNum =  m_baseConf.GetConfigItem().nTcpThreadNum;
	m_pRecvAndSendThreadGrp = new CThreadGroup(nTcpThreadNum, LONGCONNECTEDPROCESSENTITY);
	ret = m_pRecvAndSendThreadGrp->Init();
	if (ret != 0)
	{       
		WriteRunInfo::WriteLog("Init recvandsend thread fail");
		return -1;
	}       
	WriteRunInfo::WriteLog("Init recvandsend thread succeed");


	//////////////////////////////////////////////////////////////////////////
	//maintain thread (dbupdatethread.h)
	int nThreadNum =  m_MyConfig.GetDbUpdateThreadNum();
	m_pMaintainThreadGrp = new CThreadGroup(nThreadNum, MAINTAINENTITY);
	ret = m_pMaintainThreadGrp->Init();
	if (ret != 0)
	{       
		WriteRunInfo::WriteLog("Init maintain thread fail");
		return -1;
	}       
	WriteRunInfo::WriteLog("Init maintain thread succeed");

	//////////////////////////////////////////////////////////////////////////
	//Online Db Update thread (must start after maintain thread)
	m_pOnlineDBUpdateThread = new COnlineDBUpdateThread();
	ret = m_pOnlineDBUpdateThread->Init();
	if( ret != 0 )
	{
		WriteRunInfo::WriteLog("Init onlinedbupdate thread fail");
		return -1;
	}
	ret = m_pOnlineDBUpdateThread->Start();
	if( ret != 0 )
	{
		WriteRunInfo::WriteLog("Start onlinedbupdate thread fail");
		return -1;
	}
	WriteRunInfo::WriteLog("Init onlinedbupdate thread succeed");

	//////////////////////////////////////////////////////////////////////////
	//pingserver sync process thread
	nThreadNum = m_MyConfig.GetPingSynProcessThreadNum();
	m_pPingSynProcessThreadGrp = new CThreadGroup(nThreadNum, PING_SYN_PROCESS_ENTITY_TYPE);
	ret = m_pPingSynProcessThreadGrp->Init();
	if (ret != 0)
	{       
		WriteRunInfo::WriteLog("Init pingserver syn process thread fail");
		return -1;
	}
	WriteRunInfo::WriteLog("Init pingserver syn process thread succeed");

	//////////////////////////////////////////////////////////////////////////
	//listen thread
	int nPort = m_baseConf.GetConfigItem().nTcpPort;
	string sHost = m_baseConf.GetConfigItem().sServerAddr;
	string sTmpData = string((char*)(&nPort), 4);
	sTmpData.append(sHost);
	m_pListenThreadGrp = new CThreadGroup(1, LONGCONNECTEDLISTENENTITY, sTmpData);
	ret = m_pListenThreadGrp->Init();
	if (ret != 0)
	{       
		WriteRunInfo::WriteLog("Init listen thread fail");
		return -1;
	}
	WriteRunInfo::WriteLog("Init listen thread succeed");  	

	//////////////////////////////////////////////////////////////////////////
	//UDP multicast receive thread	
	sTmpData.clear();
	nPort = m_MyConfig.GetMRecvPort();
	sHost = "";
	sTmpData = string((char*)(&nPort), 4);
	sTmpData.append(sHost);
	m_pUdpMRecvThreadGrp = new CThreadGroup(1, UDP_MRECV_ENTITY_TYPE, sTmpData);
	ret = m_pUdpMRecvThreadGrp->Init();
	if (ret != 0)
	{
		WriteRunInfo::WriteError("Init UDP multicast receive thread fail");
		return -1;
	}
	WriteRunInfo::WriteLog("Init UDP multicast receive thread succeed!");
	
	m_pMaintainUpdThread = new CMaintainUdpThread(m_baseConf.GetConfigItem().sMaintainIp, 
		m_baseConf.GetConfigItem().nMaintainPort);
	ret = m_pMaintainUpdThread->Start();
	if (ret != 0)
	{
		WriteRunInfo::WriteLog("Init maintain udp thread fail");
		return -1;
	}        
	WriteRunInfo::WriteLog("Init maintain udp thread succeed");


	WriteRunInfo::WriteLog("Init prsrv succeed");  
	return 0;
}


CMyConfig &CPRServer::GetMyConf()
{
	return m_MyConfig;
}

