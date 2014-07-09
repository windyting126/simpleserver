#include "pingsynprocessthread.h"
#include "pingpeerhashtable.h"
#include "rsrvdef.h"
#include "pinglongconnectcmd.h"

CPingSynProcessThread::CPingSynProcessThread(int iEntityType, int iEntityId)
	: CSynWorkThread(iEntityType, iEntityId), m_pPeerHashTable(NULL)
{
}

int CPingSynProcessThread::Init()
{
	if (CSynWorkThread::Init() < 0)
	{
		return -1;
	}

	m_pPeerHashTable = CPingPeerHashTable::GetHashTable();
	return 0;
}

int CPingSynProcessThread::Process(CMsg *pMsg)
{
	TMsg *pTMsg = pMsg->GetTMsg();
	CBufMsgBody *pMsgBody = (CBufMsgBody*)pMsg->GetMsgBody();
	if (!pTMsg || !pMsgBody)
	{
		return -1;
	}

	char *pBuf = pMsgBody->GetBuf();
	int iLength = pMsgBody->Size();
	if (!pBuf || iLength < SYN_MSG_HEAD_LEN)
	{
		return -1;
	}

	int nSrcEntityType = pTMsg->srcEntityType;
	if ((nSrcEntityType == LONGCONNECTEDPROCESSENTITY || nSrcEntityType == UDP_MRECV_ENTITY_TYPE)
		&& pTMsg->msgType == PING_SYN_PROCESS_MSG_TYPE)
	{
		char nCmd = *(pBuf + SYN_CMDINFO_OFF);
		switch (nCmd)
		{
			case MSG_LONGCONNECT_SYN_PING:
			{
				ProcessSynPing(pBuf, iLength);
				break;
			}
			case MSG_LONGCONNECT_SYN_LOGOUT:
			{
				ProcessSynLogout(pBuf, iLength);
				break;
			}
			case MSG_LONGCONNECT_SYN_ONTIME:
			{
				ProcessSynOntime(pBuf, iLength);
				break;
			}
			default:
			{
				WriteRunInfo::WriteError("Ping Syn Recv Message type is wrong, %d", nCmd);
				break;
			}
		}
	}

	return 0;
}

int CPingSynProcessThread::ProcessSynPing(char *pBuf, int iLength)
{
	TPingSynCmd PingSynCmd;
	CSynDecodeCommand<TPingSynCmd> DecodeCmd(pBuf, iLength, PingSynCmd);
	if (DecodeCmd.Decode() != 0)
	{
		return -1;
	}
	m_pPeerHashTable->AddPeer(PingSynCmd.m_PeerInfo);
	WRITE_TRACE("ProcessSynPing Succceed");
	return 0;	
}

int CPingSynProcessThread::ProcessSynLogout(char *pBuf, int iLength)
{
	TLogoutSynCmd LogoutSynCmd;
	CSynDecodeCommand<TLogoutSynCmd> DecodeCmd(pBuf, iLength, LogoutSynCmd);
	if (DecodeCmd.Decode() != 0)
	{
		return -1;
	}
	m_pPeerHashTable->SetLogoutFlag(LogoutSynCmd.strPeerId);
	WRITE_TRACE("ProcessSynLogout Succceed");
	return 0;	
}

int CPingSynProcessThread::ProcessSynOntime(char *pBuf, int iLength)
{
	TOntimeSynCmd OntimeSynCmd;
	CSynDecodeCommand<TOntimeSynCmd> DecodeCmd(pBuf, iLength, OntimeSynCmd);
	if (DecodeCmd.Decode() == 0)
	{
		if (OntimeSynCmd.nHashBucketLen != m_pPeerHashTable->GetHashLength())
		{
			WriteRunInfo::WriteError("syn ontime fail, hash bucket length not matched: %d %d",
									 OntimeSynCmd.nHashBucketLen, m_pPeerHashTable->GetHashLength());
			return -1;
		}
		int nHashNum = 0;
		int nPeerNum = 0;
		int nResult = 0;
		while (1)
		{
			nResult = OntimeSynCmd.DecodeGetPeerList(DecodeCmd.GetBufMgr(), nHashNum, nPeerNum);
			if (nResult == 0)
			{
				m_pPeerHashTable->UpdatePeerList(OntimeSynCmd.lstPeerList, nHashNum);
			}
			else
			{
				break;
			}			
		}
		m_pPeerHashTable->PrintNodeNumber();
	}
	WRITE_TRACE("ProcessSynOntime Succceed");
	return 0;
}

