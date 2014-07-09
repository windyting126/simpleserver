/**
 * @file pingsynprocessthread.h
 * @author yanshi <yanshi@baofeng.com>
 * @date 2013-03-11
 */

#ifndef _PINGSYNPROCESSTHREAD_H_
#define _PINGSYNPROCESSTHREAD_H_

#include "synworkthread.h"

class CPingPeerHashTable;

/// thread processing pingserver's syn message.
class CPingSynProcessThread : public CSynWorkThread
{
public:
	/**
	 * constructor.
	 * @param[in] iEntityType thread entity type
	 * @param[in] iEntityId thread entity id
	 */
	CPingSynProcessThread(int iEntityType, int iEntityId);
	/// destructor.
	virtual ~CPingSynProcessThread() {}
	/**
	 * initialize thread.
	 * @return 0 succeed; otherwise fail
	 */
	virtual int Init();
protected:
	/**
	 * process message.
	 * @param[in] pMsg message to process
	 * @return 0 succeed; otherwise fail
	 */
	virtual int Process(CMsg *pMsg);
	/**
	 * process syn ping message.
	 * @param[in] pBuf point to message
	 * @param[in] iLength message length
	 * @return 0 succeed; otherwise fail	 
	 */
	int ProcessSynPing(char *pBuf, int iLength);
	/**
	 * process syn logout message.
	 * @param[in] pBuf point to message
	 * @param[in] iLength message length
	 * @return 0 succeed; otherwise fail	 
	 */
	int ProcessSynLogout(char *pBuf, int iLength);
	/**
	 * process syn ontime message.
	 * @param[in] pBuf point to message
	 * @param[in] iLength message length
	 * @return 0 succeed; otherwise fail	 
	 */
	int ProcessSynOntime(char *pBuf, int iLength);
protected:
	CPingPeerHashTable *m_pPeerHashTable; ///< peer hash table
};

#endif // _PINGSYNPROCESSTHREAD_H_

