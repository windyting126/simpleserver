/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: main.cpp
* Description	: CLoadHashTable¿‡µ•‘™≤‚ ‘
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
int main(int argc, char* argv[])
{
	CLoadHashTable LoadHashTable("test.txt");
	int nRet = LoadHashTable.Init();
	if (nRet != 0)
	{
		cout << "Init fail" << endl;
		return -1;
	}
	
	string strLine;
	while ((strLine = LoadHashTable.GetLine()) != "")
	{
		cout << strLine << endl;
	}

	int nLastError = LoadHashTable.GetLastError();
	cout << "LastError=" << nLastError << endl;
	return 0;
}

