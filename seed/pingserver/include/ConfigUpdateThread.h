/**
 * @file ConfigUpdateThread.h
 * @author yanshi <yanshi@baofeng.com>
 * @date 2013-03-07
 */

#ifndef _CONFIGUPDATETHREAD_H_
#define _CONFIGUPDATETHREAD_H_

#include "workthread.h"

/// configure update thread.
class CConfigUpdateThread : public CWorkThread
{
public:
	/**
	 * constructor.
	 * @param[in] iEntityType, entity type
	 * @param[in] iEntityId, entity id
	 */
	CConfigUpdateThread(int iEntityType, int iEntityId);
	/// destructor.
	virtual ~CConfigUpdateThread() {}
	/**
	 * run thread.
	 * @return 0 succeed; otherwise fail
	 */
	virtual int Run();
	/**
	 * put message to input list.
	 * @param[in] pMsg message to put
	 * @return 0 succeed; otherwise fail
	 */
	virtual int PutMsgToInputList(CMsg *pMsg);
protected:
	/**
	 * process message.
	 * @param[in] pMsg message to process
	 * @return 0 succeed; otherwise fail	 
	 */
	virtual int Process(CMsg *pMsg);
};

#endif // _CONFIGUPDATETHREAD_H_

