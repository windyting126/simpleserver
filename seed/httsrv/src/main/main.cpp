/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: main.cpp
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
#include <iostream>
#include <string>
#include "normalserver.h"
#include "framecommon/framecommon.h"

using namespace std;
using namespace MYFRAMECOMMON;

void MainWork()
{
	while(1)
	{
		sleep(1000);
	}
}

int main(int argc, char *argv[])
{
	if (argc == 2 && string(argv[1]) == "-l")
	{
		WriteRunInfo::SetLogFlag(TRACETYPE, 1);			
	}

/*	TLogType LogType;
	LogType.nLogtype = LOGTYPE_STATUS;
	LogType.nFileNum = 0;
	LogType.nLogOff = 1;
	strncpy(LogType.sFileNameSuffix, ".status", sizeof(LogType.sFileNameSuffix));
	WriteRunInfo::LOGTYPEARRAY.InsertLogType(LogType);
	WriteRunInfo::Init("logs/pingserver");
*/	//Trace
	//WriteRunInfo::SetLogFlag(TRACETYPE, 1);
	//WriteRunInfo::SetLogFlag(DATAINFO, 1);
	//WriteRunInfo::SetLogFlag(RUNLOGTYPE, 1);

	CNormalServer normalServer;
	normalServer.SetServer(&normalServer);
	int iRet = normalServer.PreStartServer();
	if (iRet != 0)
	{
		printf("Server start fail!\n");
//		WriteRunInfo::WriteLog("Server start fail");
		return -1;
	}
	
//	WriteRunInfo::WriteLog("Server StartServer ok");
	printf("Server start ok!\n");
	MainWork();
	return 0;
}


