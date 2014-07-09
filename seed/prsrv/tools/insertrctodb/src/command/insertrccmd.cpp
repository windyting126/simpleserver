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
#include "insertrccmd.h"

CInsertRcCmd::CInsertRcCmd(string strGcid, string strPeerId, long long llFileSize, int nCapability, int nDownLoadPercent)
{
	m_CmdHead.m_nCmdId = SEEDCMD_INSERT_RC;
	m_CmdHead.m_nProtocolVer = NEW_VERSION102;
	m_CmdBody.m_strGcid = strGcid;
	m_CmdBody.m_strPeerId = strPeerId;
	m_CmdBody.m_llFileSize = llFileSize;
	m_CmdBody.m_nCapability = nCapability;
	m_CmdBody.m_nDownloadScale = nDownLoadPercent;
}

int CInsertRcCmd::ProcessCmd()
{
	return 0;
}

int CDeleteCmdRes::ProcessCmd()
{
	return 0;
}


