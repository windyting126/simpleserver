/* Copyright:2009-
* Version		: 1.0
* FileName		: externalcheckcmd.h
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
*		wangtiefeng		2009-3-26			1.0			
*/

#ifndef __EXTERNALCHECKCMD_H
#define __EXTERNALCHECKCMD_H
#include <assert.h>
#include "basemaintaincmd.h"
#include "pingpeerhashtable.h"
#include "commmsg.h"

class CExternalCheckCmd : public CBaseMaintainCmd
{
public:
	CExternalCheckCmd(int nMsgType, int nBodyLen, char *pBody):CBaseMaintainCmd( nMsgType , nBodyLen , pBody )
	{
		m_pPeerHashTable = CPingPeerHashTable::GetHashTable();
		assert(m_pPeerHashTable);
	}
	virtual ~CExternalCheckCmd()
	{
	}
	virtual int Process();

	
protected:
	CPingPeerHashTable *m_pPeerHashTable;
private:
};
#endif

