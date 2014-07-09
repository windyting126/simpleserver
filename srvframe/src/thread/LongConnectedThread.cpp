/****************************************************************************
**
**  Copyright  (c)  2008-2009,  Baofeng, Inc.  All Rights Reserved.
**
**  Subsystem:    ConnectServer
**  File:         	     LongConnectedThread.cpp
**  Created By:   fwx
**  Created On:   2009/01/08
**
**  Purpose:
**     ForwardTaskThread
**
**  History:
**  Programmer      Date         Version        Number      Description
**  --------------- --------   ---------- ------------------------------
**    fwx       2009/01/08         1.0            01          creation
****************************************************************************/
#include <string>

#include "commmsg.h"
#include "LongConnectedThread.h"
#include "CommonTask.h"


CLongConnectedThread::CLongConnectedThread( int iEntityType, int iEntityId, int iListType/* = 0*/ )
	: CNetProcessThread(iEntityType, iEntityId)
	, m_pEpollReactor( NULL )
	, m_iListType( iListType )
{

}
        
CLongConnectedThread::~CLongConnectedThread( void )
{
	Stop();
}

int CLongConnectedThread::Init( void )
{
	m_pEpollReactor = CEpollReactor::GetInstance();
	if ( !m_pEpollReactor )
	{
		return -1;
	}
	return CNetProcessThread::Init();
}

int  CLongConnectedThread::Process(CMsg *pMsg)
{
        TMsg *pTMsg = pMsg->GetTMsg();
	if ( !pTMsg )
	{
		return 0;
	}
        WRITE_TRACE("The LongConnected thread recv a  msg , msgtype %d...", pTMsg->msgType);        
	if ( pTMsg->msgType == CLIENTMSGTYPE)
	{
		CClientMsgBody *pBody = ( CClientMsgBody * )pMsg->GetMsgBody();
		if ( !pBody )
		{
			return 0;
		}

		if ( m_iListType )
		{
			std::list<CSocketTask*>::iterator it = m_pSocketList.begin();
			while( it != m_pSocketList.end() )
			{
				CSocketTask * pSocketTask = ( *it );
				if ( pSocketTask )
				{
					pSocketTask->PutMsgToSendList( pBody->GetBuffer(), pBody->GetLen() );
				}
				it ++;
			}
		}
		else
		{
			map<ULONG64, CSocketTask*>::iterator it = m_pMapSocketList.begin();
			while( it != m_pMapSocketList.end() )
			{
				CSocketTask * pSocketTask = ( *it ).second;
				if ( pSocketTask )
				{
					pSocketTask->PutMsgToSendList( pBody->GetBuffer(), pBody->GetLen() );
				}
				it ++;
			}
		}
	}
	else if ( pTMsg->srcEntityType == EPOLLREACTORENTITY )
	{
		if (pTMsg->msgType == CONNECTMSGTYPE)
		{
			CConnectMsgBody *pBody = (CConnectMsgBody*)pMsg->GetMsgBody();
			if ( !pBody )
			{
				return 0;
			}

			std::string tmpStr = pBody->GetRemoteIp();
			CTcpSocket *pSocket = new CTcpSocket( tmpStr, pBody->GetRemotePort(), pBody->GetSocket() );
			CSocketTask *tmpSocketTask = NULL;
			CCommonTaskArg commonTaskArg( pBody->GetDestMachineType(), pBody->GetDestMachineIndex(), true, m_iListType );
			tmpSocketTask = CMyserver::GetInstance()->GetClassFactory()->GenSocketTask(this, pSocket, COMMONTASK, &commonTaskArg );
			tmpSocketTask->Init();
			WriteRunInfo::WriteLog( "connect to %d %d %d succ ", ( int )pBody->GetDestMachineType(), ( int )pBody->GetDestMachineIndex(), pBody->GetSocket() );
		}
		else
		{
			WriteRunInfo::WriteLog("Send&recv  thread recv invalid msgtype %d", pTMsg->msgType);
		}
	}
	else if ( pTMsg->srcEntityType== MAINENTITY )
	{
		if (pTMsg->msgType == TOCONNECTMSGTYPE)
		{
			CToConnectMsgBody *pBody = (CToConnectMsgBody*)pMsg->GetMsgBody();
			if ( !pBody )
			{
				return 0;
			}
			if ( m_pEpollReactor )
			{
				m_pEpollReactor->Connect(pBody->GetDestMachineType(), pBody->GetDestMachineIndex(), pBody->GetRemoteIp(), pBody->GetRemotePort(), 3, m_nEntityType, m_nEntityId);
			}
		}
		else
		{
			WriteRunInfo::WriteLog("Send msg thread recv invalid msgtype %d", pTMsg->msgType);
		}
	}
	else if (pTMsg->msgType == DISCONNECTMSGTYP)
	{
		CConnectMsgBody *pBody = ( CConnectMsgBody* )pMsg->GetMsgBody();
		if ( !pBody )
		{
			return 0;
		}
		ULONG64 nTaskId = 0;
		if (CCommonTask::GetConnectTaskId(pBody->GetDestMachineType(), pBody->GetDestMachineIndex(), nTaskId) == 0)
		{
			if ( m_iListType )
			{
				std::list<CSocketTask*>::iterator it = m_pSocketList.begin();
				while( it != m_pSocketList.end() )
				{
					CSocketTask * pSocketTask = ( *it );
					if ( pSocketTask && pSocketTask->GetTaskId() == nTaskId)
					{
						delete pSocketTask;
						WriteRunInfo::WriteLog("LongConnectedThread : Destory task[%d,%d]", pBody->GetDestMachineType(), pBody->GetDestMachineIndex());
					}
					it ++;
				}
			}
			else
			{
				map<ULONG64, CSocketTask*>::iterator it = m_pMapSocketList.find(nTaskId);
				if( it != m_pMapSocketList.end() )
				{
					CSocketTask * pSocketTask = ( *it ).second;
					if ( pSocketTask )
					{
						delete pSocketTask;
						WriteRunInfo::WriteLog("LongConnectedThread : Destory task[%d,%d]", pBody->GetDestMachineType(), pBody->GetDestMachineIndex());
					}
				}
			}
		}
	}
	else
	{
		CNetProcessThread::Process( pMsg );
	}
	return 0;
}

