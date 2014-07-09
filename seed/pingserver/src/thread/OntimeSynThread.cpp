/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: SynThread.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-04
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      --------------------------------------------------------------
*		wangtiefeng		2009-08-04			1.0			
* =======================================================================
*/
#include "OntimeSynThread.h"
#include "pinglongconnectcmd.h"
#include "CommonTask.h"
#include "CmdCounter.h"
#include "PingServerDef.h"

//const int KILOBYTE = 1024; 
//const int MAXSYNBUFLEN = 20*KILOBYTE*KILOBYTE;

COntimeSynThread::COntimeSynThread(int nEntityType, int nEntityId)
	: CWorkThread(nEntityType, nEntityId)
{
	m_pServer = NULL;
	m_pPeerHashTable = NULL;
	m_pSynSendSem = NULL;
	m_nQServerNum = 0;
	m_nRServerNum = 0;
}

COntimeSynThread::~COntimeSynThread()
{

}

int COntimeSynThread::PutMsgToInputList(CMsg *pMsg)
{
	m_InputList.put(pMsg);
	return 0;
}

int COntimeSynThread::Init()
{
	m_pServer = (CPingServer *)CMyserver::GetInstance();
	if(!m_pServer)
	{
		WriteRunInfo::WriteLog("Init CTimeoutCheckThread failed : Get Server Instance");
		return -1;
	}
	m_pPeerHashTable = m_pServer->GetPeerHashTable();
	if (!m_pPeerHashTable)
	{
		WriteRunInfo::WriteLog("Init CTimeoutCheckThread failed : Get Peer Hash Table");
		return -1;
	}
	m_pSynSendSem = m_pServer->GetSynSendSem();
	if (!m_pSynSendSem)
	{
		WriteRunInfo::WriteLog("Init CTimeoutCheckThread failed : Get Syn Send Sem");
		return -1;
	}
	m_nQServerNum = (int)m_pServer->GetConf().GetConf().vecQuervSrv.size();
	for (int iA = 0; iA < m_nQServerNum; ++ iA)
	{
		m_vecQConnectFirstTime.push_back(true);
	}
	m_nRServerNum  = (int)m_pServer->GetConf().GetConf().vecResourceSrv.size();
	for (int iA = 0; iA < m_nRServerNum; ++ iA)
	{
		m_vecRConnectFirstTime.push_back(true);
	}
	m_nSynWait = (int)m_pServer->GetConf().GetConf().nSynWait;
	m_nSynBufLen = (int)m_pServer->GetConf().GetConf().nSynBufLen;
	return 0;
}

int COntimeSynThread::Run()
{
	while (m_nRunFlag)
	{
		CMsg *pMsg = m_InputList.get();
		if (pMsg != NULL)
		{
			Process(pMsg);
			delete pMsg;
		}
	}
	return 0;
}

int COntimeSynThread::Process(CMsg *pMsg)
{
	TMsg *pTMsg = pMsg->GetTMsg();
	if (!pTMsg)
	{
		return -1;
	}

	switch (pTMsg->msgType)
	{
	case ONTIMESYN_MSGTYPE:
		{	
			ProcessOntime(pMsg);
			break;
		}
	case RESYN_MSGTYPE:
		{
			/////Buck Add statistics//////
			CCmdCounter::GetHandler()->Count(LC_RECONNECT);
			ProcessResyn(pMsg);
			break;
		}
	case SYN_AFFIRM_MSGTYPE:
		{
			ProcessSynAffirm(pMsg);
			break;
		}
	default:
		{
			WriteRunInfo::WriteError("%s:%d:%s : Wrong message head!" , __FILE__ , __LINE__ , __FUNCTION__);
		}
	}
	return 0;
}

int COntimeSynThread::ProcessOntime(CMsg *pMsg)
{
	WriteRunInfo::WriteLog("COntimeSynThread::ProcessOntime : Syn To Server BeginTime = %llu!", CMyserver::GetInstance()->GetTimeVal().tv_sec);
	SynToServer();
	WriteRunInfo::WriteLog("COntimeSynThread::ProcessOntime : Syn To Server EndTime = %llu!", CMyserver::GetInstance()->GetTimeVal().tv_sec);
	return 0;
}

