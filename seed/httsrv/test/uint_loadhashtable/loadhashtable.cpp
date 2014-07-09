/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: loadhashtable.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2010-05-27
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2010-05-27			1.0			
* =======================================================================
*/
#include "loadhashtable.h"

int CLoadHashTable::Init()
{
	int nOpenFlag =  O_RDONLY;
	m_nFileFd = open(m_strFileName.c_str(), nOpenFlag);
	if (m_nFileFd == -1)
	{
		WriteRunInfo::WriteLog("CLoadHashTable::Init : open file %s Failed!", m_strFileName.c_str());
		return -1;
	}
	m_bIfOpenFile = true;

	struct stat FileStat;
	if (stat(m_strFileName.c_str(), &FileStat) != 0)
	{
		WriteRunInfo::WriteLog("CLoadHashTable::Init: stat file %s Failed!", m_strFileName.c_str());
		return -1;
	}

	m_nFileLength = FileStat.st_size;
	m_pBuf = new char[E_READ_BUFSIZE];
	return 0;
}

string CLoadHashTable::GetLine()
{
	string strLine = "";
	if (m_nListSize > 0)
	{
		strLine = m_lstLine.front();
		m_lstLine.pop_front();
		m_nListSize --;
		return strLine;
	}

	if (m_nCurOffset >= m_nFileLength)
	{
		return strLine;
	}

	int nReadLen = read(m_nFileFd, m_pBuf, E_READ_BUFSIZE);
	if (nReadLen <= 0)
	{
		WriteRunInfo::WriteLog("CLoadHashTable::GetLine: read file %s Failed!", m_strFileName.c_str());
		m_nLastError = -1;
		return strLine;
	}
	m_nCurOffset += nReadLen;

	m_strData.append(m_pBuf, nReadLen);
	int nDataLen = (int)m_strData.length();
	int nCurPos = 0;
	const char* pData = m_strData.c_str();
	for (int nA = 0; nA < nDataLen; ++ nA)
	{
		if (*(pData + nA) == '\n')
		{
			if (nCurPos == nA)
			{
				nCurPos ++;
				continue;
			}
			else
			{
				m_lstLine.push_back(string(pData + nCurPos, nA - nCurPos));
				m_nListSize ++;
				nCurPos = nA + 1;
			}
		}
	}

	if (nCurPos < nDataLen)
	{
		m_strData.assign(pData + nCurPos, nDataLen - nCurPos);
		if (m_strData.length() > MAX_LINE)
		{
			WriteRunInfo::WriteLog("CLoadHashTable::GetLine: too long line!");
			m_nLastError = -1;
			return strLine;
		}		
	}
	if (m_nListSize <= 0)
	{
		WriteRunInfo::WriteLog("CLoadHashTable::GetLine: wrong format!");
		m_nLastError = -1;
		return strLine;
	}
	else
	{
		strLine = m_lstLine.front();
		m_lstLine.pop_front();
		m_nListSize --;
		return strLine;
	}
}


