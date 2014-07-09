/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: UdpProcAndSendThread.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-07-24
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-07-24			1.0			
* =======================================================================
*/
#include "framecommon/framecommon.h"

#include "UdpProcAndSendThread.h"
#include "PingSrvClassFactory.h"
#include "PingServer.h"
#include "CmdCounter.h"

using namespace MYFRAMECOMMON;


CUdpProcAndSendThread::CUdpProcAndSendThread(int iEntityType, int iEntityId)
: CUdpCommonSendThread(iEntityType, iEntityId)
{

}

int CUdpProcAndSendThread::Run()
{
	while (m_nRunFlag)
	{
		CMsg *pMsg = m_InputList.get();
		if (pMsg != NULL)
		{
			Process(pMsg);
			if(IfHaveDataToSend())
			{
				CUdpCommonBufMsgBody *pMsgBody = (CUdpCommonBufMsgBody *)pMsg->GetMsgBody();
				CPingServer *pServer = (CPingServer *)CMyserver::GetInstance();
				if(pMsgBody != NULL && pServer != NULL && m_pSendBuf != NULL && m_iSendPackecLen > 0)
				{
					char *pEncryptBuf = new char[m_iSendPackecLen + MHXY_ENCRYPT_EXTRA_LEN]; 
					int nEncryptLen = m_iSendPackecLen + MHXY_ENCRYPT_EXTRA_LEN;
					int nEncryptAlgorithmType = pServer->GetConf().GetConf().nEncryptType;
					WRITE_DAT(m_pSendBuf, m_iSendPackecLen, "////Not Encrypted Packet////");
					WRITE_TRACE("Send Back Udp Packet");
					mhxy_udp::encrypt(nEncryptAlgorithmType, (unsigned char*)m_pSendBuf, m_iSendPackecLen, (unsigned char*)pEncryptBuf, (unsigned&)nEncryptLen);
					m_UdpSock.UdpSend(pMsgBody->GetSocket(), (char *)pEncryptBuf, nEncryptLen, pMsgBody->GetClientIp(), pMsgBody->GetCientPort());
					DelSendBuf();
					delete [] pEncryptBuf;
				}
				ClearSendDataFlag();
			}
			delete pMsg;
		}
	}
	return 0;
}

int CUdpProcAndSendThread::MsgProcess(CMsg * pMsg)
{

	WRITE_TRACE ("Recv a msg from client ...");
	int nRes = 0;
	CUdpCommonBufMsgBody *pBufMsgBody = (CUdpCommonBufMsgBody *)pMsg->GetMsgBody();
	if (!pBufMsgBody)
	{
		WriteRunInfo::WriteError("pBufMsgBody is NULL");
		return -1;
	}

	char *pBuf = pBufMsgBody->GetBuf();
	int nLen = pBufMsgBody->Size();

	CPingServer *pServer = (CPingServer *)CMyserver::GetInstance();
	if (!pServer)
	{
		WriteRunInfo::WriteError("pServer is NULL");
		return -1;
	}
	CPingSrvClassFactory *pFactory = (CPingSrvClassFactory *)pServer->GetClassFactory();
	if (!pFactory)
	{
		WriteRunInfo::WriteError("pFactory is NULL");
		return -1;
	}

	CCommand *pCmd = pFactory->GenBaseCommand(pBuf, nLen, pBufMsgBody->GetClientIp(), pBufMsgBody->GetCientPort(), m_nEntityType, m_nEntityId);
	if (NULL == pCmd)
	{
		WRITE_TRACE ("Gen Base Command Failed!");
		/////Buck Add statistics//////
		CCmdCounter::GetHandler()->Count(INVALIDCMD);
		return -1;
	}


	int nResult = 0;
	nRes = pCmd->Process(nResult);
	//WRITE_TRACE ("Client msg, process ret is %d", nRes);
	if(nRes != 0)
	{
		/////Buck Add statistics//////
		CCmdCounter::GetHandler()->Count(INVALIDCMD);
	}
	if (pCmd->GetIfRes() && (0 == nRes))
	{
		//int nSendBufLen;
		m_pSendBuf = pCmd->GetResBuf (m_iSendPackecLen);
		if(m_pSendBuf && m_iSendPackecLen >0 && m_iSendPackecLen <= MAX_UDP_SEND_PACKET_LEN - MHXY_ENCRYPT_EXTRA_LEN )
		{
			SetSendDataFlag();
		}
		else
		{
			DelSendBuf();
		}
	}
	
	delete pCmd;
	return nRes;
}
