/*
 * =====================================================================================
 *    @file       udprecvthread.h
 *
 *    @brief      base udp recv thread
 *    @details    if this kind threads use reused socket and the reused
 *			      address is the same with the reused socket of udpsendthread,
 *			      threads of this kind must start after udpsendthreads.
 *
 *    @version    1.0
 *    @date       10/31/2012 01:22:43 PM
 *
 *    @author     wangtiefeng (), wangtf418@163.com
 *    @copyright  
 *
 * =====================================================================================
 */
#ifndef _UDPRECVTHREAD_H_912CAA4D_424C_46A8_909C_9458C1C9E858_
#define _UDPRECVTHREAD_H_912CAA4D_424C_46A8_909C_9458C1C9E858_

#include <string>
#include "workthread.h"
#include "framecommon/framecommon.h"
using namespace MYFRAMECOMMON;
using namespace std;

class CUdpRecvThread : public CWorkThread
{
public:
	CUdpRecvThread(int iEntityType,  int iEntityId, CUdpCommonSock* pUdpRecvSocket);
	virtual ~CUdpRecvThread();

public:
	virtual int Init();
	virtual int Run();
	virtual int PutMsgToInputList(CMsg* pMsg);

protected:
	virtual int Process(CMsg* pMsg);

protected:
	CUdpCommonSock* m_pUdpSocket;
	char m_RecvBuf[MAX_UDP_RECV_PACKET_LEN];
};


#endif


