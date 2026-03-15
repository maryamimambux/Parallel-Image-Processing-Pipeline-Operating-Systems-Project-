#include "../include/CircularBuffer.h"
#include <iostream>
#include <cstring>

CircularBuffer::CircularBuffer(int size) : capacity(size), front(0), rear(0), count(0) {
    buffer.resize(size);
    
    pthread_mutex_init(&mutex, nullptr);
    sem_init(&empty, 0, size);  // Initially 'empty' slots = capacity
    sem_init(&full, 0, 0);       // Initially no 'full' slots
}

CircularBuffer::~CircularBuffer() {
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
}

bool CircularBuffer::push(const ImageTask& task) {
    sem_wait(&empty);  // Wait if no empty slots
    
    pthread_mutex_lock(&mutex);
    
    // Check if buffer is full (should not happen due to semaphore)
    if (count >= capacity) {
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);  // Release the semaphore we took
        return false;
    }
    
    buffer[rear] = task;
    rear = (rear + 1) % capacity;
    count++;
    
    pthread_mutex_unlock(&mutex);
    sem_post(&full);  // Signal that a slot is now full
    
    return true;
}

bool CircularBuffer::pop(ImageTask& task) {
    sem_wait(&full);  // Wait if no full slots
    
    pthread_mutex_lock(&mutex);
    
    // Check if buffer is empty (should not happen due to semaphore)
    if (count <= 0) {
        pthread_mutex_unlock(&mutex);
        sem_post(&full);  // Release the semaphore we took
        return false;
    }
    
    task = buffer[front];
    front = (front + 1) % capacity;
    count--;
    
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);  // Signal that a slot is now empty
    
    return true;
}

bool CircularBuffer::isEmpty() {
    pthread_mutex_lock(&mutex);
    bool empty = (count == 0);
    pthread_mutex_unlock(&mutex);
    return empty;
}

bool CircularBuffer::isFull() {
    pthread_mutex_lock(&mutex);
    bool full = (count >= capacity);
    pthread_mutex_unlock(&mutex);
    return full;
}

int CircularBuffer::getCount() {
    pthread_mutex_lock(&mutex);
    int currentCount = count;
    pthread_mutex_unlock(&mutex);
    return currentCount;
}

void CircularBuffer::printStatus(const char* bufferName) {
    pthread_mutex_lock(&mutex);
    std::cout << bufferName << " - Items: " << count << "/" << capacity 
              << " [Front: " << front << ", Rear: " << rear << "]" << std::endl;
    pthread_mutex_unlock(&mutex);
}