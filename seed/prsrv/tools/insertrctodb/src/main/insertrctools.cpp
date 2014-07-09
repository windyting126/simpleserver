#include "insertrctools.h"
#include "myclassfactory.h"
#include "insertrctoolsdef.h"
#include "insertrccmd.h"
#include "insertpeercmd.h"
#include "framecommon/framecommon.h"
using namespace MYFRAMECOMMON;
using namespace std;

CInsertTools::CInsertTools()
{
}

CInsertTools::~CInsertTools()
{
}

int CInsertTools::InitServer()
{
	int nRet = CMyserver::InitServer();
	if (0 != nRet)
	{
		WriteRunInfo::WriteError ("Init server fail");
		return -1;
	}

	nRet = m_conf.Init();
	if (0 != nRet)
	{
		WriteRunInfo::WriteError ("Load conf fail");
		return -1;
	}

	m_pClassFactory = new CMyClassFactory();
	if (NULL == m_pClassFactory)
	{
		return -1;
	}

	return 0;
}

int CInsertTools::StartServer()
{
	int nRet = 0;
	WriteRunInfo::WriteLog("==========================Begin start server====================================");

	{
		m_pTimerThreadGrp = new CThreadGroup (1, TIMERENTITY);
		nRet = m_pTimerThreadGrp->Init();
		if (0 != nRet)
		{
			WriteRunInfo::WriteError ("Init timer thread fail");
			return -1;
		}
		WriteRunInfo::WriteLog ("Init timer thread succeed");
	}

	{
		m_pConnectThreadGrp = new CThreadGroup (1, LONGCONNECTEDENTITY);
		nRet = m_pConnectThreadGrp->Init();
		if (0 != nRet)
		{
			WriteRunInfo::WriteError ("Init long connect thread fail");
			return -1;
		}
		WriteRunInfo::WriteLog ("Init long connect thread succeed");
		TConfItem confItem = m_conf.GetConfItem();
		SendConnectMsg (confItem.strIp.c_str(), confItem.nPort);
	}

	WriteRunInfo::WriteLog("Pcon server start succeed ...");

	sleep(1);

	nRet = LoadPeers();
	if (nRet != 0)
	{
		WriteRunInfo::WriteError ("Load peers fail");
	}
	
	nRet = LoadRc();
	if (nRet != 0)
	{
		WriteRunInfo::WriteError ("Load rc fail");
	}
	return 0;
}

int CInsertTools::InitLogSystem()
{
	TLogType LogType;
	LogType.nLogtype = DEBUG_LOG_TYPE;
	LogType.nFileNum = 0;
	LogType.nLogOff = 1;
	strncpy (LogType.sFileNameSuffix, ".debug", sizeof(LogType.sFileNameSuffix));
	WriteRunInfo::LOGTYPEARRAY.InsertLogType (LogType);

	WriteRunInfo::Init ("insertrctools");

	return 0;
}

int CInsertTools::LoadPeers()
{
	CMysql Mysql;
	char szSqlBuf[1024];
	struct timespec tm1, tm2;

	try
	{
		Mysql.Init();
		Mysql.Connect(m_conf.GetConfItem().vecDbItems[0].strIp, m_conf.GetConfItem().vecDbItems[0].strUserName, m_conf.GetConfItem().vecDbItems[0].strPasswd);
		snprintf(szSqlBuf, sizeof(szSqlBuf), "select * from %s.%s;", m_conf.GetConfItem().strDbName.c_str(), ONLINE_PEER_TABLE.c_str());
		Mysql.Query(szSqlBuf);

		clock_gettime(CLOCK_MONOTONIC, &tm1);
		int nIndex = 0;
		long lUseTime = 0;
		int nSleepTime = (int)((1000 / (float)m_conf.GetConfItem().nSpeedPerSec) * 0.85 * 100 * 1000);
		string strPeerId;
		while(Mysql.FetchRow())
		{
			strPeerId = Mysql.GetField("fpeerid");

			CInsertPeerCmd reqCmd(strPeerId);
			reqCmd.Process();
			TransMsg(reqCmd.GetBuf(), reqCmd.GetBufLen(), 0, MACHINE_TYPE, ONLINE_SYN);
			nIndex ++;
			if(nIndex % 100 == 0)
			{
				usleep(nSleepTime);
			}
			if(nIndex >= m_conf.GetConfItem().nSpeedPerSec)
			{
				clock_gettime(CLOCK_MONOTONIC, &tm2);
				lUseTime =  (tm2.tv_sec - tm1.tv_sec) * 1000000000 + (tm2.tv_nsec - tm1.tv_nsec);
				lUseTime = lUseTime / 1000;
				WRITE_ALL_LOG(DEBUG_LOG_TYPE, "send %d peer info, use time %ld microsec", nIndex, lUseTime);
				nIndex = 0;
				lUseTime = 0;
				clock_gettime(CLOCK_MONOTONIC, &tm1);
			}
		}
		Mysql.FreeResult();
	}
	catch (CMysqlException& e)
	{
		WriteRunInfo::WriteError("CInsertTools::LoadPeers, sql %s, error %s", szSqlBuf, e.GetErrMsg());
		return -1;
	}
	return 0;
}

