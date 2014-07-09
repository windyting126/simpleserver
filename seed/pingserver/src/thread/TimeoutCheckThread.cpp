/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: TimeoutCheckThread.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-03
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-03			1.0			
* =======================================================================
*/
#include "TimeoutCheckThread.h"
#include "pinglongconnectcmd.h"
#include "PingServerDef.h"
#include "CmdCounter.h"

CTimeoutCheckThread::CTimeoutCheckThread(int nEntityType, int nEntityId)
	: CWorkThread(nEntityType, nEntityId)
{
	m_pServer = NULL;
	m_pPeerHashTable = NULL;
	m_nCheckInterval = 0;
	m_nSynInterval = 0;
	m_nTimeout = 0;
}

CTimeoutCheckThread::~CTimeoutCheckThread()
{

}

int CTimeoutCheckThread::Init()
{
	m_pServer = (CPingServer *)CMyserver::GetInstance();
	if(!m_pServer)
	{
		WriteRunInfo::WriteLog("Init CTimeoutCheckThread failed : Get Server Instance");
		return -1;
	}
	m_nCheckInterval = m_pServer->GetConf().GetConf().nCheckTimeInterval;
	m_nTimeout = m_pServer->GetConf().GetConf().nTimeout;
	m_nSynInterval = m_pServer->GetConf().GetConf().nOntimeSynInterval;
	m_nStatInterval = m_pServer->GetConf().GetConf().nStatInterval;
	m_pPeerHashTable = m_pServer->GetPeerHashTable();
	if (!m_pPeerHashTable)
	{
		WriteRunInfo::WriteLog("Init CTimeoutCheckThread failed : Get Peer Hash Table");
		return -1;
	}
	return 0;
}

int CTimeoutCheckThread::Run()
{
	struct timeval tvTmp;
	unsigned long long ullLastCheckTime;
	unsigned long long ullLastSynTime;
	unsigned long long ullLastWriteStatTime;
	gettimeofday(&tvTmp, NULL);
	ullLastCheckTime = tvTmp.tv_sec;
	ullLastSynTime = tvTmp.tv_sec;
	ullLastWriteStatTime = tvTmp.tv_sec;
	while (m_nRunFlag)
	{	
		usleep(100000);
		gettimeofday(&tvTmp, NULL);
		if (tvTmp.tv_sec - ullLastCheckTime >= (unsigned)m_nCheckInterval)
		{
			ullLastCheckTime = tvTmp.tv_sec;
			Process(NULL);	
		}
		gettimeofday(&tvTmp, NULL);
		if (tvTmp.tv_sec - ullLastSynTime >= (unsigned)m_nSynInterval)
		{
			ullLastSynTime = tvTmp.tv_sec;
			SendMsg(PINGONTIMESYN_ENTITY_TYPE, RANDOMENTITYID, 0, ONTIMESYN_MSGTYPE, NULL);
		}

		gettimeofday(&tvTmp, NULL);
		if (tvTmp.tv_sec - ullLastWriteStatTime >= (unsigned) m_nStatInterval)
		{
			ullLastWriteStatTime = tvTmp.tv_sec;
			CCmdCounter::GetHandler()->WriteLog(LOGTYPE_STATUS, true);
			CMyserver::GetInstance()->WriteMsgQueueLog(LOGTYPE_STATUS);
			((CPingServer*) CMyserver::GetInstance())->WriteSendListLog(LOGTYPE_STATUS);
			WRITE_ALL_LOG(LOGTYPE_STATUS, "CPingPeerHashTable Peer Num = [%d]", 
				CPingPeerHashTable::GetHashTable()->GetNodeNum());
		}
	}
	return 0;
}

