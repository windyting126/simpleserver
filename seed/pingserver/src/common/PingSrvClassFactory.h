/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingSrvClassFactory.h
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
#ifndef _PINGSRVCLASSFACTORY_H_9A543A9D_3240_4cc0_A14B_E506886F36CF_
#define _PINGSRVCLASSFACTORY_H_9A543A9D_3240_4cc0_A14B_E506886F36CF_

#include "framecommon/framecommon.h"
#include "classfactory.h"
#include "PingSrvBaseCommand.h"

class CPingSrvClassFactory : public CClassFactory
{
public:
	CPingSrvClassFactory(){}
	~CPingSrvClassFactory(){}
	virtual CSocketTask* GenSocketTask(CNetProcessThread *pWorkThread, CTcpSocket *pTcpSocket,
		int iTaskType, CTaskArg *pTaskArg = NULL);
	virtual CWorkThread* GenWorkThread(int nEntityType, int nEntityId, string sAnnexData, void *arg=NULL);
	virtual CCommand* GenBaseCommand(char *pBuf, int nLen, int nClientIp, short nClientPort, int nEntityType, int nEntityId);
	virtual CBaseMaintainCmd* GenMaintainCmd(int nMsgType, int nBufLen, char* pBuf);
};

#endif



