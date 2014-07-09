/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: pingpeerhashtable.cpp
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
#include "pingpeerhashtable.h"
CPingPeerHashTable * CPingPeerHashTable::m_pHashTable = NULL;

int CPingPeerHashTable::AddPeer(TPingPeerInfo &PeerInfo)
{
	int nTmpIndex = PeerInfo.GetIndex();
	int nRealIndex = nTmpIndex%m_nHashBucketLen;
	PeerInfo.ullLastReportTime = m_pServer->GetTimeVal().tv_sec;
	HASHLOCK(nRealIndex)
	HashIterator listItr = m_list[nRealIndex].begin();
	for (; listItr != m_list[nRealIndex].end(); listItr++)
	{	
		if (PeerInfo == *listItr)
		{
			break;
		}
	}
	int nRet = 0;
	if (listItr == m_list[nRealIndex].end())
	{
		if (PeerInfo.nCurUploadPipeStatus == -1)
		{
			PeerInfo.nCurUploadPipeStatus = GetUploadPipeState(PeerInfo.nCurUploadPipeNum);
		}		
		m_list[nRealIndex].push_back(PeerInfo);
		m_nNodeNum++;
		
		nRet = 1;
		if (WriteRunInfo::LOGTYPEARRAY[TRACETYPE].nLogOff)
		{
			WriteRunInfo::WriteTrace("Add New Peer : %s (%s)", CToolKit::BiToHex(PeerInfo.strPeerId.c_str(), PeerInfo.strPeerId.length()).c_str(), PeerInfo.strPeerId.c_str());
			WriteRunInfo::WriteTrace("         InternalIp= %u", PeerInfo.nInternalIp);
			WriteRunInfo::WriteTrace("         nLocalBindPort= %u", (unsigned short)PeerInfo.nLocalBindPort);
			WriteRunInfo::WriteTrace("         nTcpPort= %u", (unsigned short)PeerInfo.nTcpPort);
			WriteRunInfo::WriteTrace("         nUpnpMapdeIp= %u", PeerInfo.nUpnpMapdeIp);
			WriteRunInfo::WriteTrace("         nUpnpMapedPort= %u", (unsigned short)PeerInfo.nUpnpMapedPort);
			WriteRunInfo::WriteTrace("         ExternalIp= %u", PeerInfo.nExternalIp);
			WriteRunInfo::WriteTrace("         ExternalPort= %u", (unsigned short)PeerInfo.nExternalPort);
			WriteRunInfo::WriteTrace("         nMaxDownloadSpeed= %u", PeerInfo.nMaxDownloadSpeed);
			WriteRunInfo::WriteTrace("         nMaxUploadSpeed= %u", PeerInfo.nMaxUploadSpeed);
			WriteRunInfo::WriteTrace("         nCurUploadPipeNum= %u", PeerInfo.nCurUploadPipeNum);
			WriteRunInfo::WriteTrace("         nCurUploadPipeStatus= %u", PeerInfo.nCurUploadPipeStatus);
			WriteRunInfo::WriteTrace("         ullLastReportTime= %llu", PeerInfo.ullLastReportTime);
			WriteRunInfo::WriteTrace("         cLogoutFlag= %u", PeerInfo.cLogoutFlag);
			WriteRunInfo::WriteTrace("         NodeNum= %u", m_nNodeNum);
		}		
	}
	else
	{
		PeerInfo.nCurUploadPipeStatus = GetUploadPipeState(PeerInfo.nCurUploadPipeNum, listItr->nCurUploadPipeStatus);
		listItr->ullLastReportTime = PeerInfo.ullLastReportTime;
		int nDifType = listItr->Differ(PeerInfo);
		if( nDifType!= DIF_SAME)
		{
			if (listItr->cLogoutFlag == LOGOUT_FLAG_YES)
			{
				m_nNodeNum++;
			}
			*listItr = PeerInfo;
			if (nDifType != DIF_UPLOADPIPENUM)
			{
				nRet = 1;
			}
			if (WriteRunInfo::LOGTYPEARRAY[TRACETYPE].nLogOff)
			{
				WriteRunInfo::WriteTrace("Update Peer : %s (%s)", CToolKit::BiToHex(PeerInfo.strPeerId.c_str(), PeerInfo.strPeerId.length()).c_str(), PeerInfo.strPeerId.c_str());
				WriteRunInfo::WriteTrace("         InternalIp= %u", PeerInfo.nInternalIp);
				WriteRunInfo::WriteTrace("         nLocalBindPort= %u", (unsigned short)PeerInfo.nLocalBindPort);
				WriteRunInfo::WriteTrace("         nTcpPort= %u", (unsigned short)PeerInfo.nTcpPort);
				WriteRunInfo::WriteTrace("         nUpnpMapdeIp= %u", PeerInfo.nUpnpMapdeIp);
				WriteRunInfo::WriteTrace("         nUpnpMapedPort= %u", (unsigned short)PeerInfo.nUpnpMapedPort);
				WriteRunInfo::WriteTrace("         ExternalIp= %u", PeerInfo.nExternalIp);
				WriteRunInfo::WriteTrace("         ExternalPort= %u", (unsigned short)PeerInfo.nExternalPort);
				WriteRunInfo::WriteTrace("         nMaxDownloadSpeed= %u", PeerInfo.nMaxDownloadSpeed);
				WriteRunInfo::WriteTrace("         nMaxUploadSpeed= %u", PeerInfo.nMaxUploadSpeed);
				WriteRunInfo::WriteTrace("         nCurUploadPipeNum= %u", PeerInfo.nCurUploadPipeNum);
				WriteRunInfo::WriteTrace("         nCurUploadPipeStatus= %u", PeerInfo.nCurUploadPipeStatus);
				WriteRunInfo::WriteTrace("         ullLastReportTime= %llu", PeerInfo.ullLastReportTime);
				WriteRunInfo::WriteTrace("         cLogoutFlag= %u", PeerInfo.cLogoutFlag);
				WriteRunInfo::WriteTrace("         NodeNum= %u", m_nNodeNum);
			}			
		}
		else
		{
			WRITE_TRACE("Peer is not changed: %s (%s)", CToolKit::BiToHex(PeerInfo.strPeerId.c_str(), PeerInfo.strPeerId.length()).c_str(), PeerInfo.strPeerId.c_str());
		}
	}
	HASHUNLOCK(nRealIndex)
	return nRet;
}

