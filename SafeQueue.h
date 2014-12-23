#ifndef __SAFEQUEUE_H
#define __SAFEQUEUE_H

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <deque>

// template class
// data queue, thread safe
template<class T>
class SafeQueue
{
public:
	typedef boost::shared_ptr<T> DataPtr;
	void push(DataPtr data);
	DataPtr pop();
	DataPtr bpop();
private:
	typedef std::deque<DataPtr> QueueType;
	typedef boost::mutex MutexType;
	typedef boost::lock_guard<MutexType> LockType;
	QueueType queue_;
	MutexType mutex_;
};

//   member functions
template<class T>
void SafeQueue<T>::push(SafeQueue<T>::DataPtr data)
{
	LockType lk(mutex_);
	queue_.push_back(data);
}

template<class T>
typename SafeQueue<T>::DataPtr SafeQueue<T>::pop()
{
	DataPtr ret;
	LockType lk(mutex_);
	if(queue_.size() > 0)
	{
		ret =  queue_.front();
		queue_.pop_front();
	}
	return ret;
}


#endif

