/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: LongConnectTask.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-04
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-04			1.0			
* =======================================================================
*/
#ifndef _LONGCONNECTTASK_H_5949798D_7C87_4ff0_8388_ED944AC771B3_
#define _LONGCONNECTTASK_H_5949798D_7C87_4ff0_8388_ED944AC771B3_

#include "framecommon/framecommon.h"
#include "CommonTask.h"

using namespace MYFRAMECOMMON;

class CLongConnectTask : public CCommonTask
{
public:
	CLongConnectTask(CTcpSocket *pSocket, CNetProcessThread *pWorkThread, 
		unsigned char ucMachineType, unsigned short usMachineIndex, bool bActive, int iListType)
		: CCommonTask(pSocket, pWorkThread, ucMachineType, usMachineIndex, bActive, iListType)
	{
	}
	~CLongConnectTask()
	{
	}
	int GetSendListLen()
	{
		return m_sendList.GetNum();
	}
protected:
	virtual int ProcessPacket(PSERVERCOMMANDPACKET pServerCommandPacket);
private:
	int GetCurCmdId(char *pServerCommandPacket);
private:
	virtual int SendProcess();
};


#endif



