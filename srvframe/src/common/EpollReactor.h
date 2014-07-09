/**
 *    @file       EpollReactor.h
 *
 *    @brief      old version of long connection system
 *    @details    optimize the frame of code completely
 *
 *    @version    1.0
 *    @date       03/19/2013 07:35:08 PM
 *
 *    @author     wangtiefeng (), wangtf418@163.com
 */

#ifndef _EPOLLREACTOR_H_H_
#define _EPOLLREACTOR_H_H_

#include <string>
#include <list>
#include <set>
#include <sys/epoll.h>

#include "framecommon/framecommon.h"
#include "commmsg.h"
using namespace MYFRAMECOMMON;
using namespace std;

/// contain member: m_nServerType and m_nServerIndex
struct TConDstSrvInfo;
/// contain the details of a long connection, e.g., m_nState,m_sRemoteIp and m_usRemotePort
class TConnectItem;
/// message used in Epoll Reactor thread
class CEpollMsg;

/**
 * dispatching class of old long connection system
 * @details this class used the singleton pattern
 */
class CEpollReactor
{
public:
	/// get instance of this class
	static CEpollReactor * GetInstance();
private:
	/// constructor
	CEpollReactor();
	/// destructor
	virtual ~CEpollReactor();
	/// handler of the instance
	static CEpollReactor *m_hInstance;
	/// mutex lock for m_hInstance, e.g.,creating,destroying
	static CThreadMutex m_lockInstance;

public:
	/**
	 * start epoll reactor thread
	 * @return 0 fail, otherwise succeed
	 */
	int StartService();
	/**
	 * stop epoll reactor thread
	 */
	void StopService();

public:
	/**
	* Old Interface
	* @return ERROR_SUCCESS or ERROR_OUTOFMEMORY.
	*/
	int AsyncConnect(const char *pcszRemoteIp, unsigned short usRemotePort,
					int iEntityType, int iEntityId,
					unsigned long long ullExpireTime,
					unsigned char ucMachineType, unsigned short usMachineIndex);
public:
	/**
	 * interface for create a new connection (alse, called when connect is closed by any exception)
	 * @nConnectTimeout, reconnect interval when connection is timeout
	 * @nEntityType, entity type of long connection thread
	 * @nEntityId, entity id of long connection thread
	 */
	int Connect(unsigned char nMachineType, unsigned short usMachineIndex, const string &strRemoteIp, unsigned short nRemotePort, unsigned long long nConnectTimeout, int nEntityType, int nEntityId);
	/**
	 * interface for recreate a connection
	 * @nConnectTimeout, reconnect interval when connection is timeout
	 */
	int ReConnect(unsigned char nMachineType, unsigned short nMachineIndex, const string &strRemoteIp, unsigned short nRemotePort, unsigned long long nConnectTimeout);
	/**
	 * interface for recreate a connection
	 * @nConnectTimeout, reconnect interval when connection is timeout
	 */
	int LostConnect(unsigned char nMachineType, unsigned short nMachineIndex);
	/**
	 * interface for recreate a connection
	 * @nConnectTimeout, reconnect interval when connection is timeout
	 */
	int DisConnect(unsigned char nMachineType, unsigned short nMachineIndex);
		
private:
	/**
	 * thread function of Epoll Reactor
	 */
	static void * WorkThreadFunction(void *pArg);
private:
	/**
	 * work function in thread function WorkThreadFunction 
	 */
	void WorkProcess();
	/**
	 * message process function
	 * @pMsg, control message
	 */
	int Process(CEpollMsg *pMsg);
	/**
	 * dispatch of all long connection, e.g., check timeout of connection, connect server
	 */
	int Dispatch();

private:
	/**
	 * process message of connect
	 */
	int ProcessConnect(CEpollMsg *pMsg);
	/**
	 * process message of reconnect
	 */
	int ProcessReConnect(CEpollMsg *pMsg);
	/**
	 * process message of lost connection
	 */
	int ProcessDisConnect(CEpollMsg *pMsg);
	/**
	 * check socket timeout when connecting
	 */
	int ProcessLostConnect(CEpollMsg *pMsg);
	/**
	 * process message of disconnect
	 */

	int CheckTimeout();
	/**
	 * start connect when connection is not established
	 */
	int DispatchConnect();
	/**
	 * wait for epoll events and get connect result
	 */
	int EpollWait();
	/**
	 * send ping message to every established connection
	 */
	int SendPing();

private:
	/**
	 * send connect message to long connection thread
	 */
	int SendConnectMsg(int iSocketId,
			const char *pcszRemoteIp, unsigned short usRemotePort,
			unsigned char ucMachineType, unsigned short usMachineIndex,
			int iDestEntityType, int iDestEntityId);
	/**
	 * send destroy connect message to long connection thread
	 */
	int SendDestoryConnectMsg(unsigned char ucMachineType, unsigned short usMachineIndex,
			int iDestEntityType, int iDestEntityId);
	/**
	 * send ping message to certain established connection
	 * @nMachineType, Machine Type of long connection
	 * @nMachineIndex, Machine Index of long connection
	 */
	void SendPing(int nMachineType, int nMachineIndex);

private:
	/// message queue for Epoll Reactor thread
	CAsyncQueue<CEpollMsg *> m_msgQueue;
	/// infomation for all long connection
	map<TConDstSrvInfo, TConnectItem> m_mapConnect;
	/// infomation for destroy connection
	map<TConDstSrvInfo, TConnectItem> m_mapDisConnect;

private:
	int m_iEpFd;
	bool m_bIsStop;
	pthread_t m_iThreadId;
	bool m_bThreadOk;

