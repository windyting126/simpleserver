/**
 *    @file       CmdCounter.cpp
 *
 *    @brief      command recv and send counters
 *    @details
 *
 *    @version    1.0
 *    @date       11/12/2012 06:07:18 PM
 *
 *    @author     wangtiefeng (), wangtf418@163.com
 */
#include "CmdCounter.h"
//static member==================
CCmdCounter* CCmdCounter::m_hHandler = NULL;

int CCmdCounter::Create()
{
	if (!m_hHandler)
	{
		m_hHandler = new CCmdCounter;
	}
	return 0;
}

int CCmdCounter::Destory()
{
	if (m_hHandler)
	{
		delete m_hHandler;
	}
	return 0;
}

//member========================
CCmdCounter::CCmdCounter()
{

}

CCmdCounter::~CCmdCounter()
{

}


int CCmdCounter::Init(const string& strCounterName, unsigned int nCounterNum)
{
	int nRet = CFastCounter::Init(strCounterName, nCounterNum);
	if (nRet != 0)
	{
		return nRet;
	}
	for (unsigned int nA = 0; nA < nCounterNum; nA ++)
	{
		char szTmpBuf[100];
		snprintf(szTmpBuf, sizeof(szTmpBuf), "%s%d", strCounterName.c_str(), nA);
		nRet += AddCounter(nA, szTmpBuf);
	}
	return 0;
}