int CPingPeerHashTable::DeletePeer(const string &strPeerId)
{
	TPingPeerInfo PeerInfo;
	PeerInfo.strPeerId = strPeerId;
	int nTmpIndex = PeerInfo.GetIndex();
	int nRealIndex = nTmpIndex%m_nHashBucketLen;

	HASHLOCK(nRealIndex)
	HashIterator listItr = m_list[nRealIndex].begin();
	int nRet = 0;
	for (; listItr != m_list[nRealIndex].end(); listItr++)
	{	
		if (PeerInfo == *listItr)
		{
			m_nNodeNum--;
			if (WriteRunInfo::LOGTYPEARRAY[TRACETYPE].nLogOff)
			{
				WriteRunInfo::WriteTrace("Delete Peer : %s (%s)", CToolKit::BiToHex(listItr->strPeerId.c_str(), listItr->strPeerId.length()).c_str(), listItr->strPeerId.c_str());
				WriteRunInfo::WriteTrace("         InternalIp= %u", listItr->nInternalIp);
				WriteRunInfo::WriteTrace("         nLocalBindPort= %u", (unsigned short)listItr->nLocalBindPort);
				WriteRunInfo::WriteTrace("         nTcpPort= %u", (unsigned short)listItr->nTcpPort);
				WriteRunInfo::WriteTrace("         nUpnpMapdeIp= %u", listItr->nUpnpMapdeIp);
				WriteRunInfo::WriteTrace("         nUpnpMapedPort= %u", (unsigned short)listItr->nUpnpMapedPort);
				WriteRunInfo::WriteTrace("         ExternalIp= %u", listItr->nExternalIp);
				WriteRunInfo::WriteTrace("         ExternalPort= %u", (unsigned short)listItr->nExternalPort);
				WriteRunInfo::WriteTrace("         nMaxDownloadSpeed= %u", listItr->nMaxDownloadSpeed);
				WriteRunInfo::WriteTrace("         nMaxUploadSpeed= %u", listItr->nMaxUploadSpeed);
				WriteRunInfo::WriteTrace("         nCurUploadPipeNum= %u", listItr->nCurUploadPipeNum);
				WriteRunInfo::WriteTrace("         nCurUploadPipeStatus= %u", listItr->nCurUploadPipeStatus);
				WriteRunInfo::WriteTrace("         ullLastReportTime= %llu", listItr->ullLastReportTime);
				WriteRunInfo::WriteTrace("         cLogoutFlag= %u", listItr->cLogoutFlag);
				WriteRunInfo::WriteTrace("         NodeNum= %d", m_nNodeNum);
			}
			m_list[nRealIndex].erase(listItr);
			nRet = 1;
			break;
		}
	}
	if (listItr == m_list[nRealIndex].end())
	{
		WRITE_TRACE("Delete Peer is not in table : %s (%s)", CToolKit::BiToHex(PeerInfo.strPeerId.c_str(), PeerInfo.strPeerId.length()).c_str(), PeerInfo.strPeerId.c_str());
	}
	
	HASHUNLOCK(nRealIndex)

	return nRet;
}