int COntimeSynThread::ProcessResyn(CMsg *pMsg)
{
	if (!pMsg)
	{
		return -1;
	}
	CBufMsgBody *pBufMsgbody  = (CBufMsgBody *)pMsg->GetMsgBody();
	if(!pBufMsgbody)
	{
		WriteRunInfo::WriteError("%s:%d:%s : Message Body is NULL!", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	char *pBuf = pBufMsgbody->GetBuf();
	int nBufLen = pBufMsgbody->Size();
	if (nBufLen < (int)(sizeof(char) + sizeof(short)) || !pBuf)
	{
		WriteRunInfo::WriteError("%s:%d:%s : Message Buf Wrong!", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	char nServerType = *(pBuf);
	short nServerIndex = *(short *)(pBuf + 1);
	if (nServerType == QSERVERMACHINETYPE)
	{
		if (nServerIndex >= 0 && nServerIndex < m_nQServerNum)
		{
			if (m_vecQConnectFirstTime[nServerIndex])
			{
				m_vecQConnectFirstTime[nServerIndex] = false;
				SendHandShake(nServerType, nServerIndex);
				WRITE_TRACE("SendHandShake to Qserver Type = %d, Index = %d", nServerType, nServerIndex);
			}
			else
			{
				SendSynAffirm(nServerType, nServerIndex);
				WRITE_TRACE("SendSynAffirm to Qserver Type = %d, Index = %d", nServerType, nServerIndex);
			}			
		}
		else
		{
			WriteRunInfo::WriteError("%s:%d:%s : QServer Index is not right!", __FILE__, __LINE__, __FUNCTION__);
			return -1;
		}
		
	}
	else if (nServerType == RSERVERMACHINETYPE)
	{
		if (nServerIndex >= 0 && nServerIndex < m_nRServerNum)
		{
			if (m_vecRConnectFirstTime[nServerIndex])
			{
				m_vecRConnectFirstTime[nServerIndex] = false;
				SendHandShake(nServerType, nServerIndex);
				WRITE_TRACE("SendHandShake to Rserver Type = %d, Index = %d", nServerType, nServerIndex);
			}
			else
			{
				SendSynAffirm(nServerType, nServerIndex);
				WRITE_TRACE("SendSynAffirm to Rserver Type = %d, Index = %d", nServerType, nServerIndex);
			}			
		}
		else
		{
			WriteRunInfo::WriteError("%s:%d:%s : RServer Index is not right!", __FILE__, __LINE__, __FUNCTION__);
			return -1;
		}
		
	}
	else
	{
		WriteRunInfo::WriteError("%s:%d:%s : Machine Type is not right!", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	return 0;
}

int COntimeSynThread::ProcessSynAffirm(CMsg *pMsg)
{
	if (!pMsg)
	{
		return -1;
	}
	CBufMsgBody *pBufMsgbody  = (CBufMsgBody *)pMsg->GetMsgBody();
	if(!pBufMsgbody)
	{
		WriteRunInfo::WriteError("%s:%d:%s : Message Body is NULL!", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	char *pBuf = pBufMsgbody->GetBuf();
	int nBufLen = pBufMsgbody->Size();
	if (nBufLen <= 0 || !pBuf)
	{
		WriteRunInfo::WriteError("%s:%d:%s : Message Buf Wrong!", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	TSynAffirmCmd SynAffirmCmd;
	CSynDecodeCommand<TSynAffirmCmd> DecodeCmd(pBuf, nBufLen, SynAffirmCmd);
	if (DecodeCmd.Decode() != 0)
	{
		WriteRunInfo::WriteError("%s:%d:%s : Decode SynAffirmCmd failed!", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	WriteRunInfo::WriteLog("COntimeSynThread::ProcessSynAffirm : SynToServer : Type = %d, Index = %d, BeginTime=%llu", 
		SynAffirmCmd.nMachineType, SynAffirmCmd.nMachineIndex, CMyserver::GetInstance()->GetTimeVal().tv_sec);
	SynToServer(SynAffirmCmd.nMachineType, SynAffirmCmd.nMachineIndex);
	WriteRunInfo::WriteLog("COntimeSynThread::ProcessSynAffirm : SynToServer : Type = %d, Index = %d, EndTime=%llu", 
		SynAffirmCmd.nMachineType, SynAffirmCmd.nMachineIndex, CMyserver::GetInstance()->GetTimeVal().tv_sec);
	return 0;
}

int COntimeSynThread::SynToServer(char nServerType /* = -1 */, short nServerIndex /* = -1 */)
{
	if (!m_pServer)
	{
		WriteRunInfo::WriteError("%s:%d:%s : m_pServer is NULL!", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	TOntimeSynCmd OntimeSynCmd(m_pPeerHashTable->GetHashLength(), 0); // nListNum is not used, so initialize it as 0
	int nIterCount = 0;
	char *pBuf = new char[m_nSynBufLen];
	CBufMgr tmpBufMgr(pBuf, m_nSynBufLen);
	CSynEncodeCommand<TOntimeSynCmd> EncodeCmd(pBuf, m_nSynBufLen, OntimeSynCmd, MSG_LONGCONNECT_SYN_ONTIME);
	int nRet = 0;
	do 
	{
		if (EncodeCmd.Encode() != 0)
		{
			nRet = -1;
			break;
		}
		list<TPingPeerInfo> vecPeerList;
		for (int iA = 0; iA < m_pPeerHashTable->GetHashLength(); ++ iA)
		{
			m_pPeerHashTable->GetPeerList(vecPeerList, iA);
			if (OntimeSynCmd.EncodeAddPeerList(EncodeCmd.GetBufMgr(), iA, vecPeerList) != 0 || iA == m_pPeerHashTable->GetHashLength() - 1)
			{
				struct timeval tvTmp;
				struct timeval tvTmp2;
				struct timespec specTime;
				nIterCount++;
				if (nServerIndex == -1 || nServerType == -1)
				{					
					for (int iB = 0; iB < m_nQServerNum; ++ iB)
					{
						if(CCommonTask::CheckConnectState(QSERVERMACHINETYPE, iB))
						{
							TransMsg(EncodeCmd.GetPacket(), EncodeCmd.GetPacketLen(), iB, QSERVERMACHINETYPE);
							gettimeofday(&tvTmp, NULL);
							specTime.tv_sec = tvTmp.tv_sec + m_nSynWait;
							specTime.tv_nsec = 0;
							if (sem_timedwait(m_pSynSendSem, &specTime) == -1)
							{
								if (errno == ETIMEDOUT)
								{
									WRITE_ALL_LOG(LOGTYPE_STATUS, "sem_timedwait timeout:[%d] when Iter:[%d] Syn to Qserver:%d:%d", m_nSynWait, nIterCount, iB, EncodeCmd.GetPacketLen());
								}
								else
								{
									WRITE_ALL_LOG(LOGTYPE_STATUS, "OntimeSyn : sem_timedwait call fail when Iter:[%d] Syn to Qserver:%d:%d", iB, EncodeCmd.GetPacketLen());
								}
							}
							else
							{
								gettimeofday(&tvTmp2, NULL);
								WRITE_ALL_LOG(LOGTYPE_STATUS, "Iter:[%d] Syn to Qserver:%d:%d:%d", nIterCount, iB,
									EncodeCmd.GetPacketLen(),
									1000000 * (tvTmp2.tv_sec-tvTmp.tv_sec)+ tvTmp2.tv_usec-tvTmp.tv_usec);
							}
							WRITE_TRACE("OntimeSyn : Syn to Qserver[%d]", iB);
						}	
						else
						{
							WRITE_TRACE("OntimeSyn :¡¡Qserver[%d] is not Connecet", iB);
						}
					}
					for (int iB = 0; iB < m_nRServerNum; ++ iB)
					{
						if(CCommonTask::CheckConnectState(RSERVERMACHINETYPE, iB))
						{
							TransMsg(EncodeCmd.GetPacket(), EncodeCmd.GetPacketLen(), iB, RSERVERMACHINETYPE);
							gettimeofday(&tvTmp, NULL);
							specTime.tv_sec = tvTmp.tv_sec + m_nSynWait;
							specTime.tv_nsec = 0;
							if (sem_timedwait(m_pSynSendSem, &specTime) == -1)
							{
								if (errno == ETIMEDOUT)
								{
									WRITE_ALL_LOG(LOGTYPE_STATUS, "sem_timedwait timeout:[%d] when Iter:[%d] Syn to Rserver:%d:%d", m_nSynWait, nIterCount, iB, EncodeCmd.GetPacketLen());
								}
								else
								{
									WRITE_ALL_LOG(LOGTYPE_STATUS, "OntimeSyn : sem_timedwait call fail when Iter:[%d] Syn to Rserver:%d:%d", nIterCount, iB, EncodeCmd.GetPacketLen());
								}
							}
							else
							{
								gettimeofday(&tvTmp2, NULL);
								WRITE_ALL_LOG(LOGTYPE_STATUS, "Iter:[%d] Syn to Rserver:%d:%d:%d", nIterCount, iB,
									EncodeCmd.GetPacketLen(),
									1000000 * (tvTmp2.tv_sec-tvTmp.tv_sec)+ tvTmp2.tv_usec-tvTmp.tv_usec);
							}
							
							WRITE_TRACE("OntimeSyn : Syn to Rserver[%d]", iB);
						}
						else
						{
							WRITE_TRACE("OntimeSyn : Rserver[%d] is not Connecet", iB);
						}
					}						
				}
				else /* this branch is for single Q&R server reconnect syn */
				{
					if(CCommonTask::CheckConnectState(nServerType, nServerIndex))
					{
						TransMsg(EncodeCmd.GetPacket(), EncodeCmd.GetPacketLen(), nServerIndex, nServerType);
						gettimeofday(&tvTmp, NULL);
						specTime.tv_sec = tvTmp.tv_sec + m_nSynWait;
						specTime.tv_nsec = 0;
						if (sem_timedwait(m_pSynSendSem, &specTime) == -1)
						{
							if (errno == ETIMEDOUT)
							{
								WRITE_ALL_LOG(LOGTYPE_STATUS, "sem_timedwait timeout:[%d] when RECONNECT Iter:[%d] Syn to Server Type = %d Index = %d Len = %d", 
									m_nSynWait, nIterCount, nServerType, nServerIndex, EncodeCmd.GetPacketLen());
							}
							else
							{
								WRITE_ALL_LOG(LOGTYPE_STATUS, "OntimeSyn : sem_timedwait call fail when RECONNECT Iter:[%] Syn to Server Type = %d Index = %d Len = %d", 
									nIterCount, nServerType, nServerIndex, EncodeCmd.GetPacketLen());
							}
						}
						else
						{
							gettimeofday(&tvTmp2, NULL);
							WRITE_ALL_LOG(LOGTYPE_STATUS, "Iter:[%d] RECONNECT syn to Q&R %d:%d:%d:%d",
								nIterCount, nServerType, nServerIndex, EncodeCmd.GetPacketLen(),
								1000000 * (tvTmp2.tv_sec-tvTmp.tv_sec)+ tvTmp2.tv_usec-tvTmp.tv_usec);
						}
						WRITE_TRACE("OntimeSyn : RECONNECT Syn to Server Type = %d Index = %d", nServerType, nServerIndex);
					}
					else
					{
						WRITE_TRACE("OntimeSyn : Server Type = %d Index = %d is not connected", nServerType, nServerIndex);
					}
				}
				EncodeCmd.Attach(pBuf, m_nSynBufLen);
				EncodeCmd.Encode();
				OntimeSynCmd.EncodeAddPeerList(EncodeCmd.GetBufMgr(), iA, vecPeerList);				
			}
		}		
	} while (0);
	return nRet;	
}

int COntimeSynThread::SendHandShake(char nServerType, short nServerIndex)
{
	THandShakeCmd HandShakeCmd;
	HandShakeCmd.nMachineType = nServerType;
	HandShakeCmd.nMachineIndex = nServerIndex;
	int nBufLen = HandShakeCmd.GetSize() + TSynBaseCommand::GetSize();
	char *pBuf = new char[nBufLen];
	CBufMgr tmpBufMgr(pBuf, nBufLen);
	CSynEncodeCommand<THandShakeCmd> EncodeCmd(pBuf, nBufLen, HandShakeCmd, MSG_LONGCONNECT_SYN_HANDSHAKE);
	int nRet = 0;
	do 
	{
		if (EncodeCmd.Encode() != 0)
		{
			nRet = -1;
			break;
		}
		TransMsg(EncodeCmd.GetPacket(), EncodeCmd.GetPacketLen(), nServerIndex, nServerType);
	} while (0);
	return nRet;
}

int COntimeSynThread::SendSynAffirm(char nServerType, short nServerIndex)
{
	TSynAffirmCmd SynAffirmCmd;
	SynAffirmCmd.nMachineType = nServerType;
	SynAffirmCmd.nMachineIndex = nServerIndex;
	int nBufLen = SynAffirmCmd.GetSize() + TSynBaseCommand::GetSize();
	char *pBuf = new char[nBufLen];
	CBufMgr tmpBufMgr(pBuf, nBufLen);
	CSynEncodeCommand<TSynAffirmCmd> EncodeCmd(pBuf, nBufLen, SynAffirmCmd, MSG_LONGCONNECT_SYN_AFFIRM);
	int nRet = 0;
	do 
	{
		if (EncodeCmd.Encode() != 0)
		{
			nRet = -1;
			break;
		}
		TransMsg(EncodeCmd.GetPacket(), EncodeCmd.GetPacketLen(), nServerIndex, nServerType);
	} while (0);
	return nRet;
}

void COntimeSynThread::TransMsg(char *pClientCommandPacketBuf, unsigned int uClientCommandPacketLen,  int iServerIndex, int nServerType)
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
	tMsg.destEntityType = LONGCONNECTEDENTITY;
	tMsg.destEntityId = 0;
	tMsg.srcEntityId = GetEntityId();
	tMsg.srcEntityType= GetEntityType();
	tMsg.srcTaskId = 0;
	tMsg.taskId = 0;
	tMsg.msgType = ONTIMESYN_MSGTYPE;

	CMsgBody * pMsgBody = new CClientMsgBody(GenPacketHelper.GetBuf(), GenPacketHelper.GetRealBufLen());
	GenPacketHelper.DisAttachBuf();
	CMsg *pMsg = new CMsg(tMsg, pMsgBody);
	CMyserver::GetInstance()->PutMsgToThread(pMsg);
}

