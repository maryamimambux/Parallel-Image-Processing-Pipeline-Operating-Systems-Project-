#ifndef STAGES_H
#define STAGES_H




#include "CircularBuffer.h"
#include "ThreadPool.h"
#include <atomic>
#include <chrono>

struct StageContext {
    CircularBuffer* inputBuffer;
    CircularBuffer* outputBuffer;
    std::atomic<int>* completedCount;
    std::atomic<bool>* isRunning;
    int stageId;
    int totalImages;
};

// Filter types
enum FilterType {
    GRAYSCALE,
    BLUR,
    EDGE_DETECTION,
    BRIGHTNESS,
    CONTRAST
};

// Stage function declarations
void* loaderStage(void* arg);
void* filterStage(void* arg);
void* enhancerStage(void* arg);
void* saverStage(void* arg);

// Image processing functions
cv::Mat applyGrayscale(const cv::Mat& input);
cv::Mat applyBlur(const cv::Mat& input);
cv::Mat applyEdgeDetection(const cv::Mat& input);
cv::Mat adjustBrightness(const cv::Mat& input, int value = 30);
cv::Mat adjustContrast(const cv::Mat& input, double alpha = 1.2);

// Performance measurement
class PerformanceTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
public:
    void start();
    void stop();
    double getElapsedMilliseconds();
    double getElapsedSeconds();
};

// ... existing code ...

// Sequential processing function
void processSequential();

// PerformanceTimer class implementation (if not already there)
class PerformanceTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void stop() {
        endTime = std::chrono::high_resolution_clock::now();
    }
    
    double getElapsedMilliseconds() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    }
    
    double getElapsedSeconds() {
        return std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();
    }
};

#endif // STAGES_H
 