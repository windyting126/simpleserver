#ifndef _MY_COMMMSG_H_
#define _MY_COMMMSG_H_

#include "commmsg.h"
#include "seedbasecommand.h"
#include <string>
using namespace std;

const int MSGTYPE_UPDATE_TO_DB = 8001;

class CCmdMsgBody : public CMsgBody
{
public:
	CCmdMsgBody(CCommand* pCmd, int nCmdId)
	{
		m_pCmd = pCmd;
		m_nCmdId = nCmdId;
	}
	~CCmdMsgBody()
	{
		if (m_pCmd)
		{
			delete m_pCmd;
			m_pCmd = NULL;
		}
	};
public:
	int Size()
	{
		return 0;
	}
	CMsgBody *Clone()
	{
		return NULL;
	}
	CCommand* GetCmd()
	{
		return m_pCmd;
	}
	int GetCmdId()
	{
		return m_nCmdId;
	}
private:
	CCommand* m_pCmd;
	int m_nCmdId;
};

#endif
