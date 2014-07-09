/**
 * =====================================================================================
 *    @file       udpsendthread.cpp
 *
 *    @brief      
 *    @details
 *
 *    @version    1.0
 *    @date       10/31/2012 02:41:25 PM
 *
 *    @author     wangtiefeng (), wangtf418@163.com
 *    @copyright  
 *
 * =====================================================================================
 */
#include "udpsendthread.h"

CUdpSendThread::CUdpSendThread(int iEntityType,  int iEntityId, const string& strIp, unsigned short usPort)
	: CWorkThread(iEntityType, iEntityId), m_UdpSocket(strIp, usPort, false, true)
{
	m_UdpSocket.CreateUdpSocket();
}

CUdpSendThread::~CUdpSendThread()
{

}

int CUdpSendThread::Init()
{
	if (m_UdpSocket.GetSocket() == INVALID_SOCKET)
	{
		WriteRunInfo::WriteLog("UdpSendThread::Init : CreateUdpSocket fail");
		return -1;
	}
	return 0;
}

int CUdpSendThread::Run()
{
	while (m_nRunFlag)
	{
		CMsg *pMsg = m_InputList.get();
		if (!pMsg) continue;
		Process(pMsg);
		delete pMsg;
	}
	return 0;
}

int CUdpSendThread::PutMsgToInputList(CMsg* pMsg)
{
	m_InputList.put(pMsg);
	return 0;
}

int CUdpSendThread::GetMsgQueueLen()
{
	return m_InputList.size();
}

int CUdpSendThread::Process(CMsg* pMsg)
{
	CUdpCommonBufMsgBody* pMsgBody = (CUdpCommonBufMsgBody*)pMsg->GetMsgBody();
	if (pMsgBody && pMsgBody->GetBuf() && pMsgBody->Size() > 0)
	{
		m_UdpSocket.UdpSend(pMsgBody->GetBuf(), pMsgBody->Size(), pMsgBody->GetClientIp(), pMsgBody->GetCientPort());
	}
	return 0;
}




