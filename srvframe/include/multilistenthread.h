#ifndef __MULTILISTENTHREAD_
#define __MULTILISTENTHREAD_
#include "listenthread.h"


class CMultiListenThread:public CListenThread
{
public:
	CMultiListenThread(int iEntityType, int iEntityId, 
	 	string sHost, int nPort, TListenBlock *pListenBlock):CListenThread(iEntityType, 
	 	iEntityId, sHost, nPort, pListenBlock)
 	{
 	}
   ~CMultiListenThread()
	{
	}

   struct TListenInfo
   {
		string m_strHost;
		unsigned short m_nPort;
		TListenBlock ListenBlock;
		TListenInfo(const string& strHost, unsigned short nPort) : m_strHost(strHost), m_nPort(nPort)
		{

		}
   };
	int Init();
	static int AddListen(const string& sHost, unsigned short nPort);
private:
	static CThreadMutex m_ListenListLock;;
	static list<TListenInfo> m_lstListenInfo;
};
#endif
