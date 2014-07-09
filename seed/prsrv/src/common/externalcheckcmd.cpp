/* Copyright:2009-
* Version		: 1.0
* FileName		: externalcheckcmd.cpp
* Description	: 
*---- Class List	: 
*---- Function List	: 
*---- Variable List : 
*---- Related Files : 
* Author:       : wangtiefeng
* Created Date	: 2009-1-14
* 
* History		: 
*		<author>		  <time>		<version>		<description>
*		wangtiefeng		2009-3-26			1.0			
*/

#include "externalcheckcmd.h"
#include "protocol.h"


int CExternalCheckCmd::Process()
 {
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
			 if(m_pPeerHashTable == NULL)
			 {
				 WriteRunInfo::WriteLog("External Check : Get OnlinePeerStorage Handler Failed!");
				 goto ERROR_RETURN;
			 }
			 else
			 {
				 int nHashLen = m_pPeerHashTable->GetHashLength();
				 int iOpenFlag = O_CREAT | O_WRONLY | O_TRUNC;
				 int iFd = open("logs/HashTable.txt", iOpenFlag);
				 if(iFd == -1)
				 {
					 WriteRunInfo::WriteLog("Open file logs/HashTable.txt failed");
					 break;
				 }
				 string strHeadInfo = "HashNum PeerIdHex (PeerId) LogoutFlag ExternalIp UpnpMapedIp InternalIp UpPipeNum UpPipeState\n";
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
						 strPeerInfo = szHashNum;

						 strPeerInfo += " ";
						 strPeerInfo += CToolKit::BiToHex(itrA->strPeerId.c_str(), itrA->strPeerId.length());

						 strPeerInfo += " ";
						 strPeerInfo += "(" +  itrA->strPeerId + ")";
						 char szNum[20];

						 snprintf(szNum, sizeof(szNum), "%u", itrA->cLogoutFlag);
						 strPeerInfo += " ";
						 strPeerInfo += szNum;

						 snprintf(szNum, sizeof(szNum), "%u", itrA->nExternalIp);
						 strPeerInfo += " ";
						 strPeerInfo += szNum;

						 snprintf(szNum, sizeof(szNum), "%u", itrA->nUpnpMapdeIp);
						 strPeerInfo += " ";
						 strPeerInfo += szNum;

						 snprintf(szNum, sizeof(szNum), "%u", itrA->nInternalIp);
						 strPeerInfo += " ";
						 strPeerInfo += szNum;

						 snprintf(szNum, sizeof(szNum), "%u", itrA->nCurUploadPipeNum);
						 strPeerInfo += " ";
						 strPeerInfo += szNum;

						 snprintf(szNum, sizeof(szNum), "%u", itrA->nCurUploadPipeStatus);
						 strPeerInfo += " ";
						 strPeerInfo += szNum;

						 strPeerInfo += "\n";
						 write(iFd, strPeerInfo.c_str(), strPeerInfo.length());
					 }
				 }
				 close(iFd);
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

