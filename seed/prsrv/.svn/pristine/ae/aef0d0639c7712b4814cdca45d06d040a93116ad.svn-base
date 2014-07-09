
#ifndef __MY_CLASS_FACTORY_H
#define __MY_CLASS_FACTORY_H

#include "classfactory.h"

class CMyClassFactory
	: public CClassFactory
{
public:
	CMyClassFactory();
	~CMyClassFactory();

	virtual CWorkThread* GenWorkThread(int nEntityType, int nEntityId, string sAnnexData, void* arg = NULL);

	virtual CSocketTask* GenSocketTask(CNetProcessThread* pWorkThread, CTcpSocket* pTcpSocket, int nTaskType, CTaskArg* pTaskArg = NULL);
};


#endif


