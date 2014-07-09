#include "mylongconnectthread.h"
#include "insertrctoolsdef.h"

CMyLongConnectedThread::CMyLongConnectedThread(int nEntityType, int nEntityId, int nListType)
	: CLongConnectedThread (nEntityType, nEntityId, nListType)
{
}

CMyLongConnectedThread::~CMyLongConnectedThread()
{
}

int CMyLongConnectedThread::Process(CMsg *pMsg)
{
	return CLongConnectedThread::Process(pMsg);
}
