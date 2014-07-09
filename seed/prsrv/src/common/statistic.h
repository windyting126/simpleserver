#ifndef __STATISTIC_H__
#define __STATISTIC_H__

#include "framecommon/framecommon.h"

class CStatisticItem
{
public:
	CStatisticItem(int nId, string &strDesp);
	~CStatisticItem();
	int AddCount(int nCount = 1);
	int GetCount();
	int ClearCount();
	int GetId()
	{
		return m_nId;
	}
	string& GetDescription()
	{
		return m_strDescription;
	}
protected:
private:
	int m_nId;
	string m_strDescription;
	CAtomic32* m_Count;
};

class CStatistic
{
public:
	CStatistic();
	~CStatistic();

	int Init(string strTypeName, int nLogType, int nBucketLen = 2048);
	int WriteLog(bool ifClear = true);

	int AddItem(int nId, string strDesp);
	int AddCount(int nId, int nCount = 1);
private:
	string m_strTypeName;
	int m_nLogType;
	int m_nStartTime;

	vector<CStatisticItem*> m_vecStatItem;
	CThreadRwLock m_statLock;
};

class CCmdCounter : public CStatistic
{
private:
	CCmdCounter(){}
	~CCmdCounter(){}

public:
	static int Create();
	static int Destory();
	static CCmdCounter* GetHandler()
	{
		return m_hHandler;
	}
private:
	static CCmdCounter* m_hHandler;
};

#endif
