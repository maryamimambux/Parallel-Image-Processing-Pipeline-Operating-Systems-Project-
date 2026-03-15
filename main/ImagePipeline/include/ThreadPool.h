#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <vector>
#include <functional>

typedef void* (*ThreadFunction)(void*);

class ThreadPool {
private:
    std::vector<pthread_t> threads;
    int numThreads;
    bool isRunning;
    
public:
    ThreadPool(int numThreads);
    ~ThreadPool();
    
    bool createThreads(ThreadFunction func, void* arg);
    bool waitForCompletion();
    int getNumThreads() { return numThreads; }
    void stop();
};

#endif