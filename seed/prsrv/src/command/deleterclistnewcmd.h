/*
* =======================================================================
* Copyright (c) 2011-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: deleterclistnewcmd.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2011-04-13
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2011-04-13			1.0			
* =======================================================================
*/
#ifndef __DELETERCLISTNEWCMD_H_CFC32EE3_343C_43B1_8FBB_A3601312555F_
#define __DELETERCLISTNEWCMD_H_CFC32EE3_343C_43B1_8FBB_A3601312555F_
#include "seedbasecommand.h"
#include "deleterclistcmd.h"

class CDeleteRcListNewCmd : public CSeedBaseCmd<TDeleteRcListCmd, TDeleteRcListCmdRes>
{
public:
	CDeleteRcListNewCmd(char* pBuf, int nLen) : CSeedBaseCmd<TDeleteRcListCmd, TDeleteRcListCmdRes>(pBuf, nLen)
	{
		m_outHead.m_nCmd = SEEDCMD_DELETE_RCLISTNEW_RES;
		m_nIfRes = 0;
	}
	~CDeleteRcListNewCmd(){}
protected:
	int ProcessCmd();
};


#endif

