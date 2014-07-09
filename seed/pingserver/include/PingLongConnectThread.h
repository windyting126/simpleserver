/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingLongConnectThread.h
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
#ifndef _PINGLONGCONNECTTHREAD_H_2CAA596A_342C_45c3_B717_119EEFA087C6_
#define _PINGLONGCONNECTTHREAD_H_2CAA596A_342C_45c3_B717_119EEFA087C6_

#include "LongConnectedThread.h"

class CPingLongConnectThread : public CLongConnectedThread
{
public:
	CPingLongConnectThread(int nEntityType, int nEntityId, int nListType = 0) 
		: CLongConnectedThread(nEntityType, nEntityId, nListType)
	{
	}
	~CPingLongConnectThread(){}
	string GetSendListLog();
private:
	virtual int Process(CMsg *pMsg);
};


#endif

