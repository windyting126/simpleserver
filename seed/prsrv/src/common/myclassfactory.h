/*
 * FileName:       
 * Author:         wangqifeng  Version: 1.0  Date: 2009-1-4
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version >   <desc>
 */
#ifndef __MY_CLASSFACTORY_H
#define __MY_CLASSFACTORY_H

#include "classfactory.h"
#include "seedbasecommand.h"

class CMyClassFactory:public CClassFactory
{
public:
  CMyClassFactory(){}
  ~CMyClassFactory(){}
  CSocketTask *GenSocketTask(CNetProcessThread *pWorkThread, CTcpSocket *pTcpSocket,
                int iTaskType, CTaskArg * pTaskArg = NULL);
  CWorkThread *GenWorkThread(int nEntityType, int nEntityId, string sAnnexData, 
	  void *arg=NULL);
  virtual CBaseMaintainCmd *GenMaintainCmd( int nMsgType, int nBufLen, char *pBuf );
  CCommand* GenBaseCommand(char* pBuf, int nLen, int nIp);

};
#endif

