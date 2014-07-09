/*
 * FileName:       
 * Author:         wangqifeng  Version: 1.0  Date: 2009-1-4
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version >   <desc>
 */
#ifndef __MY_TESTSERVER_H
#define __MY_TESTSERVER_H

#include "myserver.h"
#include "threadgroup.h"
#include "pingpeerhashtable.h"
#include "myconfig.h"
#include "maintainudpthread.h"
#include "onlinedbupdatethread.h"

class CPRServer:public CMyserver
{
public:
    CPRServer()
	{
		m_pPingPeerHashTable = NULL;
	}
    ~CPRServer()
	{
		if (m_pPingPeerHashTable)
		{
			//delete m_pPingPeerHashTable;
			//m_pPingPeerHashTable = NULL;
			m_pPingPeerHashTable->DestroyHashTable();
		}
	}
	
	static void InitLogSystem();
	
    virtual int InitServer();
    virtual int StartServer();

	CMyConfig &GetMyConf();
	
private:
	int StartThreads();

	CThreadGroup *m_pListenThreadGrp;
	CThreadGroup *m_pRecvAndSendThreadGrp;
	CThreadGroup *m_pMaintainThreadGrp;
	CThreadGroup *m_pTimerThreadGrp;
	CThreadGroup *m_pUdpMRecvThreadGrp;
	CThreadGroup *m_pPingSynProcessThreadGrp;

	CPingPeerHashTable *m_pPingPeerHashTable;
	CMaintainUdpThread *m_pMaintainUpdThread;
	COnlineDBUpdateThread *m_pOnlineDBUpdateThread;
	CMyConfig m_MyConfig;
};
#endif

