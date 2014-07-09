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
#include "framecommon/framecommon.h"
#include "PingServer.h"
#include "PingServerDef.h"

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

	TLogType LogType;
	LogType.nLogtype = LOGTYPE_STATUS;
	LogType.nFileNum = 0;
	LogType.nLogOff = 1;
	strncpy(LogType.sFileNameSuffix, ".status", sizeof(LogType.sFileNameSuffix));
	WriteRunInfo::LOGTYPEARRAY.InsertLogType(LogType);
	WriteRunInfo::Init("logs/pingserver");
	//Trace
	//WriteRunInfo::SetLogFlag(TRACETYPE, 1);
	//WriteRunInfo::SetLogFlag(DATAINFO, 1);
	//WriteRunInfo::SetLogFlag(RUNLOGTYPE, 1);

	CPingServer PingServer;
	PingServer.SetServer(&PingServer);
	int iRet = PingServer.InitServer();
	if (iRet != 0)
	{
		WriteRunInfo::WriteLog("Server init fail");
		return -1;
	}
	if (PingServer.StartServer() != 0)
	{
		WriteRunInfo::WriteLog("Server StartServer fail");
		return -1;	
	}

	WriteRunInfo::WriteLog("Server StartServer ok");
	MainWork();
	return 0;
}


