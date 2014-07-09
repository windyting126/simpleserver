/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: ExternalCheckCmd.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2010-03-05
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2010-03-05			1.0			
* =======================================================================
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ExternalCheckCmd.h"
#include "protocol.h"
#include "PingServerDef.h"

int CExternalCheckCmd::Process()
{
	int nRet = 0;
	switch(m_nMsgType)
	{
	case ONLINE_CHECK_PEERSTORAGEINFO:
		{
			if(m_pPeerHashTable == NULL)
			{
				WriteRunInfo::WriteLog("External Check : Get OnlinePeerStorage Handler Failed!");
				goto ERROR_RETURN;
			}
			else
			{
				TECOnlinePeerStorageRes ECOnlinePeerStorageRes;
				TMaintainHeadRes MaintainHeadRes;
				MaintainHeadRes.nMsgType = m_nMsgType;
				MaintainHeadRes.nMsgBodyLen =  ECOnlinePeerStorageRes.GetSize() + 1;
				MaintainHeadRes.cReslut = ON_RESULT_OK;

				ECOnlinePeerStorageRes.uiHashLength = m_pPeerHashTable->GetHashLength();
				ECOnlinePeerStorageRes.usOnlineServerNum = 1;
				ECOnlinePeerStorageRes.usOnlineServerIndex = 0;
				ECOnlinePeerStorageRes.uiPeerNum = m_pPeerHashTable->GetNodeNum();
				int iBufLen = MaintainHeadRes.GetSize() + MaintainHeadRes.nMsgBodyLen - 1;
				char *pBuf = new char[iBufLen];
				m_sSendBuf.AttachBuf( pBuf , iBufLen );
				MaintainHeadRes.Encode( m_sSendBuf );
				ECOnlinePeerStorageRes.Encode( m_sSendBuf );			

			}
			break;
		}
	case ONLINE_CHECK_PEERINFO:
		{
			if(m_pPeerHashTable == NULL)
			{
				WriteRunInfo::WriteLog("External Check : Get OnlinePeerStorage Handler Failed!");
				goto ERROR_RETURN;
			}
			else
			{
				TECOnlinePeerCheck ECOnlinePeerCheck;
				if(  ECOnlinePeerCheck.Decode( m_sRecvBuf ) < 0 ) 
				{
					WriteRunInfo::WriteLog("External Check : Decode ECOnlinePeerCheck  Failed!");
					goto ERROR_RETURN;
				}

				TECOnlinePeerCheckRes ECOnlinePeerCheckRes;
				memcpy( ECOnlinePeerCheckRes.szPeerId , ECOnlinePeerCheck.szPeerId , sizeof( ECOnlinePeerCheckRes.szPeerId ) );
				TMaintainHeadRes MaintainHeadRes;
				MaintainHeadRes.nMsgType = m_nMsgType;
				MaintainHeadRes.nMsgBodyLen = ECOnlinePeerCheckRes.GetSize() + 1;
				MaintainHeadRes.cReslut = ON_RESULT_OK;

				TPingPeerInfo PeerInfo;
				int iResult = m_pPeerHashTable->FindPeer(string(ECOnlinePeerCheckRes.szPeerId, sizeof( ECOnlinePeerCheckRes.szPeerId )), PeerInfo);
				if(iResult == 1)
				{
					ECOnlinePeerCheckRes.uiInternalIp = PeerInfo.nInternalIp;
					ECOnlinePeerCheckRes.usInternalPort = PeerInfo.nLocalBindPort;
					ECOnlinePeerCheckRes.uiPublicIp = PeerInfo.nExternalIp;
					ECOnlinePeerCheckRes.usPublicPort = PeerInfo.nExternalPort;
					ECOnlinePeerCheckRes.iAttribute = 0;
					ECOnlinePeerCheckRes.cLogoutFlag = PeerInfo.cLogoutFlag;
				}
				else
				{
					ECOnlinePeerCheckRes.uiInternalIp = 0;
					ECOnlinePeerCheckRes.usInternalPort = 0;
					ECOnlinePeerCheckRes.uiPublicIp = 0;
					ECOnlinePeerCheckRes.usPublicPort = 0;
					ECOnlinePeerCheckRes.iAttribute = 0;
					ECOnlinePeerCheckRes.cLogoutFlag = 0;
				}				

				int iLength = MaintainHeadRes.GetSize() + MaintainHeadRes.nMsgBodyLen - 1;
				char *pBuf = new char[iLength];
				m_sSendBuf.AttachBuf( pBuf , iLength );
				MaintainHeadRes.Encode( m_sSendBuf );
				ECOnlinePeerCheckRes.Encode( m_sSendBuf );
			}
			break;
		}
	case ONLINE_DUMP_HASHTABLE:
		{
			nRet = DumpHashTable();

			TMaintainHeadRes MaintainHeadRes;
			MaintainHeadRes.nMsgType = m_nMsgType;
			MaintainHeadRes.nMsgBodyLen = 1;
			MaintainHeadRes.cReslut = (nRet == 0)?ON_RESULT_OK:ON_RESULT_FAILE;

			int iBufLen = MaintainHeadRes.GetSize();
			char *pBuf = new char[iBufLen];
			m_sSendBuf.AttachBuf( pBuf , iBufLen );
			MaintainHeadRes.Encode( m_sSendBuf );

			break;
		}
	case ONLINE_LOAD_HASHTABLE:
		{
			nRet = LoadHashTable();

			TMaintainHeadRes MaintainHeadRes;
			MaintainHeadRes.nMsgType = m_nMsgType;
			MaintainHeadRes.nMsgBodyLen = 1;
			MaintainHeadRes.cReslut = (nRet == 0)?ON_RESULT_OK:ON_RESULT_FAILE;

			int iBufLen = MaintainHeadRes.GetSize();
			char *pBuf = new char[iBufLen];
			m_sSendBuf.AttachBuf( pBuf , iBufLen );
			MaintainHeadRes.Encode( m_sSendBuf );
			if (nRet == 0)
			{
				SendMsg(PINGONTIMESYN_ENTITY_TYPE, RANDOMENTITYID, 0, ONTIMESYN_MSGTYPE, NULL);
				WriteRunInfo::WriteLog("External Check : LoadHashTable Suc And StartRsync");
			}

			break;
		}
	default:
		{
			return CBaseMaintainCmd::Process();
			break;
		}
	}
	return 0;

ERROR_RETURN:
	TMaintainHeadRes MaintainHeadRes;
	MaintainHeadRes.nMsgType = m_nMsgType;
	MaintainHeadRes.nMsgBodyLen = 1;
	MaintainHeadRes.cReslut = ON_RESULT_FAILE;
	int iBufLen = MaintainHeadRes.GetSize();
	char *pBuf = new char[iBufLen];
	m_sSendBuf.AttachBuf( pBuf , iBufLen );
	MaintainHeadRes.Encode( m_sSendBuf );
	return -1;
}

