/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: LogoutCmd.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-06
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-06			1.0			
* =======================================================================
*/
#include "LogoutCmd.h"
#include "pinglongconnectcmd.h"
#include "CmdCounter.h"

CLogoutCmd::CLogoutCmd(char *pBuf, int nLen, int nPublicIp, short nPublicPort, int nEntityType, int nEntityId)
: CPingSrvBaseCommand<TLogoutCmd>(pBuf, nLen , nPublicIp, nPublicPort, nEntityType, nEntityId)
{
	m_nIfRes = 0;
}

int CLogoutCmd::ProcessCmd()
{
	/////Buck Add statistics//////
	CCmdCounter::GetHandler()->Count(LOGOUTCMD);
	
	int nRet = 0;
	do 
	{
		nRet = m_pHashTable->DeletePeer(m_in.strPeerId);
		if (!nRet)
		{
			break;
		}
		else
		{
			nRet = 0;
		}
		TLogoutSynCmd LogoutSynCmd;
		LogoutSynCmd.nInternalIp = m_in.nInternalIp;
		LogoutSynCmd.strPeerId = m_in.strPeerId;
		int nBufLen =  LogoutSynCmd.GetSize() + TSynBaseCommand::GetSize();
		char *pBuf = new char[nBufLen];
		CBufMgr tmpBufMgr(pBuf, nBufLen);
		CSynEncodeCommand<TLogoutSynCmd> EncodeCmd(pBuf, nBufLen, LogoutSynCmd, MSG_LONGCONNECT_SYN_LOGOUT);
		if (EncodeCmd.Encode() != 0)
		{
			nRet = -1;
			break;
		}
		if (!m_pServerConf)
		{
			nRet = -1;
			break;
		}
		if (m_pServerConf->GetConf().bUseMulticast)
		{
			TransMsg(EncodeCmd.GetPacket(), (unsigned)EncodeCmd.GetPacketLen(), -1, -1);
			WRITE_TRACE("Trans LogMsg to Rserver&Qserver");
		}
		else
		{
			int nRServerNum = (int)m_pServerConf->GetConf().vecResourceSrv.size();
			for (int iA = 0; iA < nRServerNum; iA ++)
			{
				TransMsg(EncodeCmd.GetPacket(), (unsigned)EncodeCmd.GetPacketLen(), iA, RSERVERMACHINETYPE);
				WRITE_TRACE("Trans LogMsg to Rserver [%d]", iA);
			}
			int nQserverNum = (int)m_pServerConf->GetConf().vecQuervSrv.size();
			for (int iA = 0; iA < nQserverNum; iA ++)
			{
				TransMsg(EncodeCmd.GetPacket(), (unsigned)EncodeCmd.GetPacketLen(), iA, QSERVERMACHINETYPE);
				WRITE_TRACE("Trans LogMsg to Qserver [%d]", iA);
			}
		}

		/////Buck Add statistics//////
		CCmdCounter::GetHandler()->Count(SYNLOGOUTCMD);
	} while (0);
	
	return nRet;
}
