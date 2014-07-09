/*
* =======================================================================
* Copyright (c) 2011-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: insertrccmd.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2011-03-25
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2011-03-25			1.0			
* =======================================================================
*/
#include "insertpeercmd.h"

CInsertPeerCmd::CInsertPeerCmd(string &strPeerId)
{
	m_CmdHead.m_nCmdId = MSG_LONGCONNECT_SYN_PING;
	m_CmdBody.m_strPeerId = strPeerId;
}

int CInsertPeerCmd::ProcessCmd()
{
	return 0;
}

int CInsertPeerCmdRes::ProcessCmd()
{
	return 0;
}