int CPingPeerHashTable::FindPeer(const string &strPeerId, TPingPeerInfo &PeerInfo) const
{
	PeerInfo.strPeerId = strPeerId;
	int nTmpIndex = PeerInfo.GetIndex();
	int nRealIndex = nTmpIndex%m_nHashBucketLen;
	HASHLOCK(nRealIndex)
	HashIterator listItr = m_list[nRealIndex].begin();
	int nRet = 0;
	for (; listItr != m_list[nRealIndex].end(); listItr++)
	{	
		if (PeerInfo == *listItr)
		{
			PeerInfo = *listItr;
			nRet = 1;
			if (WriteRunInfo::LOGTYPEARRAY[TRACETYPE].nLogOff)
			{
				WriteRunInfo::WriteTrace("Find Peer : %s (%s)", CToolKit::BiToHex(PeerInfo.strPeerId.c_str(), PeerInfo.strPeerId.length()).c_str(), PeerInfo.strPeerId.c_str());
				WriteRunInfo::WriteTrace("         InternalIp= %u", PeerInfo.nInternalIp);
				WriteRunInfo::WriteTrace("         nLocalBindPort= %u", (unsigned short)PeerInfo.nLocalBindPort);
				WriteRunInfo::WriteTrace("         nTcpPort= %u", (unsigned short)PeerInfo.nTcpPort);
				WriteRunInfo::WriteTrace("         nUpnpMapdeIp= %u", PeerInfo.nUpnpMapdeIp);
				WriteRunInfo::WriteTrace("         nUpnpMapedPort= %u", (unsigned short)PeerInfo.nUpnpMapedPort);
				WriteRunInfo::WriteTrace("         ExternalIp= %u", PeerInfo.nExternalIp);
				WriteRunInfo::WriteTrace("         ExternalPort= %u", (unsigned short)PeerInfo.nExternalPort);
				WriteRunInfo::WriteTrace("         nMaxDownloadSpeed= %u", PeerInfo.nMaxDownloadSpeed);
				WriteRunInfo::WriteTrace("         nMaxUploadSpeed= %u", PeerInfo.nMaxUploadSpeed);
				WriteRunInfo::WriteTrace("         nCurUploadPipeNum= %u", PeerInfo.nCurUploadPipeNum);
				WriteRunInfo::WriteTrace("         nCurUploadPipeStatus= %u", PeerInfo.nCurUploadPipeStatus);
				WriteRunInfo::WriteTrace("         ullLastReportTime= %llu", PeerInfo.ullLastReportTime);
				WriteRunInfo::WriteTrace("         cLogoutFlag= %u", PeerInfo.cLogoutFlag);
				WriteRunInfo::WriteTrace("         NodeNum= %d", m_nNodeNum);
			}
			break;
		}
	}
	if (listItr == m_list[nRealIndex].end())
	{
		WRITE_TRACE("Find Peer is not in table : %s (%s)", CToolKit::BiToHex(PeerInfo.strPeerId.c_str(), PeerInfo.strPeerId.length()).c_str(), PeerInfo.strPeerId.c_str());
	}
	HASHUNLOCK(nRealIndex)
	return nRet;
}

int CPingPeerHashTable::GetPeerTime(const string &strPeerId, unsigned long long &ullPeerTime) const
{
	TPingPeerInfo PeerInfo;
	PeerInfo.strPeerId = strPeerId;
	int nTmpIndex = PeerInfo.GetIndex();
	int nRealIndex = nTmpIndex%m_nHashBucketLen;

	HASHLOCK(nRealIndex)
	HashIterator listItr = m_list[nRealIndex].begin();
	int nRet = 0;
	for (; listItr != m_list[nRealIndex].end(); listItr++)
	{	
		if (PeerInfo == *listItr)
		{
			ullPeerTime = listItr->ullLastReportTime;
			nRet = 1;
			break;
		}
	}		
	HASHUNLOCK(nRealIndex)
	
	return nRet;
}

