#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include "ImageTask.h"

class CircularBuffer {
private:
    std::vector<ImageTask> buffer;
    int capacity;
    int front;
    int rear;
    int count;
    
    pthread_mutex_t mutex;
    sem_t empty;
    sem_t full;
    
public:
    CircularBuffer(int size);
    ~CircularBuffer();
    
    bool push(const ImageTask& task);
    bool pop(ImageTask& task);
    bool isEmpty();
    bool isFull();
    int getCount();
    void printStatus(const char* bufferName);
};

#endif