/**================================================
 *=================================================
 * @file atomic32.h
 
 * 	Atomic operations that C/C++ can't guarantee us.  Useful for resource counting etc..
 
 * @author Chen Hui (chenhui101@gmail.com) 
 
 * @version 1.0
 
 * @date 2012-11-19
 
 *=================================================
 */


#ifndef __ATOMIC32_H__
#define __ATOMIC32_H__

namespace MYFRAMECOMMON{

#define LOCK "lock ; "



class CAtomic32
{
	/**
	 * Make sure gcc doesn't try to be clever and move things around
	 * on us. We need to use _exactly_ the address the user gave us,
	 * not some alias that contains the same information.
	 */
	typedef struct { volatile int counter; } atomic_t;

	#define ATOMIC_INIT(i)	{ (i) }

	/**
	 * atomic_read - read atomic variable
	 * @param v : pointer of type atomic_t
	 * 
	 * Atomically reads the value of v.
	 */ 
	#define atomic_read(v)		((v)->counter)

	/**
	 * atomic_set - set atomic variable
	 * @param v : pointer of type atomic_t
	 * @param i : required value
	 * 
	 * Atomically sets the value of v to i.
	 */ 
	#define atomic_set(v,i)		(((v)->counter) = (i))

public:
	CAtomic32(int i = 0) { Set(i); }

	/*following interface guarantee atomic*/
	inline int Get() { return atomic_read(&m_Int32); }
	inline void Set(int i) { atomic_set(&m_Int32, i); }
	inline void Inc() { atomic_inc(&m_Int32); }
	inline void Dec() { atomic_dec(&m_Int32); }
	inline int Add(int i) { return atomic_add_return(i, &m_Int32); }
	inline int Sub(int i) { return atomic_sub_return(i, &m_Int32); }

private:

	/**
	 * atomic_inc - increment atomic variable
	 * @param v : pointer of type atomic_t
	 * 
	 * Atomically increments v by 1.
	 */ 
	static inline void atomic_inc(atomic_t *v)
	{
		__sync_add_and_fetch(&v->counter, 1);
	}

	/**
	 * atomic_dec - decrement atomic variable
	 * @param v : pointer of type atomic_t
	 * 
	 * Atomically decrements v by 1.
	 */ 
	static inline void atomic_dec(atomic_t *v)
	{
		__sync_sub_and_fetch(&v->counter, 1);
	}

	/**
	 * atomic_add_return - add and return
	 * @param i : integer value to add
	 * @param v : pointer of type atomic_t
	 *
	 * Atomically adds i to v and returns i + v
	 */
	static inline int atomic_add_return(int i, atomic_t *v)
	{
		return __sync_add_and_fetch(&v->counter, i);
	}

	/**
	 * atomic_sub_return - sub and return
	 * @param i : integer value to add
	 * @param v : pointer of type atomic_t
	 *
	 * Atomically decrements v by i and returns i - v
	 */
	static inline int atomic_sub_return(int i, atomic_t *v)
	{
		return __sync_sub_and_fetch(&v->counter, i);
	}
	
private:
	atomic_t m_Int32;
};

} //end namespace MYFRAMECOMMON

#endif

