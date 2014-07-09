/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingServerDef.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-03
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-03			1.0			
* =======================================================================
*/
#ifndef _PINGSERVERDEF_H_31092C46_9FE9_49e1_BC3B_5D5B7C22ADB1_
#define _PINGSERVERDEF_H_31092C46_9FE9_49e1_BC3B_5D5B7C22ADB1_

const int PINGSERVER_VERSION = 59;

const int PINGSERVER_CMD_PING = 22;
const int PINGSERVER_CMD_LOGOUT = 23;

const int PINGSERVER_MSG_HEAD_LEN = sizeof(int) + sizeof(char);
const int PINGSERVER_CMDINFO_OFF = sizeof(int);

const int PINGONTIMESYN_ENTITY_TYPE = 10;
const int CHECKTIMEOUT_ENTITY_TYPE = 11;
const int UDP_MSEND_ENTITY_TYPE = 12;///< UDP multicast send thread entity type
const int CONFIG_UPDATE_ENTITY_TYPE = 13;///< pmgrsrv DB configure update entity type

/*log type*/
const int LOGTYPE_STATUS = 10;


enum EStatType
{
	MINCMDINDEX = 0,
	PINGCMD = 0,
	LOGOUTCMD = 1,
	INVALIDCMD = 2,
	PEERTIMEOUT = 3,
	SYNPINGCMD = 4,
	SYNLOGOUTCMD = 5,
	LC_RECONNECT = 6,
	MAXCMDINDEX
};
	
#endif


