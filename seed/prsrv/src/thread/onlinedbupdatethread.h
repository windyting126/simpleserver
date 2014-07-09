/* Copyright:2009-
* Version		: 1.0
* FileName		: onlinedbupdatethread.h
* Description	: 
*---- Class List	: 
*---- Function List	: 
*---- Variable List : 
*---- Related Files : 
* Author:       : wangtiefeng
* Created Date	: 2009-1-14
* 
* History		: 
*		<author>		  <time>		<version>		<description>
*		wangtiefeng		2009-1-14			1.0			
*/

#ifndef __ONLINEDBUPDATETHREAD_H
#define __ONLINEDBUPDATETHREAD_H

#include "commonthread.h"

class COnlineDBUpdateThread : public CCommonThread
{
public:
	COnlineDBUpdateThread();
	~COnlineDBUpdateThread();
	
	virtual int Run();
	virtual int Init();

protected:
	int UpdateRcOnlineTable(int nIndex);
	int CreateRcOnlineTempTable(int nIndex);
	int DeleteRcOnlineTempTable(int nIndex);
	int InsertPeerInfo(int nIndex, long &lPeerNum);
	int MergeTable(int nIndex);
private:
	int m_iOntimeInterval;
	int m_nOnlineDBNum;
	int m_nStatInterval;
	
};
#endif
