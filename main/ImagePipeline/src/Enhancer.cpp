#include "../include/Stages.h"
#include <iostream>
#include <unistd.h>

cv::Mat adjustBrightness(const cv::Mat& input, int value) {
    cv::Mat output;
    input.convertTo(output, -1, 1.0, value);
    return output;
}

cv::Mat adjustContrast(const cv::Mat& input, double alpha) {
    cv::Mat output;
    input.convertTo(output, -1, alpha, 0);
    return output;
}

void* enhancerStage(void* arg) {
    StageContext* ctx = static_cast<StageContext*>(arg);
    
    std::cout << "Enhancer stage started" << std::endl;
    
    int enhancedCount = 0;
    bool running = true;
    
    while (running) {
        ImageTask task;
        
        if (ctx->inputBuffer->pop(task)) {
            // Check for termination signal
            if (task.sequenceNumber == -1 || !task.isValid) {
                ctx->outputBuffer->push(task);
                
                enhancedCount++;
                if (enhancedCount >= ctx->totalImages) {
                    running = false;
                }
                continue;
            }
            
            // Apply enhancements
            cv::Mat enhanced;
            
            // Adjust brightness and contrast
            enhanced = adjustBrightness(task.image, 30);  // Increase brightness
            enhanced = adjustContrast(enhanced, 1.2);     // Increase contrast
            
            std::cout << "Enhanced: " << task.filename << std::endl;
            
            // Create enhanced task
            ImageTask enhancedTask(enhanced, "enhanced_" + task.filename, task.sequenceNumber);
            
            ctx->outputBuffer->push(enhancedTask);
            enhancedCount++;
        } else {
            usleep(1000);
        }
    }
    
    std::cout << "Enhancer stage completed. Enhanced " << enhancedCount << " images" << std::endl;
    
    return nullptr;
}