int CTimeoutCheckThread::Process(CMsg *pMsg)
{
	int nHashBucketLen = m_pPeerHashTable->GetHashLength();
	list<TPingPeerInfo> lstPingPeerInfo;
	for (int iA = 0; iA < nHashBucketLen; iA ++)
	{
		m_pPeerHashTable->CheckPeerList(lstPingPeerInfo, iA, m_nTimeout);
		list<TPingPeerInfo>::iterator itrPeer = lstPingPeerInfo.begin();
		for (; itrPeer != lstPingPeerInfo.end(); ++ itrPeer)
		{
			TLogoutSynCmd LogoutSynCmd;
			LogoutSynCmd.nInternalIp = itrPeer->nInternalIp;
			LogoutSynCmd.strPeerId = itrPeer->strPeerId;
			int nBufLen =  LogoutSynCmd.GetSize() + TSynBaseCommand::GetSize();
			char *pBuf = new char[nBufLen];
			CBufMgr tmpBufMgr(pBuf, nBufLen);
			CSynEncodeCommand<TLogoutSynCmd> EncodeCmd(pBuf, nBufLen, LogoutSynCmd, MSG_LONGCONNECT_SYN_LOGOUT);
			if (EncodeCmd.Encode() == 0 && m_pServer)
			{
				if (m_pServer->GetConf().GetConf().bUseMulticast)
				{
					TransMsg(EncodeCmd.GetPacket(), (unsigned)EncodeCmd.GetPacketLen(), -1, -1);
					WRITE_TRACE("Trans Timeout to Rserver&Qserver");
				}
				else
				{
					int nRServerNum = (int)m_pServer->GetConf().GetConf().vecResourceSrv.size();
					for (int iA = 0; iA < nRServerNum; iA ++)
					{
						TransMsg(EncodeCmd.GetPacket(), (unsigned)EncodeCmd.GetPacketLen(), iA, RSERVERMACHINETYPE);
						WRITE_TRACE("Trans Timeout to Rserver [%d]", iA);
					}
					int nQserverNum = (int)m_pServer->GetConf().GetConf().vecQuervSrv.size();
					for (int iA = 0; iA < nQserverNum; iA ++)
					{
						TransMsg(EncodeCmd.GetPacket(), (unsigned)EncodeCmd.GetPacketLen(), iA, QSERVERMACHINETYPE);
						WRITE_TRACE("Trans Timeout to Qserver [%d]", iA);
					}
				}

				/////Buck Add statistics//////
				CCmdCounter::GetHandler()->Count(PEERTIMEOUT);
			}
		}
	}
	return 0;
}

int CTimeoutCheckThread::PutMsgToInputList(CMsg *pMsg)

{
	return 0;
}

void CTimeoutCheckThread::TransMsg(char *pClientCommandPacketBuf, unsigned int uClientCommandPacketLen, int iServerIndex, int nServerType)
{
	CTmpBufMgr GenPacketHelper;
	int nTmpLen = sizeof(SERVERINNERCOMMANDPACKET) + uClientCommandPacketLen;
	char *pTmpBuf;
	pTmpBuf = new char[nTmpLen];
	GenPacketHelper.AttachBuf(pTmpBuf, nTmpLen);

	SERVERINNERCOMMANDPACKET serverInnerCommandPacket;
	serverInnerCommandPacket.ucDestMachineType = nServerType;
	serverInnerCommandPacket.usDestMachineIndex = iServerIndex;

	serverInnerCommandPacket.serverCommandPacket.usMsgId = (unsigned short)htons(ONLINE_SYN);
	serverInnerCommandPacket.serverCommandPacket.iMsgLen = (int)htonl(uClientCommandPacketLen);
	GenPacketHelper.Put(serverInnerCommandPacket);

	GenPacketHelper.PutCharBuf(pClientCommandPacketBuf, uClientCommandPacketLen);

	TMsg tMsg;
	tMsg.destEntityType = ((iServerIndex == -1 && nServerType == -1) ? UDP_MSEND_ENTITY_TYPE : LONGCONNECTEDENTITY);
	tMsg.destEntityId = 0;
	tMsg.srcEntityId = GetEntityId();
	tMsg.srcEntityType= GetEntityType();
	tMsg.srcTaskId = 0;
	tMsg.taskId = 0;
	tMsg.msgType = SYN_MSGTYPE;

	CMsgBody * pMsgBody = new CClientMsgBody(GenPacketHelper.GetBuf(), GenPacketHelper.GetRealBufLen());
	GenPacketHelper.DisAttachBuf();
	CMsg *pMsg = new CMsg(tMsg, pMsgBody);
	CMyserver::GetInstance()->PutMsgToThread(pMsg);
}

