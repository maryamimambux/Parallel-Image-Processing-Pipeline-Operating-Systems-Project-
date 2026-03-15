#include "../include/ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(int numThreads) : numThreads(numThreads), isRunning(false) {
    threads.resize(numThreads);
}

ThreadPool::~ThreadPool() {
    stop();
}

bool ThreadPool::createThreads(ThreadFunction func, void* arg) {
    if (isRunning) {
        std::cerr << "Thread pool is already running" << std::endl;
        return false;
    }
    
    isRunning = true;
    
    for (int i = 0; i < numThreads; i++) {
        if (pthread_create(&threads[i], nullptr, func, arg) != 0) {
            std::cerr << "Failed to create thread " << i << std::endl;
            isRunning = false;
            return false;
        }
        std::cout << "Created thread " << i << " in pool" << std::endl;
    }
    
    return true;
}

bool ThreadPool::waitForCompletion() {
    bool success = true;
    
    for (int i = 0; i < numThreads; i++) {
        if (pthread_join(threads[i], nullptr) != 0) {
            std::cerr << "Failed to join thread " << i << std::endl;
            success = false;
        }
    }
    
    isRunning = false;
    return success;
}

void ThreadPool::stop() {
    if (isRunning) {
        // Cancel threads if needed (implementation depends on your needs)
        isRunning = false;
    }
}