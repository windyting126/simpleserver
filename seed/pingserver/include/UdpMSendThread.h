/**
 * @file UdpMSendThread.h
 * @author yanshi <yanshi@baofeng.com>
 * @date 2013-03-06
 */

#ifndef _UDPMSENDTHREAD_H_
#define _UDPMSENDTHREAD_H_

#include "udpsendthread.h"

/// udp multicast send sync thread.
class CUdpMSendThread : public CUdpSendThread
{
public:
	/// constructor.
	CUdpMSendThread(int iEntityType, int iEntityId, const string& strIp, unsigned short usPort);
	/// destructor.
	virtual ~CUdpMSendThread() {}
	/**
	 * initialize thread.
	 * @return 0 succeed; otherwise fail
	 */
	virtual int Init();
protected:
	/**
	 * process message.
	 * @param[in] pMsg message to process
	 * @return 0 succeed; otherwise fail
	 */
	virtual int Process(CMsg *pMsg);
private:
	unsigned int m_nMIP; ///< target multicast IP
	unsigned short m_nPort; ///< target port
};

#endif // _UDPMSENDTHREAD_H_

