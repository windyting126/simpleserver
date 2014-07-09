#include <string>
#include "rserver.h"
#include "pingpeerdef.h"

using namespace std;

CPRServer *g_pRServer;

int main(int argc, char **argv)
{
	if (argc == 2 && string(argv[1]) == "-l")
	{
		WriteRunInfo::SetLogFlag(TRACETYPE, 1);			
	}

	CPRServer::InitLogSystem();	
	g_pRServer = new CPRServer();
	CMyserver::SetServer(g_pRServer);

	int ret = g_pRServer->InitServer();
	if (ret != 0)
	{
		WriteRunInfo::WriteLog("Server init failed");
		return -1;
	}

	g_pRServer->StartServer();

	return 0;
}


