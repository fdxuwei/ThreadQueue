CXX = g++
CPPFLAGS = -g -I/home/xuwei/Project/muduo-master/
LDFLAGS =  
LDLIBS = -lboost_chrono -lboost_thread -lmuduo_net -lmuduo_base  -pthread
EXEC = main

all:  $(EXEC)


clean:
	rm -f ${EXEC} *.o 