int CPingPeerHashTable::SetPeerTime(const string &strPeerId, unsigned long long ullPeerTime)
{
	TPingPeerInfo PeerInfo;
	PeerInfo.strPeerId = strPeerId;
	int nTmpIndex = PeerInfo.GetIndex();
	int nRealIndex = nTmpIndex%m_nHashBucketLen;
	HASHLOCK(nRealIndex)
	HashIterator listItr = m_list[nRealIndex].begin();
	int nRet = 0;
	for (; listItr != m_list[nRealIndex].end(); listItr++)
	{	
		if (PeerInfo == *listItr)
		{
			listItr->ullLastReportTime = ullPeerTime;
			nRet = 1;
			break;
		}
	}		
	HASHUNLOCK(nRealIndex)
	return nRet;
}

int CPingPeerHashTable::SetLogoutFlag(const string &strPeerId)
{
	TPingPeerInfo PeerInfo;
	PeerInfo.strPeerId = strPeerId;
	int nTmpIndex = PeerInfo.GetIndex();
	int nRealIndex = nTmpIndex%m_nHashBucketLen;

	HASHLOCK(nRealIndex)
	HashIterator listItr = m_list[nRealIndex].begin();
	int nRet = 0;
	for (; listItr != m_list[nRealIndex].end(); listItr++)
	{	
		if (PeerInfo == *listItr)
		{
			if (listItr->cLogoutFlag == LOGOUT_FLAG_YES)
			{
				WRITE_TRACE("Set Logout Peer has log out : %s (%s)", CToolKit::BiToHex(PeerInfo.strPeerId.c_str(), PeerInfo.strPeerId.length()).c_str(), PeerInfo.strPeerId.c_str());
			}
			else
			{
				listItr->cLogoutFlag = LOGOUT_FLAG_YES;
				m_nNodeNum --;
				WRITE_TRACE("Set Logout Flag : %s (%s)", CToolKit::BiToHex(PeerInfo.strPeerId.c_str(), PeerInfo.strPeerId.length()).c_str(), PeerInfo.strPeerId.c_str());
				WRITE_TRACE("         NodeNum= %u", m_nNodeNum);
				nRet = 1;
			}			
			break;
		}
	}
	if (listItr == m_list[nRealIndex].end())
	{
		WRITE_TRACE("Set Logout Peer is not in table : %s (%s)", CToolKit::BiToHex(PeerInfo.strPeerId.c_str(), PeerInfo.strPeerId.length()).c_str(), PeerInfo.strPeerId.c_str());
	}
	HASHUNLOCK(nRealIndex)
	
	return nRet;
}

int CPingPeerHashTable::UpdatePeerList(const list<TPingPeerInfo> &listPeerInfo, int iHashIndex)
{
	if (iHashIndex < 0 || iHashIndex >= m_nHashBucketLen)
		return -1;

	HASHLOCK(iHashIndex)
	int nCurListLen = 0;
	for (list<TPingPeerInfo>::iterator itr = m_list[iHashIndex].begin(); itr != m_list[iHashIndex].end(); ++ itr)
	{
		if(itr->cLogoutFlag == LOGOUT_FLAG_NO) nCurListLen ++;
	}	
	int nUpdateListLen = listPeerInfo.size();
	m_list[iHashIndex] = listPeerInfo;
	m_nNodeNum += (nUpdateListLen - nCurListLen);
	//WRITE_TRACE("Update PeerList [%u]", iHashIndex);
	//WRITE_TRACE("         NodeNum= %u", m_nNodeNum);
	HASHUNLOCK(iHashIndex)

	return 0;
}

int CPingPeerHashTable::GetPeerList(list<TPingPeerInfo> &listPeerInfo, int iHashIndex) const
{
	if (iHashIndex < 0 || iHashIndex >= m_nHashBucketLen)
		return -1;
	listPeerInfo.clear();

	HASHLOCK(iHashIndex)
	list<TPingPeerInfo>::iterator iterTmp = m_list[iHashIndex].begin();
	while(iterTmp != m_list[iHashIndex].end())
	{
		if(iterTmp->cLogoutFlag == LOGOUT_FLAG_NO)
		{
			listPeerInfo.push_back(*iterTmp);
		}
		iterTmp++;
	}
	HASHUNLOCK(iHashIndex)
	
	return 0;
}

