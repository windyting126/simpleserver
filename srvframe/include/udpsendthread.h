/*
 * =====================================================================================
 *    @file       udpsendthread.h
 *
 *    @brief      base udp send thread
 *    @details    if this kind threads use reused socket and the reused,
 *                address is the same with the reused socket of udpsendthread,
 *                hreads of this kind must start before udprecvthreads.
 *
 *    @version    1.0
 *    @date       10/31/2012 02:41:19 PM
 *
 *    @author     wangtiefeng (), wangtf418@163.com
 *    @copyright
 *
 * =====================================================================================
 */
#ifndef _UDPSENDTHREAD_H_6C694B4E_E088_47A7_8C04_D7AEDB0D68AE_
#define _UDPSENDTHREAD_H_6C694B4E_E088_47A7_8C04_D7AEDB0D68AE_

#include <string>
#include "framecommon/framecommon.h"
#include "workthread.h"
#include "synqueue.h"
using namespace MYFRAMECOMMON;
using namespace std;

class CUdpSendThread : public CWorkThread
{
public:
	CUdpSendThread(int iEntityType,  int iEntityId, const string& strIp, unsigned short usPort);
	virtual ~CUdpSendThread();

public:
	virtual int Init();
	virtual int Run();
	virtual int PutMsgToInputList(CMsg* pMsg);
	int GetMsgQueueLen();

protected:
	virtual int Process(CMsg* pMsg);

protected:
	CUdpCommonSock m_UdpSocket;
	CQueue m_InputList;
};


#endif


