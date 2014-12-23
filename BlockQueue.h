#ifndef __BLOCKQUEUE_H_
#define __BLOCKQUEUE_H_

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <deque>

// template class
// data queue, thread safe
template<class T>
class BlockQueue
{
public:
	typedef boost::shared_ptr<T> DataPtr;
	void push(DataPtr data);
	DataPtr pop(int timeout);
private:
	typedef std::deque<DataPtr> QueueType;
	typedef boost::mutex MutexType;
	typedef boost::unique_lock<MutexType> LockType;
	typedef boost::condition_variable CondType;
	QueueType queue_;
	MutexType mutex_;
	CondType cond_;
};

//   member functions
template<class T>
void BlockQueue<T>::push(BlockQueue<T>::DataPtr data)
{
	{	
		LockType lk(mutex_);
		queue_.push_back(data);
	}
	cond_.notify_one();
}

template<class T>
typename BlockQueue<T>::DataPtr BlockQueue<T>::pop(int timeout)
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
