#ifndef __INSERT_RC_TOOLS__
#define __INSERT_RC_TOOLS__

#include "myserver.h"
#include "insertrctoolsconf.h"
#include "insertrctoolsdef.h"

class CInsertTools
	: public CMyserver
{
public:
	CInsertTools();
	~CInsertTools();
	int InitServer();
	int StartServer();
	CConf& GetConf()
	{
		return m_conf;
	}

	static int InitLogSystem();

private:
	int SendConnectMsg(const char* pcszRemoteIp, unsigned short usRemotePort);
	void TransMsg(char* pBuf, int nLen, int nMachineIndex, int nType, int nMsgId);
	int LoadPeers();
	int LoadRc();
private:
	CConf m_conf;
	CThreadGroup* m_pTimerThreadGrp;
	CThreadGroup* m_pConnectThreadGrp;
};


#endif 

