#ifndef __BLOCKQUEUE2_H_
#define __BLOCKQUEUE2_H_

#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>
#include <deque>
#include "SpinLock.h"

// template class
// data queue, thread safe
template<class T>
class BlockQueue2
{
public:
	typedef boost::shared_ptr<T> DataPtr;
	void push(DataPtr data);
	DataPtr pop(int timeout); //ms
private:
	typedef std::deque<DataPtr> QueueType;
	typedef SpinLock MutexType;
	typedef boost::unique_lock<MutexType> LockType;
	typedef boost::condition_variable_any CondType;
	QueueType queue_;
	CondType cond_;
	MutexType mutex_;
};

//   member functions
template<class T>
void BlockQueue2<T>::push(BlockQueue2<T>::DataPtr data)
{
	{	
		LockType lk(mutex_);
		queue_.push_back(data);
	}
	cond_.notify_one();
}

template<class T>
typename BlockQueue2<T>::DataPtr BlockQueue2<T>::pop(int timeout)
{
	DataPtr ret;
	LockType lk(mutex_);
	while(queue_.size() <= 0)
	{
		if(boost::cv_status::timeout == cond_.wait_for(lk, boost::chrono::milliseconds(timeout)))
			return ret; // timeout
	}
	ret =  queue_.front();
	queue_.pop_front();
	return ret;
}

#endif
