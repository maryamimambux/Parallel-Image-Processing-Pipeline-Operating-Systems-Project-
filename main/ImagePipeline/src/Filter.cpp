#include "../include/Stages.h"
#include <iostream>
#include <unistd.h>

// Image processing function implementations
cv::Mat applyGrayscale(const cv::Mat& input) {
    cv::Mat output;
    cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
    cv::cvtColor(output, output, cv::COLOR_GRAY2BGR); // Convert back to 3-channel for consistency
    return output;
}

cv::Mat applyBlur(const cv::Mat& input) {
    cv::Mat output;
    cv::GaussianBlur(input, output, cv::Size(5, 5), 1.5);
    return output;
}

cv::Mat applyEdgeDetection(const cv::Mat& input) {
    cv::Mat gray, edges, output;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, edges, 50, 150);
    cv::cvtColor(edges, output, cv::COLOR_GRAY2BGR);
    return output;
}

void* filterStage(void* arg) {
    StageContext* ctx = static_cast<StageContext*>(arg);
    
    std::cout << "Filter stage started" << std::endl;
    
    int processedCount = 0;
    bool running = true;
    
    while (running) {
        ImageTask task;
        
        // Pop from input buffer
        if (ctx->inputBuffer->pop(task)) {
            // Check for termination signal
            if (task.sequenceNumber == -1 || !task.isValid) {
                // Pass termination signal to next stage
                ctx->outputBuffer->push(task);
                
                // Check if this is the last termination signal
                processedCount++;
                if (processedCount >= ctx->totalImages) {
                    running = false;
                }
                continue;
            }
            
            // Apply filters based on sequence number or other criteria
            cv::Mat filtered;
            
            // Apply multiple filters in sequence
            filtered = applyGrayscale(task.image);
            filtered = applyBlur(filtered);
            
            // Optionally apply edge detection to some images
            if (processedCount % 3 == 0) {
                filtered = applyEdgeDetection(filtered);
            }
            
            std::cout << "Filtered: " << task.filename << std::endl;
            
            // Create filtered task
            ImageTask filteredTask(filtered, "filtered_" + task.filename, task.sequenceNumber);
            
            // Push to next buffer
            ctx->outputBuffer->push(filteredTask);
            processedCount++;
        } else {
            usleep(1000); // Wait if buffer is empty
        }
    }
    
    std::cout << "Filter stage completed. Processed " << processedCount << " images" << std::endl;
    
    return nullptr;
}