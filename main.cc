#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include "SafeQueue.h"
#include "BlockQueue.h"
#include "BlockQueue2.h"

using namespace std;

const int kCount = 1000000;
const char *kMsg = "Hello World";

class Server1
{
public:
	void start()
	{
		run_ = true;
		thread_.reset(new boost::thread(boost::bind(&Server1::loop, this)));
	}
	void stop()
	{
		run_ = false;
		thread_->join();
	}
	void push(const char *msg)
	{
		queue_.push(QueueType::DataPtr(new string(msg)));
	}
private:	
	void loop()
	{
		while(run_)
		{
			sleep(2);
			struct timeval tvStart, tvEnd;
			int count = 0;
			QueueType::DataPtr data;
			gettimeofday(&tvStart, NULL);
			while(data = queue_.pop())
			{
				++count;
			}
			if(count > 0)
			{
				gettimeofday(&tvEnd, NULL);
				int timespan = (tvEnd.tv_sec-tvStart.tv_sec)*1000+(tvEnd.tv_usec-tvStart.tv_usec)/1000;
				printf("server1, num/time : %d/%d, speed=%d/ms\n", count, timespan, count/timespan);
				break;
			}
		}
	}
	typedef SafeQueue<string> QueueType;
	bool run_;
	boost::shared_ptr<boost::thread> thread_;
	QueueType queue_;
};

class Server2
{
public:
	void start()
	{
		run_ = true;
		thread_.reset(new boost::thread(boost::bind(&Server2::loop, this)));
	}
	void stop()
	{
		run_ = false;
		thread_->join();// must join to ensure this thread exit before main thread
	}
	void push(const char *msg)
	{
		queue_.push(QueueType::DataPtr(new string(msg)));
	}
private:	
	void loop()
	{
		struct timeval tvStart, tvEnd;
		gettimeofday(&tvStart, NULL);
		int count = 0;
		QueueType::DataPtr data;
		while(run_ || (data = queue_.pop(1000))) // ensure all data processed when stopped
		{
			if(data)
				count++;
		}
		if(count > 0)
		{
			gettimeofday(&tvEnd, NULL);
			int timespan = (tvEnd.tv_sec-tvStart.tv_sec)*1000+(tvEnd.tv_usec-tvStart.tv_usec)/1000;
			printf("server2, num/time : %d/%d, speed=%d/ms\n", count, timespan, count/timespan);
		}
	}
	typedef BlockQueue<string> QueueType;
	//typedef BlockQueue2<string> QueueType;
	bool run_;
	boost::shared_ptr<boost::thread> thread_;
	QueueType queue_;
};

int main()
{
	// s1
	Server1 s1;
	s1.start();
	for(int i = 0; i < kCount; i++)
	{
		s1.push(kMsg);
	}
	s1.stop();

	// s2
	Server2 s2;
	s2.start();
	for(int i = 0; i < kCount; i++)
	{
		s2.push(kMsg);
	}
	s2.stop();
	return 0;
}