int CPingPeerHashTable::CheckPeerList(list<TPingPeerInfo> &listPeerInfo, int iHashIndex, int nTimeOut)
{
	if (iHashIndex < 0 || iHashIndex >= m_nHashBucketLen || nTimeOut <= 0)
		return -1;
	listPeerInfo.clear();
	unsigned long long ullCurTime = m_pServer->GetTimeVal().tv_sec;
	
	HASHLOCK(iHashIndex)
	list<TPingPeerInfo>::iterator iterTmp = m_list[iHashIndex].begin();
	list<TPingPeerInfo>::iterator iterTimeout;

	while(iterTmp != m_list[iHashIndex].end())
	{
		if(ullCurTime - iterTmp->ullLastReportTime > (unsigned)nTimeOut)
		{
			iterTimeout = iterTmp;
			iterTmp ++;
			listPeerInfo.push_back(*iterTimeout);
			m_nNodeNum --;
			if (WriteRunInfo::LOGTYPEARRAY[TRACETYPE].nLogOff)
			{
				WriteRunInfo::WriteTrace("Add Time Out Peer to List : PeerId = %s (%s)", CToolKit::BiToHex(iterTimeout->strPeerId.c_str(), iterTimeout->strPeerId.length()).c_str(), iterTimeout->strPeerId.c_str());
				WriteRunInfo::WriteTrace("         InternalIp= %u", iterTimeout->nInternalIp);
				WriteRunInfo::WriteTrace("         nLocalBindPort= %u", (unsigned short)iterTimeout->nLocalBindPort);
				WriteRunInfo::WriteTrace("         nTcpPort= %u", (unsigned short)iterTimeout->nTcpPort);
				WriteRunInfo::WriteTrace("         nUpnpMapdeIp= %u", iterTimeout->nUpnpMapdeIp);
				WriteRunInfo::WriteTrace("         nUpnpMapedPort= %u", (unsigned short)iterTimeout->nUpnpMapedPort);
				WriteRunInfo::WriteTrace("         ExternalIp= %u", iterTimeout->nExternalIp);
				WriteRunInfo::WriteTrace("         ExternalPort= %u", (unsigned short)iterTimeout->nExternalPort);
				WriteRunInfo::WriteTrace("         nMaxDownloadSpeed= %u", iterTimeout->nMaxDownloadSpeed);
				WriteRunInfo::WriteTrace("         nMaxUploadSpeed= %u", iterTimeout->nMaxUploadSpeed);
				WriteRunInfo::WriteTrace("         nCurUploadPipeNum= %u", iterTimeout->nCurUploadPipeNum);
				WriteRunInfo::WriteTrace("         nCurUploadPipeStatus= %u", iterTimeout->nCurUploadPipeStatus);
				WriteRunInfo::WriteTrace("         ullLastReportTime= %llu", iterTimeout->ullLastReportTime);
				WriteRunInfo::WriteTrace("         cLogoutFlag= %u", iterTimeout->cLogoutFlag);
				WriteRunInfo::WriteTrace("         NodeNum= %u", m_nNodeNum);
			}			
			m_list[iHashIndex].erase(iterTimeout);
		}
		else 
		{
			iterTmp++;
		}
	}
	HASHUNLOCK(iHashIndex)

	return 0;
}

char CPingPeerHashTable::GetUploadPipeState(char nCurUploadPipeNum)
{
	if (nCurUploadPipeNum < 0) return -1;
	
	if (nCurUploadPipeNum >= m_nMaxPipeNum * m_fBusyPercent)
	{
		return UPS_BUSY;
	}
	else if (nCurUploadPipeNum >= m_nMaxPipeNum * m_fNormalPercent)
	{
		return UPS_NORMAL;
	}
	else
	{
		return UPS_FREE;
	}
}

char CPingPeerHashTable::GetUploadPipeState(char nCurUploadPipeNum, char nLastState)
{
	if (nCurUploadPipeNum < 0) return -1;
	if (nCurUploadPipeNum >= m_nMaxPipeNum * m_fBusyPercent)
	{
		return UPS_BUSY;
	}
	else if (nCurUploadPipeNum >= m_nMaxPipeNum * m_fNormalPercent)
	{
		if (nLastState == UPS_BUSY && (nCurUploadPipeNum > m_nMaxPipeNum * (m_fBusyPercent - 0.05)))
		{
			return UPS_BUSY;
		}
		else
		{
			return UPS_NORMAL;
		}
		
	}
	else
	{
		if (nLastState == UPS_NORMAL && (nCurUploadPipeNum > m_nMaxPipeNum * (m_fNormalPercent - 0.05)))
		{
			return UPS_NORMAL;
		}
		else
		{
			return UPS_FREE;
		}		
	}
}


