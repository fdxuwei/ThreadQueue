#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <boost/atomic.hpp>

class SpinLock 
{
public:
	SpinLock() : state_(Unlocked) {}
	void lock()
	{
		while (state_.exchange(Locked, boost::memory_order_acquire) == Locked) 
		{
			/* busy-wait */
		}
	}
	void unlock()
	{
		state_.store(Unlocked, boost::memory_order_release);
	}

private:
	typedef enum {Locked, Unlocked} LockState;
	boost::atomic<LockState> state_;
};

#endif

