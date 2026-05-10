#ifndef BUFFER_H
#define BUFFER_H

#include <opencv2/opencv.hpp>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <string>

// This holds one image as it moves through the pipeline
struct ImageTask {
    int id;                    // unique image ID
    std::string filename;      // original filename
    cv::Mat image;             // the actual image data
    bool is_done;              // signals pipeline to stop
};

// Shared buffer between two pipeline stages
// Protected by mutex + semaphores (producer-consumer pattern)
struct SharedBuffer {
    std::queue<ImageTask> queue;   // holds images waiting
    pthread_mutex_t mutex;         // only one thread touches queue at a time
    sem_t empty_slots;             // how many empty spots left
    sem_t filled_slots;            // how many images are waiting

    // Set up the buffer with a max capacity
    void init(int capacity) {
        pthread_mutex_init(&mutex, nullptr);
        sem_init(&empty_slots, 0, capacity);  // starts full of empty slots
        sem_init(&filled_slots, 0, 0);        // starts with 0 images
    }

    // Add an image (blocks if buffer is full)
    void push(ImageTask task) {
        sem_wait(&empty_slots);           // wait for an empty slot
        pthread_mutex_lock(&mutex);       // lock the buffer
        queue.push(task);                 // add image
        pthread_mutex_unlock(&mutex);     // unlock
        sem_post(&filled_slots);          // signal: one more image ready
    }

    // Take an image (blocks if buffer is empty)
    ImageTask pop() {
        sem_wait(&filled_slots);          // wait for an image
        pthread_mutex_lock(&mutex);       // lock the buffer
        ImageTask task = queue.front();   // get image
        queue.pop();                      // remove from queue
        pthread_mutex_unlock(&mutex);     // unlock
        sem_post(&empty_slots);           // signal: one more empty slot
        return task;
    }

    void destroy() {
        pthread_mutex_destroy(&mutex);
        sem_destroy(&empty_slots);
        sem_destroy(&filled_slots);
    }
};

#endif