	int m_nPingInterval;
	set<TConDstSrvInfo> m_setConDstSrvInfo;
};

//struct define////////////////////////////////////////////////////////////
struct TConDstSrvInfo
{
	unsigned char m_nServerType;
	unsigned short m_nServerIndex;
	//function
	TConDstSrvInfo() : m_nServerType(0), m_nServerIndex(0)
	{

	}
	TConDstSrvInfo(unsigned char nServerType, unsigned short nServerIndex)
	{
		m_nServerType = nServerType;
		m_nServerIndex = nServerIndex;
	}
	unsigned int GetWeight() const
	{
		unsigned int nWeight = ((m_nServerType << 16) & 0xffff0000) + m_nServerIndex;
		return nWeight;
	}
	bool operator<(const TConDstSrvInfo& ConDstSrvInfo) const
	{
		return (GetWeight() < ConDstSrvInfo.GetWeight());
	}
	bool operator==(const TConDstSrvInfo& ConDstSrvInfo) const
	{
		return ((m_nServerType == ConDstSrvInfo.m_nServerType) && (m_nServerIndex == ConDstSrvInfo.m_nServerIndex));
	}
};

struct TConnectItem
{
public:
	enum EItemState
	{
		EIS_INIT = 0,
		EIS_ESTABLISH = 1
	};
public:
	TConnectItem(unsigned char ucMachineType,
			unsigned short usMachineIndex,
			const string &strRemoteIp,
			unsigned short usRemotePort,
			ULONG64 ullExpireTime,
			int iDestEntityType,
			int iDestEntityId)
	{
		m_ucMachineType = ucMachineType;
		m_usMachineIndex = usMachineIndex;
		m_sRemoteIp = strRemoteIp;
		m_usRemotePort = usRemotePort;
		m_ullExpireTime = ullExpireTime;

		m_iDestEntityType = iDestEntityType;
		m_iDestEntityId = iDestEntityId;
		m_nState = EIS_INIT;
		m_nTryConnTime = GetTime(0);
		m_nSocketId = -1;
	}

	unsigned char m_ucMachineType;
	unsigned short m_usMachineIndex;
	string m_sRemoteIp;
	unsigned short m_usRemotePort;
	ULONG64 m_ullExpireTime;

	int m_iDestEntityType;
	int m_iDestEntityId;
	int m_nState;
	unsigned long long m_nTryConnTime;
	int m_nSocketId;
};


//msg definine/////////////////////////////////
struct TEpollMsgBody
{
	TEpollMsgBody()
	{

	}
	virtual ~TEpollMsgBody()
	{

	}
};

class CEpollMsg
{
public:
	CEpollMsg()
	{
		m_nMsgType = -1;
		m_pMsgBody = NULL;
	}
	CEpollMsg(int nMsgType, TEpollMsgBody *pMsgBody)
	{
		m_nMsgType = nMsgType;
		m_pMsgBody = pMsgBody;
	}
	~CEpollMsg()
	{
		if (m_pMsgBody)
		{
			delete m_pMsgBody;
		}
	}
public:
	int GetMsgType() {return m_nMsgType;}
	TEpollMsgBody * GetMsgBody() {return m_pMsgBody;}
	void DetachMsgBody() {m_pMsgBody = NULL;}
private:
	int m_nMsgType;
	TEpollMsgBody *m_pMsgBody;
};

struct TEpollConnectMsgBody : public TEpollMsgBody
{
	unsigned char m_nMachineType;
	unsigned short m_nMachineIndex;
	int m_nSrcEntityType;
	int m_nSrcEntityId;
	string m_strSrvIp;
	unsigned short m_nSrvPort;
	unsigned long long m_nConnectTimeout;
};

struct TEpollReConnectMsgBody : public TEpollMsgBody
{
	unsigned char m_nMachineType;
	unsigned short m_nMachineIndex;
	string m_strSrvIp;
	unsigned short m_nSrvPort;
	unsigned long long m_nConnectTimeout;
};

struct TEpollDisConnectMsgBody : public TEpollMsgBody
{
	unsigned char m_nMachineType;
	unsigned short m_nMachineIndex;
};

struct TEpollLostConnectMsgBody : public TEpollMsgBody
{
	unsigned char m_nMachineType;
	unsigned short m_nMachineIndex;
};

#endif //_EPOLLREACTOR_H_H_




