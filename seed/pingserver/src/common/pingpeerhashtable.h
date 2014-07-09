/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: pingpeerhashtable.h
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
#ifndef _PINGPEERHASHTABLE_H_2C4F0C46_643C_45d9_BB65_E02B1055DE54_
#define _PINGPEERHASHTABLE_H_2C4F0C46_643C_45d9_BB65_E02B1055DE54_

#include <string>
#include <list>
#include <time.h>
#include <pthread.h>

#include "commmsg.h"
#include "framecommon/framecommon.h"
#include "threadgroup.h"
#include "myserver.h"
#include "pingpeerdef.h"

//维护命令
const int ONLINE_DUMP_HASHTABLE = 2002;	//dump Hash表中的Peer信息
const int ONLINE_LOAD_HASHTABLE = 2003;	//load Peer信息到Hash表

using namespace std;
using namespace MYFRAMECOMMON;

class CPingPeerHashTable : public CMyHashTable<TPingPeerInfo, list<TPingPeerInfo> >
{
public:
	static int CreateHashTable(int nHashBucketLen, unsigned char nMaxPipesNum = 100, float fBusyPercent = 0.8, float fNormalPercent = 0.5 , int iIfLOCK = 1)
	{
		if (nHashBucketLen <= 0)
		{
			WriteRunInfo::WriteLog("Create Peer Hash Table Failed : nHashBucketLen is less than 0!");
			return -1;
		}
		if(!m_pHashTable)
		{
			m_pHashTable = new CPingPeerHashTable(nHashBucketLen, nMaxPipesNum, fBusyPercent, fNormalPercent, iIfLOCK);
		}
		return 0;
	}
	static void DestroyHashTable()
	{
		if (m_pHashTable)
		{
			delete m_pHashTable;
			m_pHashTable = NULL;
		}		
	}
	static CPingPeerHashTable * GetHashTable()
	{
		return m_pHashTable;
	}
	int Init()
	{
		m_pServer = CMyserver::GetInstance();
		if(!m_pServer)
		{
			WriteRunInfo::WriteLog("Init Peer Hash Table Failed : m_pServer is NULL!");
			return -1;
		}
		return 0;
	}
public:
	//Return 0 when Peer is exist and not changed ,and return 1 when added new Peer or updated exist Peer.
	int AddPeer(TPingPeerInfo &PeerInfo);

	//Return 0 when the Peer is not exist , return 1 when deleted the Peer.
	int DeletePeer(const string &strPeerId);

	//Check if the Peer is exist.Return 1 when the Peer is exist ,return 0 when not exist.
	int FindPeer(const string &strPeerId, TPingPeerInfo &PeerInfo) const;

	//Check Peer time. Return 1 when the Peer is exit , return 0 when not exist.
	int GetPeerTime(const string &strPeerId, unsigned long long &ullPeerTime) const;

	//Set Peer time. Return 1 when the Peer is exist , return 0 when not exist.
	int SetPeerTime(const string &strPeerId, unsigned long long ullPeerTime);
	
	//Return 0 when the Peer is not exist , return 1 when delete the Peer.
	int SetLogoutFlag(const string &strPeerId);

	//Return 0 when successful and return -1 when failed.
	int UpdatePeerList(const list<TPingPeerInfo> &listPeerInfo, int iHashIndex);

	//Return 0 when successful and return -1 when failed.
	int GetPeerList(list<TPingPeerInfo> &listPeerInfo, int iHashIndex) const;

	//Return 0 when successful and return -1 when failed.
	int CheckPeerList(list<TPingPeerInfo> &listPeerInfo, int iHashIndex, int nTimeOut);

	int GetHashLength()
	{
		return m_nHashBucketLen;
	}
	void PrintNodeNumber()
	{
		WRITE_TRACE("PingPeerHashTable , Node Num = %d", m_nNodeNum);
	}
protected:
	char GetUploadPipeState(char nCurUploadPipeNum);
	char GetUploadPipeState(char nCurUploadPipeNum, char nLastState);
private:
	CPingPeerHashTable(int nHashBucketLen, unsigned char nMaxPipesNum = 100, float fBusyPercent = 0.8, float fNormalPercent = 0.5, int iIfLOCK = 1) 
		: CMyHashTable<TPingPeerInfo, list<TPingPeerInfo> >(nHashBucketLen, iIfLOCK)
	{
		m_nMaxPipeNum = nMaxPipesNum;
		m_fBusyPercent = fBusyPercent;
		m_fNormalPercent = fNormalPercent;
	}
	~CPingPeerHashTable()
	{
	}
private:
	static CPingPeerHashTable *m_pHashTable;
	CMyserver *m_pServer;
	unsigned char m_nMaxPipeNum;
	float m_fBusyPercent;
	float m_fNormalPercent;
};


#endif