int CInsertTools::LoadRc()
{
	CMysql Mysql;
	char szSqlBuf[1024];
	string strPeerId;
	long long llFileSize;
	string strGcid;
	int nCapability;
	unsigned char nDownloadScale;
	struct timespec tm1, tm2;
	int nIndex = 0;
	long lUseTime = 0;
	int nSleepTime = (int)((1000 / (float)m_conf.GetConfItem().nSpeedPerSec) * 0.85 * 100 * 1000);
	clock_gettime(CLOCK_MONOTONIC, &tm1);

	for (int i = 0; i < m_conf.GetConfItem().nDbNum; i ++)
	{
		try
		{
			Mysql.Init();
			Mysql.Connect(m_conf.GetConfItem().vecDbItems[i].strIp, m_conf.GetConfItem().vecDbItems[i].strUserName, m_conf.GetConfItem().vecDbItems[i].strPasswd);

			for (int j = 0; j < m_conf.GetConfItem().nTableNum; j ++)
			{
				stringstream strTableName;
				strTableName << RC_PEER_TABLE;
				strTableName << j;
				snprintf(szSqlBuf, sizeof(szSqlBuf), "select * from %s.%s;", m_conf.GetConfItem().strDbName.c_str(), strTableName.str().c_str());
				strTableName.clear();
				Mysql.Query(szSqlBuf);

				while(Mysql.FetchRow())
				{
					strGcid = Mysql.GetField("fcid");
					strPeerId = Mysql.GetField("fpeerid");
					llFileSize = strtoul(Mysql.GetField("ffilesize"), 0, 10);
					nCapability = strtoul(Mysql.GetField("fcapability"), 0, 10);
					nDownloadScale = strtoul(Mysql.GetField("downpercent"), 0, 10);

					CInsertRcCmd reqCmd(strGcid, strPeerId, llFileSize, nCapability, nDownloadScale);
					reqCmd.Process();
					TransMsg(reqCmd.GetBuf(), reqCmd.GetBufLen(), 0, MACHINE_TYPE, CONNECT_SERVER_COMMON);

					nIndex ++;
					if(nIndex % 100 == 0)
					{
						usleep(nSleepTime);
					}
					if(nIndex >= m_conf.GetConfItem().nSpeedPerSec)
					{
						clock_gettime(CLOCK_MONOTONIC, &tm2);
						lUseTime =  (tm2.tv_sec - tm1.tv_sec) * 1000000000 + (tm2.tv_nsec - tm1.tv_nsec);
						lUseTime = lUseTime / 1000;
						WRITE_ALL_LOG(DEBUG_LOG_TYPE, "send %d rc info, use time %ld microsec", nIndex, lUseTime);
						nIndex = 0;
						lUseTime = 0;
						clock_gettime(CLOCK_MONOTONIC, &tm1);
					}
				}
				Mysql.FreeResult();
			}

			for (int j = 0; j < m_conf.GetConfItem().nTableNum; j ++)
			{
				stringstream strTableName;
				strTableName << RC_PEER_TEMP_TABLE;
				strTableName << j;
				snprintf(szSqlBuf, sizeof(szSqlBuf), "select * from %s.%s;", m_conf.GetConfItem().strDbName.c_str(), strTableName.str().c_str());
				strTableName.clear();
				Mysql.Query(szSqlBuf);

				while(Mysql.FetchRow())
				{
					strGcid = Mysql.GetField("fcid");
					strPeerId = Mysql.GetField("fpeerid");
					llFileSize = strtoul(Mysql.GetField("ffilesize"), 0, 10);
					nCapability = strtoul(Mysql.GetField("fcapability"), 0, 10);
					nDownloadScale = strtoul(Mysql.GetField("downpercent"), 0, 10);

					CInsertRcCmd reqCmd(strGcid, strPeerId, llFileSize, nCapability, nDownloadScale);
					reqCmd.Process();
					TransMsg(reqCmd.GetBuf(), reqCmd.GetBufLen(), 0, MACHINE_TYPE, CONNECT_SERVER_COMMON);

					nIndex ++;
					if(nIndex % 100 == 0)
					{
						usleep(nSleepTime);
					}
					if(nIndex >= m_conf.GetConfItem().nSpeedPerSec)
					{
						clock_gettime(CLOCK_MONOTONIC, &tm2);
						lUseTime =  (tm2.tv_sec - tm1.tv_sec) * 1000000000 + (tm2.tv_nsec - tm1.tv_nsec);
						lUseTime = lUseTime / 1000;
						WRITE_ALL_LOG(DEBUG_LOG_TYPE, "send %d rc temp info, use time %ld microsec", nIndex, lUseTime);
						nIndex = 0;
						lUseTime = 0;
						clock_gettime(CLOCK_MONOTONIC, &tm1);
					}
				}
				Mysql.FreeResult();
			}
		}
		catch (CMysqlException& e)
		{
			WriteRunInfo::WriteError("CInsertTools::LoadPeers, sql %s, error %s", szSqlBuf, e.GetErrMsg());
			return -1;
		}
	}
	
	return 0;
}

