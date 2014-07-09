#include "insertrctools.h"
#include "insertrctoolsdef.h"
#include "framecommon/framecommon.h"
using namespace std;
using namespace MYFRAMECOMMON;

int main (int argc, char** argv)
{
	CInsertTools::InitLogSystem();
		
	WriteRunInfo::WriteLog ("Begin start pconsrv ...");

	CInsertTools* pServer = new CInsertTools();
	CMyserver::SetServer (pServer);
	if (pServer->InitServer() != 0)
	{
		WriteRunInfo::WriteError ("Init server fail");
		return -1;
	}
	if (pServer->StartServer() != 0)
	{
		WriteRunInfo::WriteError ("Start server fail");
		return -1;
	}

	sleep (1);
	return 0;
}


