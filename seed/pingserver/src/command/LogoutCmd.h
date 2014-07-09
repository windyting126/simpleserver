/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: LogoutCmd.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-06
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-06			1.0			
* =======================================================================
*/
#ifndef _LOGOUTCMD_H_E081A058_7FA0_493d_A194_2CAFCDA5FF0C_
#define _LOGOUTCMD_H_E081A058_7FA0_493d_A194_2CAFCDA5FF0C_

#include "PingSrvBaseCommand.h"

struct TLogoutCmd
{
	int nInternalIp;
	string strPeerId;
	int Decode(CBufMgr &bufMgr)
	{
		if (bufMgr.GetInt(nInternalIp) != 0)
		{
			WRITE_TRACE("Ping, Decode for nInternalIp FAILED!!");
			return -1;
		}
		if (bufMgr.GetString (strPeerId) != 0)
		{
			WRITE_TRACE("Ping, Decode for peerid FAILED!!");
			return -1;
		}
		if (strPeerId.length() != PEERLENGTH)
		{
			WRITE_TRACE("Ping, peerid len is invalide, %d", strPeerId.length ());
			return -2;
		}
		return 0;
	}

};

class CLogoutCmd : public CPingSrvBaseCommand<TLogoutCmd>
{
public:
	CLogoutCmd(char *pBuf, int nLen, int nPublicIp, short nPublicPort,  int nEntityType, int nEntityId);
	~CLogoutCmd(){}
protected:
	virtual int ProcessCmd();
};

#endif



