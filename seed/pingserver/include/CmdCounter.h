/**
 *    @file       CmdCounter.h
 *
 *    @brief      command recv and send counters
 *    @details
 *
 *    @version    1.0
 *    @date       11/12/2012 05:54:07 PM
 *
 *    @author     wangtiefeng (), wangtf418@163.com
 */

#ifndef _CMDCOUNTER_H_DC8ADFF3_B4A4_4712_ADD5_DA24DA9DEF10_
#define _CMDCOUNTER_H_DC8ADFF3_B4A4_4712_ADD5_DA24DA9DEF10_

#include "fastcounter.h"
//const int INVALID_CMD_COUNTTYPE = 256;
//const int COUNTERS_NUMBER = 257;

class CCmdCounter : public CFastCounter
{
private:
	CCmdCounter();
	~CCmdCounter();

public:
	static int Create();
	static int Destory();
	static CCmdCounter* GetHandler()
	{
		return m_hHandler;
	}

public:
	int Init(const string& strCounterName, unsigned int nCounterNum);

private:
	static CCmdCounter* m_hHandler;
};

#endif
