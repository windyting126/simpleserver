/**
 * @file udpmrecvthread.h
 * @author yanshi <yanshi@baofeng.com>
 * @date 2013-03-08
 */

#ifndef _UDPMRECVTHREAD_H_
#define _UDPMRECVTHREAD_H_

#include "udpcommonrecvthread.h"

/// UDP multicast receive thread.
class CUdpMRecvThread : public CUdpCommonRecvThread
{
public:
	/**
	 * constructor.
	 * @param[in] iEntityType thread entity type
	 * @param[in] iEntityId thread entity id
	 * @param[in] strIp bind IP
	 * @param[in] usPort bind port
	 */
	CUdpMRecvThread(int iEntityType, int iEntityId, string& strIp, unsigned short usPort);
	/// destructor.
	virtual ~CUdpMRecvThread() {}
	/**
	 * initialize thread.
	 * @return 0 succeed; otherwise fail
	 */
	virtual int Init();
	/**
	 * run thread.
	 * @return 0 succeed; otherwise fail
	 */
	virtual int Run();
};

#endif // _UDPMRECVTHREAD_H_

