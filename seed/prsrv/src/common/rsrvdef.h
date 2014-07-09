/*
* =======================================================================
* Copyright (c) 2011-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: rsrvdef.h
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
#ifndef __RSRVDEF_H_D6E5EED5_5C54_47AC_9469_642B0DCAD6AC_
#define __RSRVDEF_H_D6E5EED5_5C54_47AC_9469_642B0DCAD6AC_

const unsigned char g_nDownPercentUnknow = -1;
const int g_nMaxFileBlockNum = 500;

const int NEW_VERSION102 = 102;

const int SEEDCMD_ISRC_ONLINE		= 101;	//之前仅转发给RSrv(现在直接在ConnectSrv处回包)
const int SEEDCMD_ISRC_ONLINE_RES	= 102;

const int SEEDCMD_REPORT_RCLIST		= 103;
const int SEEDCMD_REPORT_RCLIST_RES = 104;

const int SEEDCMD_INSERT_RC			= 105;
const int SEEDCMD_INSERT_RC_RES		= 106;

const int SEEDCMD_QUERY_PEER		= 107;	//仅转发给QSrv
const int SEEDCMD_QUERY_PEER_RES	= 108;

const int SEEDCMD_DELETE_RCLIST		= 109;
const int SEEDCMD_DELETE_RCLIST_RES = 110;	//早期失败的协议实际没有考虑到该回包（目前这条命令没有回包）

const int SEEDCMD_QUERY_PEERNUM		= 111;	//仅转发给QSrv
const int SEEDCMD_QUERY_PEERNUM_RES = 112;

const int SEEDCMD_DELETE_RCLISTNEW		= 113;	//2011-04-13：与客户端配合，修改SEEDCMD_DELETE_RCLIST不会包的遗留问题 
const int SEEDCMD_DELETE_RCLISTNEW_RES	= 114;

const int UDP_MRECV_ENTITY_TYPE = 10;				///< UDP multicast receive thread entity type
const int PING_SYN_PROCESS_ENTITY_TYPE = 11;		///< syn ping process thread entity type

const int PING_SYN_PROCESS_MSG_TYPE = 0;			///< syn ping process message type

const int MAX_RT_PING_SYN_MESSAGE_LENGTH = 1400;	///< max realtime ping syn message length

const int STATISTIC_LOG_TYPE = 60;
const int INSERTDB_COUNT = 301;
const int DELETEDB_COUNT = 302;
const int MERGETABLE_DELAY = 60;

const string RC_DB_NAME = "presource_peer_new";
const string g_strPeerRCDB = RC_DB_NAME;
const string g_strRcTableName = "t_rc_peer_list_";
const string g_strRcTableTempName = "t_rc_peer_list_temp_";
const string g_strRcOnlineTableName = "t_rc_online_peer_list_temp_";

const int EACH_SENT_TO_DB_PEER_COUNT = 1000;

#define CREATE_RESOURCE_TABLE \
	"create table if not exists %s.%s \
	( `fcid` varchar(40) collate latin1_bin NOT NULL default '', \
	  `fpeerid` varchar(50) collate latin1_bin NOT NULL default '', \
	  `fpublic_ip` int(10) unsigned NOT NULL default '0', \
	  `ffilesize` int(16) NOT NULL default '0', \
	  `fflag` tinyint(4) NOT NULL default '0', \
	  `fcapability` int(11) NOT NULL default '0', \
	  `ftime` int(11) unsigned NOT NULL default '0', \
	  `downpercent` int(3) unsigned NOT NULL default '255' \
	) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_bin;"

#define CREATE_RESOURCE_TEMP_TABLE \
	"create table if not exists %s.%s \
	( `fcid` varchar(40) collate latin1_bin NOT NULL default '', \
	  `fpeerid` varchar(50) collate latin1_bin NOT NULL default '', \
	  `fpublic_ip` int(10) unsigned NOT NULL default '0', \
	  `ffilesize` int(16) NOT NULL default '0', \
	  `fflag` tinyint(4) NOT NULL default '0', \
	  `fcapability` int(11) NOT NULL default '0', \
	  `ftime` int(11) unsigned NOT NULL default '0', \
	  `downpercent` int(3) unsigned NOT NULL default '255', \
	  PRIMARY KEY  (`fcid`,`fpeerid`), \
	  KEY `t_rc_peer_list_temp_ftime` (`ftime`) \
	) ENGINE=MEMORY DEFAULT CHARSET=latin1 COLLATE=latin1_bin;"

#define CREATE_RESOUCE_ONLINE_TEMP_TABLE \
	"create table if not exists %s.%s \
	( `fcid` varchar(40) collate latin1_bin NOT NULL default '', \
	  `fpeerid` varchar(50) collate latin1_bin NOT NULL default '', \
	  `fpublic_ip` int(10) unsigned NOT NULL default '0', \
	  `ffilesize` int(16) NOT NULL default '0', \
	  `fflag` tinyint(4) NOT NULL default '0', \
	  `fcapability` int(11) NOT NULL default '0', \
	  `ftime` int(11) unsigned NOT NULL default '0', \
	  `downpercent` int(3) unsigned NOT NULL default '255', \
	  PRIMARY KEY  (`fcid`,`fpeerid`), \
	  KEY `t_rc_online_peer_list_fcid` (`fcid`) ,\
      KEY `downpercent` (`downpercent`)\
	) ENGINE=MEMORY DEFAULT CHARSET=latin1 COLLATE=latin1_bin;"

#endif