int CExternalCheckCmd::DumpHashTable()
{
	if(m_pPeerHashTable == NULL)
	{
		WriteRunInfo::WriteLog("DumpHashTable: Get OnlinePeerStorage Handler Failed!");
		return -1;
	}
	
	int nHashLen = m_pPeerHashTable->GetHashLength();
	int iOpenFlag = O_CREAT | O_WRONLY | O_TRUNC;
	string strTmpFileName = "install/HashTable";
	string strRandString = "";
	CToolKit::GenRandomStr(strRandString, 10);
	strTmpFileName += strRandString;
	int iFd = open(strTmpFileName.c_str(), iOpenFlag);
	if(iFd == -1)
	{
		WriteRunInfo::WriteLog("Open file %s failed", strTmpFileName.c_str());
		return -1;
	}
	string strHeadInfo = "HashNum PeerIdHex (PeerId) ExIp ExPort UpnpIp UpnpPort InIp TcpPort LocalPort";
	strHeadInfo += " UpPipeState MaxDownSpeed MaxUpSpeed UpPipeNum UpdateTime LogoutFlag\n";
	write(iFd, strHeadInfo.c_str(), strHeadInfo.length());
	for (int nHash = 0; nHash < nHashLen; ++nHash)
	{
		list<TPingPeerInfo> lstPingPeerInfo;
		m_pPeerHashTable->GetPeerList(lstPingPeerInfo, nHash);

		string strPeerInfo;
		char szHashNum[20];
		snprintf(szHashNum, sizeof(szHashNum), "%u", nHash);
		for(list<TPingPeerInfo>::iterator itrA = lstPingPeerInfo.begin(); itrA != lstPingPeerInfo.end(); itrA ++)
		{
			//Hash值/////////////////////////////////////////////////
			strPeerInfo = szHashNum;

			//PeerId/////////////////////////////////////////////////
			strPeerInfo += " ";
			strPeerInfo += CToolKit::BiToHex(itrA->strPeerId.c_str(), itrA->strPeerId.length());

			strPeerInfo += " ";
			strPeerInfo += "(" +  itrA->strPeerId + ")";
			char szNum[20];

			//Ip及端口/////////////////////////////////////////////////
			snprintf(szNum, sizeof(szNum), "%u", itrA->nExternalIp);
			strPeerInfo += " ";
			strPeerInfo += szNum;

			snprintf(szNum, sizeof(szNum), "%u", (unsigned short)itrA->nExternalPort);
			strPeerInfo += " ";
			strPeerInfo += szNum;

			snprintf(szNum, sizeof(szNum), "%u", itrA->nUpnpMapdeIp);
			strPeerInfo += " ";
			strPeerInfo += szNum;

			snprintf(szNum, sizeof(szNum), "%u", (unsigned short)itrA->nUpnpMapedPort);
			strPeerInfo += " ";
			strPeerInfo += szNum;

			snprintf(szNum, sizeof(szNum), "%u", itrA->nInternalIp);
			strPeerInfo += " ";
			strPeerInfo += szNum;

			snprintf(szNum, sizeof(szNum), "%u", (unsigned short)itrA->nTcpPort);
			strPeerInfo += " ";
			strPeerInfo += szNum;

			snprintf(szNum, sizeof(szNum), "%u", (unsigned short)itrA->nLocalBindPort);
			strPeerInfo += " ";
			strPeerInfo += szNum;

			//PeerStatus/////////////////////////////////////////////////
			snprintf(szNum, sizeof(szNum), "%u", (unsigned char)itrA->nCurUploadPipeStatus);
			strPeerInfo += " ";
			strPeerInfo += szNum;

			snprintf(szNum, sizeof(szNum), "%u", (unsigned short)itrA->nMaxDownloadSpeed);
			strPeerInfo += " ";
			strPeerInfo += szNum;

			snprintf(szNum, sizeof(szNum), "%u", (unsigned short)itrA->nMaxUploadSpeed);
			strPeerInfo += " ";
			strPeerInfo += szNum;


			snprintf(szNum, sizeof(szNum), "%u", itrA->nCurUploadPipeNum);
			strPeerInfo += " ";
			strPeerInfo += szNum;

			//上报时间和超时/////////////////////////////////////////////////
			snprintf(szNum, sizeof(szNum), "%llu", itrA->ullLastReportTime);
			strPeerInfo += " ";
			strPeerInfo += szNum;

			snprintf(szNum, sizeof(szNum), "%u", (unsigned char)itrA->cLogoutFlag);
			strPeerInfo += " ";
			strPeerInfo += szNum;

			strPeerInfo += "\n";
			write(iFd, strPeerInfo.c_str(), strPeerInfo.length());
		}
	}
	close(iFd);
	if (rename(strTmpFileName.c_str(), "install/HashTable.txt") != 0)
	{
		remove(strTmpFileName.c_str());
		WriteRunInfo::WriteLog("rename file %s failed", strTmpFileName.c_str());
		return -1;
	}

	return 0;
}

