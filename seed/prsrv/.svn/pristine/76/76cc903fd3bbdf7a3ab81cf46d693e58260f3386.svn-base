#include "statistic.h"

CCmdCounter* CCmdCounter::m_hHandler = NULL;

CStatisticItem::CStatisticItem(int nId, string &strDesp)
{
	m_nId = nId;
	m_strDescription = strDesp;
	m_Count = new CAtomic32;
}

CStatisticItem::~CStatisticItem()
{
	delete m_Count;
}

int CStatisticItem::AddCount(int nCount /* = 1 */)
{
	m_Count->Add(nCount);
	return 0;
}

int CStatisticItem::GetCount()
{
	return m_Count->Get();
}

int CStatisticItem::ClearCount()
{
	m_Count->Set(0);
	return 0;
}

CStatistic::CStatistic()
{
	m_nLogType = 0;
	m_nStartTime = 0;
}

CStatistic::~CStatistic()
{
	m_statLock.GetWriteLock();
	vector<CStatisticItem*>::iterator itrStatItem = m_vecStatItem.begin();
	while (itrStatItem != m_vecStatItem.end())
	{
		if (*itrStatItem != NULL)
		{
			delete *itrStatItem;
		}
		itrStatItem ++;
	}
	m_statLock.ReleaseLock();
}

int CStatistic::Init(string strTypeName, int nLogType, int nBucketLen /* = 2048 */)
{
	if (nBucketLen <= 0)
	{
		return -1;
	}
	m_strTypeName = strTypeName;
	m_nLogType = nLogType;
	m_vecStatItem.reserve(nBucketLen);
	for(int i = 0; i < nBucketLen; i ++)
	{
		m_vecStatItem.push_back(NULL);
	}

	struct timeval tvTmp;
	gettimeofday(&tvTmp, NULL);
	m_nStartTime = tvTmp.tv_sec;

	return 0;
}

int CStatistic::WriteLog(bool ifClear /* = true */)
{
	struct timeval tvTmp;
	gettimeofday(&tvTmp, NULL);
	int nUseTime = tvTmp.tv_sec - m_nStartTime;
	if (ifClear)
	{
		m_nStartTime = tvTmp.tv_sec;
	}

	stringstream statStr;
	statStr.str("");
	statStr << "<<" << m_strTypeName << ">>:";
	statStr << "last " << nUseTime << " sec:";

	m_statLock.GetReadLock();
	vector<CStatisticItem*>::iterator itrStatItem = m_vecStatItem.begin();
	while (itrStatItem != m_vecStatItem.end())
	{
		if (*itrStatItem == NULL)
		{
			itrStatItem ++;
			continue;
		}

		statStr << "[" << (*itrStatItem)->GetDescription() << ":" << (*itrStatItem)->GetId() << ":" << (*itrStatItem)->GetCount() << "]";
		if (ifClear)
		{
			(*itrStatItem)->ClearCount();
		}

		itrStatItem ++;
	}
	m_statLock.ReleaseLock();

	WRITE_ALL_LOG(m_nLogType, "%s", statStr.str().c_str());
	return true;
}

int CStatistic::AddItem(int nId, string strDesp)
{
	if(nId >= (int)m_vecStatItem.size())
	{
		return -1;
	}
	
	int nRet = -1;
	m_statLock.GetWriteLock();
	if (m_vecStatItem[nId] == NULL)
	{
		CStatisticItem *pStatItem = new CStatisticItem(nId, strDesp);
		m_vecStatItem[nId] = pStatItem;
		nRet = 0;
	}
	m_statLock.ReleaseLock();
	return nRet;
}

int CStatistic::AddCount(int nId, int nCount /* = 1 */)
{
	int nRet = -1;
	m_statLock.GetReadLock();
	if (m_vecStatItem[nId] != NULL)
	{
		m_vecStatItem[nId]->AddCount(nCount);
		nRet = 0;
	}
	m_statLock.ReleaseLock();
	return nRet;
}

int CCmdCounter::Create()
{
	if (m_hHandler == NULL)
	{
		m_hHandler = new CCmdCounter;
	}
	return 0;
}

int CCmdCounter::Destory()
{
	if (m_hHandler != NULL)
	{
		delete m_hHandler;
		m_hHandler = NULL;
	}
	return 0;
}
