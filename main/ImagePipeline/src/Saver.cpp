#include "../include/Stages.h"
#include <iostream>
#include <unistd.h>

void* saverStage(void* arg) {
    StageContext* ctx = static_cast<StageContext*>(arg);
    
    std::cout << "Saver stage started" << std::endl;
    
    int savedCount = 0;
    bool running = true;
    
    while (running) {
        ImageTask task;
        
        if (ctx->inputBuffer->pop(task)) {
            // Check for termination signal
            if (task.sequenceNumber == -1 || !task.isValid) {
                savedCount++;
                if (savedCount >= ctx->totalImages) {
                    running = false;
                }
                continue;
            }
            
            // Save the processed image
            std::string outputPath = "./output/" + task.filename;
            
            if (cv::imwrite(outputPath, task.image)) {
                std::cout << "Saved: " << task.filename << " to output folder" << std::endl;
                savedCount++;
            } else {
                std::cerr << "Failed to save: " << task.filename << std::endl;
            }
        } else {
            usleep(1000);
        }
    }
    
    std::cout << "Saver stage completed. Saved " << savedCount << " images" << std::endl;
    
    return nullptr;
}

// PerformanceTimer implementation
void PerformanceTimer::start() {
    startTime = std::chrono::high_resolution_clock::now();
}

void PerformanceTimer::stop() {
    endTime = std::chrono::high_resolution_clock::now();
}

double PerformanceTimer::getElapsedMilliseconds() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}

double PerformanceTimer::getElapsedSeconds() {
    return std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();
}