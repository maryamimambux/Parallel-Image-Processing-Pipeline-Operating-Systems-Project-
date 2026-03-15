#include "../include/CircularBuffer.h"
#include "../include/ThreadPool.h"
#include "../include/Stages.h"
#include <iostream>
#include <atomic>
#include <cstring>
#include <dirent.h>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

// Function prototype for sequential processing
void processSequential();

int main() {
    std::cout << "\n=== Parallel Image Processing Pipeline ===\n" << std::endl;
    
    // Create directories if they don't exist
    system("mkdir -p input output");
    
    // Count images in input folder
    int imageCount = 0;
    DIR* dir;
    struct dirent* ent;
    
    if ((dir = opendir("./input")) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string filename = ent->d_name;
            if (filename.length() > 4 && 
                (filename.substr(filename.length() - 4) == ".jpg" ||
                 filename.substr(filename.length() - 4) == ".png" ||
                 filename.substr(filename.length() - 5) == ".jpeg")) {
                imageCount++;
            }
        }
        closedir(dir);
    }
    
    if (imageCount == 0) {
        std::cout << "No images found in input folder. Please add some images first." << std::endl;
        std::cout << "You can add .jpg or .png images to the 'input' folder." << std::endl;
        return 1;
    }
    
    std::cout << "Found " << imageCount << " images to process\n" << std::endl;
    
    // Create buffers with appropriate sizes
    const int BUFFER_SIZE = 10;
    CircularBuffer buffer1(BUFFER_SIZE);  // Loader -> Filter
    CircularBuffer buffer2(BUFFER_SIZE);  // Filter -> Enhancer
    CircularBuffer buffer3(BUFFER_SIZE);  // Enhancer -> Saver
    
    // Shared counters
    std::atomic<int> loaderCompleted(0);
    std::atomic<bool> isRunning(true);
    
    // Create stage contexts
    StageContext loaderCtx{nullptr, &buffer1, &loaderCompleted, &isRunning, 0, imageCount};
    StageContext filterCtx{&buffer1, &buffer2, nullptr, &isRunning, 1, imageCount};
    StageContext enhancerCtx{&buffer2, &buffer3, nullptr, &isRunning, 2, imageCount};
    StageContext saverCtx{&buffer3, nullptr, nullptr, &isRunning, 3, imageCount};
    
    // Create thread pools (2 threads per stage)
    ThreadPool loaderPool(2);
    ThreadPool filterPool(2);
    ThreadPool enhancerPool(2);
    ThreadPool saverPool(2);
    
    std::cout << "\n=== Starting Parallel Pipeline ===\n" << std::endl;
    
    // Start timing
    PerformanceTimer timer;
    timer.start();
    
    // Start all stages
    bool success = true;
    success &= loaderPool.createThreads(loaderStage, &loaderCtx);
    success &= filterPool.createThreads(filterStage, &filterCtx);
    success &= enhancerPool.createThreads(enhancerStage, &enhancerCtx);
    success &= saverPool.createThreads(saverStage, &saverCtx);
    
    if (!success) {
        std::cerr << "Failed to create threads" << std::endl;
        return 1;
    }
    
    // Wait for all stages to complete
    loaderPool.waitForCompletion();
    filterPool.waitForCompletion();
    enhancerPool.waitForCompletion();
    saverPool.waitForCompletion();
    
    // Stop timing
    timer.stop();
    
    std::cout << "\n=== Pipeline Statistics ===" << std::endl;
    std::cout << "Total images processed: " << imageCount << std::endl;
    std::cout << "Parallel processing time: " << timer.getElapsedMilliseconds() << " ms" << std::endl;
    std::cout << "Time per image: " << timer.getElapsedMilliseconds() / imageCount << " ms" << std::endl;
    
    // Optional: Run sequential for comparison
    std::cout << "\nDo you want to run sequential processing for comparison? (y/n): ";
    char choice;
    std::cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        processSequential();
    }
    
    std::cout << "\n=== Pipeline Completed Successfully ===" << std::endl;
    std::cout << "Processed images saved in 'output' folder" << std::endl;
    
    return 0;
}

// Sequential processing function implementation
void processSequential() {
    std::cout << "\n=== Running Sequential Processing ===" << std::endl;
    
    PerformanceTimer timer;
    timer.start();
    
    // Read all images
    std::vector<std::string> imageFiles;
    DIR* dir;
    struct dirent* ent;
    
    if ((dir = opendir("./input")) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string filename = ent->d_name;
            if (filename.length() > 4 && 
                (filename.substr(filename.length() - 4) == ".jpg" ||
                 filename.substr(filename.length() - 4) == ".png" ||
                 filename.substr(filename.length() - 5) == ".jpeg")) {
                imageFiles.push_back(filename);
            }
        }
        closedir(dir);
    }
    
    std::sort(imageFiles.begin(), imageFiles.end());
    
    if (imageFiles.empty()) {
        std::cout << "No images found for sequential processing." << std::endl;
        return;
    }
    
    std::cout << "Processing " << imageFiles.size() << " images sequentially..." << std::endl;
    
    // Process each image sequentially
    int processedCount = 0;
    for (const auto& filename : imageFiles) {
        std::string inputPath = "./input/" + filename;
        cv::Mat img = cv::imread(inputPath);
        
        if (img.empty()) {
            std::cerr << "Failed to load: " << filename << std::endl;
            continue;
        }
        
        // Apply all stages in sequence
        cv::Mat processed = applyGrayscale(img);
        processed = applyBlur(processed);
        processed = adjustBrightness(processed, 30);
        processed = adjustContrast(processed, 1.2);
        
        std::string outputPath = "./output/sequential_" + filename;
        if (cv::imwrite(outputPath, processed)) {
            processedCount++;
            std::cout << "Processed sequentially: " << filename << std::endl;
        } else {
            std::cerr << "Failed to save: " << filename << std::endl;
        }
    }
    
    timer.stop();
    std::cout << "\nSequential processing completed:" << std::endl;
    std::cout << "Processed: " << processedCount << " images" << std::endl;
    std::cout << "Time taken: " << timer.getElapsedMilliseconds() << " ms" << std::endl;
    std::cout << "Average per image: " << timer.getElapsedMilliseconds() / processedCount << " ms" << std::endl;
}