/*
* FileName:       
* Author:         wangqifeng  Version: 1.0  Date: 2005-9-4
* Description:    
* Version:        
* Function List:  
*                 1.
* History:        
*     <author>   <time>    <version >   <desc>
*/
#include "threadgroup.h"
#include "myserver.h"

CThreadGroup::~CThreadGroup()
{
}

int CThreadGroup::PutMsgToThread(CMsg *pMsg)
{
	TMsg *pTMsg = pMsg->GetTMsg();
	if (pTMsg->destEntityType != m_iThreadEntityType) //�����򵥵��ж�
	{
		//д��־�����˴���Ϣ
		WriteRunInfo::WriteLog("Error entity type(%d) , %d", pTMsg->destEntityType, m_iThreadEntityType);

		////ֱ�ӽ�����Ϣɾ��
		delete pMsg;
		return -1;
	}

	if (pTMsg->destEntityId == RANDOMENTITYID) //δָ��entityid, ��ѭ
	{                
		m_rotationMutex.Lock();
		m_threadList[m_iRotationEntityId]->PutMsgToInputList(pMsg);               
		m_iRotationEntityId ++;
		m_iRotationEntityId = m_iRotationEntityId%m_iEntityIdSize;
		m_rotationMutex.UnLock();
	}
	else  //ָ��entityid
	{
		if (pTMsg->destEntityId < 0 || pTMsg->destEntityId >= m_iEntityIdSize) //ʵ��ID��Χ����ȷ
		{
			//д��־�����˴���Ϣ
			WriteRunInfo::WriteLog("Error entityid (%d) , %d", pTMsg->destEntityId, m_iEntityIdSize);
			//ֱ�ӽ�����Ϣɾ��
			delete pMsg;
			return -1;
		}
		//����ͽ���Ϣ������Ӧ���߳���
		m_threadList[pTMsg->destEntityId]->PutMsgToInputList(pMsg);
	}
	return 0;        
}

int CThreadGroup::Init()
{    
	for (int i=0; i < m_iEntityIdSize; i++)
	{
		CWorkThread *pThread = CMyserver::GetInstance()->GetClassFactory()->GenWorkThread(m_iThreadEntityType, i, m_sAnnexData, 
			m_arg);
		if (pThread == NULL)
		{
			WriteRunInfo::WriteLog("Malloc memory error");
			return -1;
		}
		int ret = pThread->Start();
		if (ret != 0)
		{
			WriteRunInfo::WriteLog("Start thread fail");
			return -1;
		}
		WriteRunInfo::WriteLog("The threadid is %d, pid is %d", pThread->GetThreadId(), pThread->GetPid());
		m_threadList.push_back(pThread);
	} 
	//������߳���ע�ᵽ��������
	CMyserver::GetInstance()->RegisterThreadGroup(this);
	return 0;
}



int CThreadGroup::GetRunFlag()
{
	int ret = 0;
	for (unsigned int i=0; i < m_threadList.size(); i++)
	{
		ret += m_threadList[i]->GetRunFlag();
	}
	return ret;
}

int CThreadGroup::StopAllThread()
{
	for (unsigned int i=0; i < m_threadList.size(); i++)
	{
		m_threadList[i]->Stop();
	}
	return 0;
}

