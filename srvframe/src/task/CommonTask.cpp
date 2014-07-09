/****************************************************************************
**
**  Copyright  (c)  2008-2009,  Baofeng, Inc.  All Rights Reserved.
**
**  Subsystem:    ConnectServer
**  File:         	     CommonTask.cpp
**  Created By:   fwx
**  Created On:   2009/01/08
**
**  Purpose:
**     CommonTask
**
**  History:
**  Programmer      Date         Version        Number      Description
**  --------------- --------   ---------- ------------------------------
**    fwx       2009/01/08         1.0            01          creation
****************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include "CommonTask.h"
#include "EpollReactor.h"
#define LONGCONNECT_RECVSNED_NUM 20
//static member==============
TTaskConnectAttrib CCommonTask::m_TaskConnectAttrib[256][256];
CThreadRwLock CCommonTask::m_lockTaskConnectAttrib;

int CCommonTask::CheckConnectState( unsigned char ucMachineType , unsigned short usMachineIndex )
{
	if (usMachineIndex >= 256)
	{
		return CONNECT_ATTRIBUTE_CLOSE;
	}
	int nConnectStat = 0;
	m_lockTaskConnectAttrib.GetReadLock();
	nConnectStat = m_TaskConnectAttrib[ucMachineType][usMachineIndex].m_nConnectState;
	m_lockTaskConnectAttrib.ReleaseLock();
	return nConnectStat;
}

int CCommonTask::GetConnectTaskId(unsigned char ucMachineType , unsigned short usMachineIndex, ULONG64 &nTaskId)
{
	if (usMachineIndex >= 256)
	{
		return -1;
	}
	m_lockTaskConnectAttrib.GetReadLock();
	if (m_TaskConnectAttrib[ucMachineType][usMachineIndex].m_nConnectState == CONNECT_ATTRIBUTE_CLOSE)
	{
		m_lockTaskConnectAttrib.ReleaseLock();
		return -1;
	}
	nTaskId = m_TaskConnectAttrib[ucMachineType][usMachineIndex].m_nTaskId;
	m_lockTaskConnectAttrib.ReleaseLock();
	return 0;
}

//member==============================
void CCommonTask::AddTaskConnectAttrib( unsigned char ucMachineType , unsigned short usMachineIndex , int iConnectState)
{
	if (usMachineIndex >= 256 )
	{
		WriteRunInfo::WriteError("usMachineIndex=%u too large", usMachineIndex);
		return;
	}

	m_lockTaskConnectAttrib.GetWriteLock();
	m_TaskConnectAttrib[ucMachineType][usMachineIndex].m_nConnectState = iConnectState;
	m_TaskConnectAttrib[ucMachineType][usMachineIndex].m_nTaskId = GetTaskId();
	m_lockTaskConnectAttrib.ReleaseLock();
}

CCommonTask::CCommonTask( CTcpSocket *pSocket, CNetProcessThread *pWorkThread, unsigned char ucMachineType, unsigned short usMachineIndex, bool bActive, int iListType )
	: CRecvAndSendTask( pSocket, pWorkThread, iListType )
	, m_uReceivePos( 0 )	
	, m_uReceiveSize( 0 )
	, m_ucDestMachineType( ucMachineType )
	, m_usDestMachineIndex( usMachineIndex )
	, m_bActive( bActive )
	, m_state( ReadPacketHead )
{
	ResetRecvBuffer( 0, sizeof( SERVERCOMMANDPACKET ) );
	AddTaskConnectAttrib( ucMachineType , usMachineIndex , CONNECT_ATTRIBUTE_ESTABLISH );
	m_nLastRecvPinTime = (long long)CMyserver::GetInstance()->GetTimeVal().tv_sec;
	m_nTaskTimeOut = CMyserver::GetInstance()->GetBaseConf().GetConfigItem().nLongConnectTimeout;  //汪铁丰：2010-12-06 增加了定时ping逻辑，修正无法判断连接断开的bug
	if (m_pTcpSocket != NULL)
	{
		int nSocket = m_pTcpSocket->GetSocket();
		int nSockBufLen = 8 * 1024 * 1024;//set socket buflen 8MB
		setsockopt(nSocket, SOL_SOCKET, SO_SNDBUF, &nSockBufLen, sizeof(int));
		setsockopt(nSocket, SOL_SOCKET, SO_RCVBUF, &nSockBufLen, sizeof(int));
	}
}

CCommonTask::~CCommonTask( void )
{
	AddTaskConnectAttrib( m_ucDestMachineType , m_usDestMachineIndex , CONNECT_ATTRIBUTE_CLOSE );
	if ( m_bActive )
	{
		CEpollReactor::GetInstance()->LostConnect(m_ucDestMachineType, m_usDestMachineIndex);
	}
}

int CCommonTask::Init()
{
	int ikeepAlive = 1; // 开启keepalive属性
	int ikeepIdle = 60; // 如该连接在60秒内没有任何数据往来,则进行探测 
	int ikeepInterval = 5; // 探测时发包的时间间隔为5 秒
	int ikeepCount = 3; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.

	setsockopt( GetSocket(), SOL_SOCKET, SO_KEEPALIVE, ( void * )&ikeepAlive, sizeof( ikeepAlive ) );
	setsockopt( GetSocket(), SOL_TCP, TCP_KEEPIDLE, ( void* )&ikeepIdle, sizeof( ikeepIdle ) );
	setsockopt( GetSocket(), SOL_TCP, TCP_KEEPINTVL, ( void * )&ikeepInterval, sizeof( ikeepInterval ) );
	setsockopt( GetSocket(), SOL_TCP, TCP_KEEPCNT, ( void * )&ikeepCount, sizeof( ikeepCount ) );
	
	AddToEpoll();
	return 0;
}

int CCommonTask::DestroySelf( void )
{
	delete this;
	return 0;
}

int CCommonTask::CheckTimeOut()
{
	if (m_nTaskTimeOut == -1)
	{
		return 0;
	}

	int ret = 0;
	if ((CMyserver::GetInstance()->GetTimeVal().tv_sec - m_nLastRecvPinTime) > m_nTaskTimeOut)
	{
		DestroySelf();
		ret = 1;
	}
	return ret;
}

int CCommonTask::RecvProcess( void )
{
	unsigned int uMaxReceive;
	int iReced;

    int ii=0;
    while (ii < LONGCONNECT_RECVSNED_NUM)
    {
    	uMaxReceive = m_uReceiveSize - m_uReceivePos;
    	iReced = recv( GetSocket(), &m_vReceiveBuffer[0] + m_uReceivePos,  uMaxReceive, MSG_DONTWAIT );
    	if ( iReced == 0 )
    	{
    	    WriteRunInfo::WriteLog("client close the socket, %d", GetSocket());
    		return -1;
    	}
    	
    	if ( iReced < 0 )
    	{
    		if ( errno != EINTR && errno != EAGAIN )
    		{
    			return -1;
    		}
    		return 0;
    	}
    	
    	m_uReceivePos += iReced;
    	
    	switch( m_state ) 
    	{
    	case ReadPacketHead:
    		{
    			if ( !PacketFinished() ) break;
    			PSERVERCOMMANDPACKET pServerCommandPacket = ( PSERVERCOMMANDPACKET )&m_vReceiveBuffer[0];
    			int iNeedRecvLen = ( int )ntohl( pServerCommandPacket->iMsgLen );
    			if ( iNeedRecvLen > ( 400 * 1024 * 1024 ) || iNeedRecvLen < 0 )
    			{
    				return -1;
    			}
				if (iNeedRecvLen == 0)
				{
					m_nLastRecvPinTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
					//printf("Update tasktime recv %lld(threadid=%d:%d,taskid=%llu)\n", m_nLastRecvPinTime, m_pThread->GetEntityType(),m_pThread->GetEntityId() ,m_lTaskId);
					ResetRecvBuffer( 0, sizeof( SERVERCOMMANDPACKET ) );
					
					SERVERINNERCOMMANDPACKET srvCommHeader;
					memset(&srvCommHeader, 0, sizeof(SERVERINNERCOMMANDPACKET));
					memcpy(&(srvCommHeader.serverCommandPacket), pServerCommandPacket, sizeof(SERVERCOMMANDPACKET));
					if (srvCommHeader.serverCommandPacket.usMsgId == (unsigned short)(-1))
					{
						srvCommHeader.serverCommandPacket.usMsgId = (unsigned short)htons (-2);	//修改ping回报消息id，避免ping包造成死循环
						PutMsgToSendList((char*)&srvCommHeader, (int)sizeof(srvCommHeader));	//长连接ping包回报 add by wangtiefeng
					}				
				}
				else
				{
					m_state = ReadPacket;
					ResetRecvBuffer( sizeof( SERVERCOMMANDPACKET ), iNeedRecvLen + sizeof( SERVERCOMMANDPACKET ) );
				}    			
    		}
    		break;
    	case ReadPacket:
    		{
    			if ( !PacketFinished() ) break;
    			ParseOnePacket();
				m_nLastRecvPinTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;

    			m_state = ReadPacketHead;
    			ResetRecvBuffer( 0, sizeof( SERVERCOMMANDPACKET ) );
    		}
    		break;
    	}
        ii++;
    }
	return 0;
}

int CCommonTask::SendProcess( void )
{
	int retValue = 0;
    int ii=0;
    while (ii < LONGCONNECT_RECVSNED_NUM)
    {
    	if (m_pCurSendMsg != NULL)
    	{
    	        retValue = SendData();
    	}
    	else
    	{
    	        m_pCurSendMsg = GetMsgFromSendList();
    	        if (m_pCurSendMsg != NULL)
    	        {
    	                retValue = SendData();                        
    	        }                        
    	        else
    	        {
    	        	int tmpEvent = MYPOLLERR|MYPOLLHUP|MYPOLLIN;
    		        CNetProcessThread *pNetThread = (CNetProcessThread*)m_pThread;
    		        CEpoll *pEoll = pNetThread->GetEpoll();
    		        pEoll->ModifyFromEpoll(this, tmpEvent);
    	            retValue = 1;
					break;
    	        }
    	}
        if (retValue == 2 || retValue == -1)
             break;		
        ii++;
    }

	return retValue;
}

int CCommonTask::PutMsgToSendList( const char *pBuf, int iLen )
{
	if ( !pBuf )
	{
		return -1;
	}
	if ( (int)sizeof(SERVERINNERCOMMANDPACKET) > iLen )
	{
		return -1;
	}
	
	PSERVERINNERCOMMANDPACKET pServerInnerCommandPacket = ( PSERVERINNERCOMMANDPACKET )pBuf;
	if ( pServerInnerCommandPacket->ucDestMachineType != m_ucDestMachineType ||
	      pServerInnerCommandPacket->usDestMachineIndex != m_usDestMachineIndex )
	{
		return -1;
	}

	WRITE_TRACE( "PutMsgToSendList %d %d", m_ucDestMachineType, m_usDestMachineIndex );
	int n = sizeof( pServerInnerCommandPacket->ucDestMachineType ) + sizeof( pServerInnerCommandPacket->usDestMachineIndex );
	return SendToList( ( ( char *)pServerInnerCommandPacket ) + n, iLen - n );
}

int CCommonTask::ProcessPacket( PSERVERCOMMANDPACKET pServerCommandPacket )
{
	if ( !pServerCommandPacket )
	{
		return -1;
	}
	
	TMsg tMsg;
	tMsg.destEntityType = RECVANDSENDMSGENTITY;
	tMsg.destEntityId = ( int )ntohl( pServerCommandPacket->iDestProcessId );
	tMsg.srcEntityId = 0;
	tMsg.srcEntityType= LONGCONNECTEDENTITY;
	tMsg.srcTaskId = GetTaskId();
	tMsg.taskId = ( unsigned long long )ntohll( pServerCommandPacket->ullDestTaskId );
	tMsg.msgType = CLIENTMSGTYPE;
	
	int iLength = ( int )ntohl( pServerCommandPacket->iMsgLen);
	char *pTmpBuf = new char[iLength];
	memcpy( pTmpBuf , pServerCommandPacket->serverCommandPacketContent , iLength );

	CMsgBody * pMsgBody = new CClientMsgBody( pTmpBuf , iLength );
	
	CMsg *pMsg = new CMsg( tMsg, pMsgBody );
	
	CMyserver::GetInstance()->PutMsgToThread( pMsg );

	return 0;
}

void CCommonTask::ResetRecvBuffer( unsigned int uReceivePos, unsigned int uReceiveSize )
{
	m_uReceivePos = uReceivePos;
	m_uReceiveSize = uReceiveSize;
	if ( ( unsigned int )m_vReceiveBuffer.size() < m_uReceiveSize )
	{
		m_vReceiveBuffer.resize( m_uReceiveSize );
	}
}

int CCommonTask::ParseOnePacket( void )
{
	PSERVERCOMMANDPACKET pServerCommandPacket = ( PSERVERCOMMANDPACKET )&m_vReceiveBuffer[0];
	if ( !pServerCommandPacket )
	{
		return -1;
	}
	return ProcessPacket( pServerCommandPacket );
}

int CCommonTask::AddToEpoll( void )
{          
        int tmpEvent = MYPOLLERR | MYPOLLHUP | MYPOLLIN;                         
       if (m_sendList.GetNum() != 0 || m_pCurSendMsg != NULL)
        {
                tmpEvent = tmpEvent|MYPOLLOUT;
        }
       
        CNetProcessThread *pNetThread = (CNetProcessThread*)m_pThread;
        CEpoll *pEoll = pNetThread->GetEpoll();
        pEoll->AddToEpoll(this, tmpEvent);
        return 0;
}

int CCommonTask::DelFromEpoll( void )
{
        CNetProcessThread *pNetThread = (CNetProcessThread*)m_pThread;
        CEpoll *pEoll = pNetThread->GetEpoll();
        pEoll->DelFromEpoll( this );
        return 0;
}

int CCommonTask::SendToList(const char *pBuf, int nLen)
{        
        if (pBuf == NULL || nLen == 0)
        {       
                return -1;
        }               
        char *pTmpBuf = new char[nLen];
        memcpy(pTmpBuf, pBuf, nLen);
        CSendMsg *pSendMsg = new CSendMsg(nLen, pTmpBuf);
        
        m_sendList.push_back(pSendMsg);
        if (m_sendList.GetNum() == 1) //只有刚刚变化时才修改状态
        {
            int tmpEvent = MYPOLLERR|MYPOLLHUP|MYPOLLIN | MYPOLLOUT;
            CNetProcessThread *pNetThread = (CNetProcessThread*)m_pThread;
            CEpoll *pEoll = pNetThread->GetEpoll();
            pEoll->ModifyFromEpoll(this, tmpEvent);
        }
        return 0;
}