int CExternalCheckCmd::LoadHashTable()
{
	if(m_pPeerHashTable == NULL)
	{
		WriteRunInfo::WriteLog("CExternalCheckCmd::LoadHashTable : Get OnlinePeerStorage Handler Failed!");
		return -1;
	}
	
	int nHashLen = m_pPeerHashTable->GetHashLength();
	
	CLoadHashTable LoadHash("install/HashTable.txt");
	int nRet = LoadHash.Init();
	if (nRet != 0)
	{
		WriteRunInfo::WriteLog("CExternalCheckCmd::LoadHashTable : ");
	}

	unsigned int nHashNum = 0;
	unsigned int nLastHashNum = 0;
	list<TPingPeerInfo> lstPingPeerInfo;
	string strLine;
	bool bIfJumpHeadLine = false;
	while ((strLine = LoadHash.GetLine()) != "")
	{
		if (!bIfJumpHeadLine)
		{
			bIfJumpHeadLine = true;
			continue;
		}
		
		vector<string> vecSplitItem;
		CToolKit::SplitString(strLine.c_str(), " ", vecSplitItem);
		if (vecSplitItem.size() != 16)
		{
			WriteRunInfo::WriteLog("LoadHashTable: error line [%s]!", strLine.c_str());
			continue;	//恶性攻击下，存在非可见的peerid
		}
		TPingPeerInfo PingPeerInfo;
		nHashNum = strtoull(vecSplitItem[0].c_str(), NULL, 10);

		if (nLastHashNum != nHashNum)
		{
			m_pPeerHashTable->UpdatePeerList(lstPingPeerInfo, nLastHashNum);
			lstPingPeerInfo.clear();
			for (int nA = nLastHashNum + 1; nA < (int)nHashNum; ++ nA)
			{
				m_pPeerHashTable->UpdatePeerList(lstPingPeerInfo, nA);
			}
			nLastHashNum = nHashNum;
		}
		PingPeerInfo.strPeerId = CToolKit::HexToBi(vecSplitItem[1].c_str(), vecSplitItem[1].length());
		PingPeerInfo.nExternalIp =  (int)strtoull(vecSplitItem[3].c_str(), NULL, 10);
		PingPeerInfo.nExternalPort =  (short)strtoull(vecSplitItem[4].c_str(), NULL, 10);
		PingPeerInfo.nUpnpMapdeIp = (int)strtoull(vecSplitItem[5].c_str(), NULL, 10);
		PingPeerInfo.nUpnpMapedPort = (short)strtoull(vecSplitItem[6].c_str(), NULL, 10);
		PingPeerInfo.nInternalIp = (int)strtoull(vecSplitItem[7].c_str(), NULL, 10);
		PingPeerInfo.nTcpPort = (short)strtoull(vecSplitItem[8].c_str(), NULL, 10);
		PingPeerInfo.nLocalBindPort = (short)strtoull(vecSplitItem[9].c_str(), NULL, 10);
		PingPeerInfo.nCurUploadPipeStatus = (char)strtoull(vecSplitItem[10].c_str(), NULL, 10);
		PingPeerInfo.nMaxDownloadSpeed = (short)strtoull(vecSplitItem[11].c_str(), NULL, 10);
		PingPeerInfo.nMaxUploadSpeed = (short)strtoull(vecSplitItem[12].c_str(), NULL, 10);
		PingPeerInfo.nCurUploadPipeNum = (unsigned char)strtoull(vecSplitItem[13].c_str(), NULL, 10);
		PingPeerInfo.ullLastReportTime = (unsigned long long)strtoull(vecSplitItem[14].c_str(), NULL, 10);
		PingPeerInfo.cLogoutFlag = (char)strtoull(vecSplitItem[15].c_str(), NULL, 10);
		lstPingPeerInfo.push_back(PingPeerInfo);
	}

	int nErrCode = LoadHash.GetLastError();
	if (nErrCode != 0)
	{
		WriteRunInfo::WriteLog("CExternalCheckCmd::LoadHashTable : Load Hash Table Fail");
		return -1;
	}
	m_pPeerHashTable->UpdatePeerList(lstPingPeerInfo, nLastHashNum);
	lstPingPeerInfo.clear();
	for (int nHash = nLastHashNum + 1; nHash < nHashLen; ++ nHash)
	{
		m_pPeerHashTable->UpdatePeerList(lstPingPeerInfo, nHash);
	}
	return 0;
}



