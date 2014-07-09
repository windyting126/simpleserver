#include "framecommon/framecommon.h"
#include "myclassfactory.h"
#include "insertrctoolsdef.h"
#include "mylongconnectthread.h"
#include "mylongconnecttask.h"

using namespace MYFRAMECOMMON;

CMyClassFactory::CMyClassFactory()
{
}

CMyClassFactory::~CMyClassFactory()
{
}

CWorkThread* CMyClassFactory::GenWorkThread(int nEntityType, int nEntityId, string sAnnexData, void* arg)
{
	CWorkThread* pWorkThread = NULL;

	switch (nEntityType)
	{
		case LONGCONNECTEDENTITY:
			{
				pWorkThread = new CMyLongConnectedThread(nEntityType, nEntityId);
				break;
			}
		default:
			{
				pWorkThread = CClassFactory::GenWorkThread(nEntityType, nEntityId, sAnnexData, arg);
			}
	}

	if (NULL == pWorkThread)
	{
		WriteRunInfo::WriteError ("Invalid thread entity %d", nEntityType);
	}
	return pWorkThread;
}

CSocketTask* CMyClassFactory::GenSocketTask(CNetProcessThread* pWorkThread, CTcpSocket* pTcpSocket, int nTaskType, CTaskArg* pTaskArg)
{
	CSocketTask* pSocketTask = NULL;

	switch (nTaskType)
	{
		case COMMONTASK:
			{
				if (pTaskArg)
				{
					CCommonTaskArg* pCommonTaskArg = (CCommonTaskArg*)pTaskArg;
					pSocketTask = new CMyLongConnectTask(pTcpSocket, pWorkThread, pCommonTaskArg->GetDestMachineType(), pCommonTaskArg->GetDestMachineIndex(), pCommonTaskArg->IsActive(), pCommonTaskArg->GetListType());
				}
				break;
			}
		default:
			pSocketTask = CClassFactory::GenSocketTask (pWorkThread, pTcpSocket, nTaskType);
	}

	return pSocketTask;
}