int CInsertTools::SendConnectMsg(const char* pcszRemoteIp, unsigned short usRemotePort)
{
	TMsg tMsg;
	tMsg.destEntityType = LONGCONNECTEDENTITY;
	tMsg.destEntityId = 0;
	tMsg.srcEntityId = 0;
	tMsg.srcEntityType = MAINENTITY;
	tMsg.srcTaskId = 0;
	tMsg.taskId = 0;
	tMsg.msgType = TOCONNECTMSGTYPE;

	CMsgBody* pMsgBody = new CToConnectMsgBody(pcszRemoteIp, usRemotePort, MACHINE_TYPE, 0);
	if (!pMsgBody) 
	{
		return -1;
	}

	CMsg* pMsg = new CMsg(tMsg, pMsgBody);
	if (!pMsg)
	{
		delete pMsgBody;
		return -1;
	}
	CMyserver::GetInstance()->PutMsgToThread(pMsg);
	return 0;
}

void CInsertTools::TransMsg(char* pBuf, int nLen, int nMachineIndex, int nType, int nMsgId)
{
	CTmpBufMgr GenPacketHelper;
	GenPacketHelper.SetIfNetTrans(0); 
	int nTmpLen = sizeof(SERVERINNERCOMMANDPACKET) + nLen; //+ sizeof(int) + strCid.size();
	char* pTmpBuf = new char [nTmpLen];
	GenPacketHelper.AttachBuf(pTmpBuf, nTmpLen);

	SERVERINNERCOMMANDPACKET serverInnerCommandPacket;
	serverInnerCommandPacket.ucDestMachineType = nType;
	serverInnerCommandPacket.usDestMachineIndex = nMachineIndex;
	serverInnerCommandPacket.serverCommandPacket.usMsgId = (unsigned short)htons (nMsgId);
	serverInnerCommandPacket.serverCommandPacket.uSrcClientIp = 0;
	serverInnerCommandPacket.serverCommandPacket.usSrcClientPort = 0;
	serverInnerCommandPacket.serverCommandPacket.ullSrcTaskId = 0;
	serverInnerCommandPacket.serverCommandPacket.iSrcProcessId = 0;
	serverInnerCommandPacket.serverCommandPacket.iMsgLen = (int)htonl (nLen); //+ sizeof(int) + strCid.size());

	GenPacketHelper.Put (serverInnerCommandPacket);
	GenPacketHelper.PutCharBuf (pBuf, nLen);

	TMsg tMsg;
	tMsg.destEntityType = LONGCONNECTEDENTITY;
	tMsg.destEntityId = 0;
	tMsg.srcEntityId = 0;
	tMsg.srcEntityType = 0;
	tMsg.srcTaskId = 0;
	tMsg.taskId = 0;
	tMsg.msgType = CLIENTMSGTYPE;

	CMsgBody* pMsgBody = new CClientMsgBody (GenPacketHelper.GetBuf(), GenPacketHelper.GetRealBufLen());
	GenPacketHelper.DisAttachBuf();
	if (!pMsgBody)
	{
		return;
	}

	CMsg* pMsg = new CMsg (tMsg, pMsgBody);
	if (!pMsg)
	{
		delete pMsgBody;
		return;
	}

	CMyserver::GetInstance()->PutMsgToThread(pMsg);
}