int CLoadHashTable::Init()
{
	int nOpenFlag =  O_RDONLY;
	m_nFileFd = open(m_strFileName.c_str(), nOpenFlag);
	if (m_nFileFd == -1)
	{
		WriteRunInfo::WriteLog("CLoadHashTable::Init : open file %s Failed!", m_strFileName.c_str());
		return -1;
	}
	m_bIfOpenFile = true;

	struct stat FileStat;
	if (stat(m_strFileName.c_str(), &FileStat) != 0)
	{
		WriteRunInfo::WriteLog("CLoadHashTable::Init: stat file %s Failed!", m_strFileName.c_str());
		return -1;
	}

	m_nFileLength = FileStat.st_size;
	m_pBuf = new char[E_READ_BUFSIZE];
	return 0;
}

string CLoadHashTable::GetLine()
{
	string strLine = "";
	if (m_nListSize > 0)
	{
		strLine = m_lstLine.front();
		m_lstLine.pop_front();
		m_nListSize --;
		return strLine;
	}

	if (m_nCurOffset >= m_nFileLength)
	{
		return strLine;
	}

	int nReadLen = read(m_nFileFd, m_pBuf, E_READ_BUFSIZE);
	if (nReadLen <= 0)
	{
		WriteRunInfo::WriteLog("CLoadHashTable::GetLine: read file %s Failed!", m_strFileName.c_str());
		m_nLastError = -1;
		return strLine;
	}
	m_nCurOffset += nReadLen;

	m_strData.append(m_pBuf, nReadLen);
	int nDataLen = (int)m_strData.length();
	int nCurPos = 0;
	const char* pData = m_strData.c_str();
	for (int nA = 0; nA < nDataLen; ++ nA)
	{
		if (*(pData + nA) == '\n')
		{
			if (nCurPos == nA)
			{
				nCurPos ++;
				continue;
			}
			else
			{
				m_lstLine.push_back(string(pData + nCurPos, nA - nCurPos));
				m_nListSize ++;
				nCurPos = nA + 1;
			}
		}
	}

	if (nCurPos < nDataLen)
	{
		m_strData.assign(pData + nCurPos, nDataLen - nCurPos);
		if (m_strData.length() > MAX_LINE)
		{
			WriteRunInfo::WriteLog("CLoadHashTable::GetLine: too long line!");
			m_nLastError = -1;
			return strLine;
		}		
	}
	if (m_nListSize <= 0)
	{
		WriteRunInfo::WriteLog("CLoadHashTable::GetLine: wrong format!");
		m_nLastError = -1;
		return strLine;
	}
	else
	{
		strLine = m_lstLine.front();
		m_lstLine.pop_front();
		m_nListSize --;
		return strLine;
	}
}

void CExternalCheckCmd::SendMsg(int destEntityType, int destEntityId, ULONG64 lTaskId, int msgType,  CMsgBody *pMsgBody, ULONG64 lSrcTaskId)
{
	TMsg tMsg;
	tMsg.destEntityType = destEntityType;
	tMsg.destEntityId = destEntityId;
	tMsg.srcEntityId = RANDOMENTITYID;
	tMsg.srcEntityType= -1;
	tMsg.srcTaskId = lSrcTaskId;
	tMsg.taskId = lTaskId;
	tMsg.msgType = msgType;
	CMsg *pMsg = new CMsg(tMsg, pMsgBody);
	CMyserver::GetInstance()->PutMsgToThread(pMsg);
}

