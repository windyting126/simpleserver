/**
 * @file configupdatetask.h
 * @author yanshi <rocketrock@163.com>
 * @date 2013-03-11
 */

#ifndef _CONFIGUPDATETASK_H_
#define _CONFIGUPDATETASK_H_

/// configure update task.
class CConfigUpdateTask
{
public:
	/**
	 * singleton method.
	 * @return singleton
	 */
	static CConfigUpdateTask& GetInstance();
	/**
	 * update configure from pmgrsrv DB.
	 * @return 0 succeed; otherwise fail
	 */
	int UpdateConfig();
private:
	/// constructor.
	CConfigUpdateTask() {}
	/**
	 * copy constructor.
	 * @attention not implement
	 */
	CConfigUpdateTask(const CConfigUpdateTask&);
	/**
	 * assign opertator.
	 * @attention not implement
	 */
	CConfigUpdateTask& operator=(const CConfigUpdateTask&);
};

#endif // _CONFIGUPDATETASK_H_


