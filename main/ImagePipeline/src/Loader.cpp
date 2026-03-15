#include "../include/Stages.h"
#include <iostream>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <unistd.h>

void* loaderStage(void* arg) {
    StageContext* ctx = static_cast<StageContext*>(arg);
    
    std::cout << "Loader stage started with " << ctx->totalImages << " images" << std::endl;
    
    // Read images from input folder
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
    
    // Sort files to maintain order
    std::sort(imageFiles.begin(), imageFiles.end());
    
    std::cout << "Found " << imageFiles.size() << " images to process" << std::endl;
    
    // Load and push each image
    int loadedCount = 0;
    for (const auto& filename : imageFiles) {
        std::string fullPath = "./input/" + filename;
        cv::Mat img = cv::imread(fullPath);
        
        if (img.empty()) {
            std::cerr << "Failed to load image: " << filename << std::endl;
            continue;
        }
        
        std::cout << "Loaded: " << filename << " (" << img.cols << "x" << img.rows << ")" << std::endl;
        
        ImageTask task(img, filename, loadedCount);
        
        // Push to buffer with retry mechanism
        int retries = 0;
        while (!ctx->outputBuffer->push(task) && retries < 10) {
            usleep(1000); // Wait 1ms and retry (ERROR: without #include <unistd.h>)
             retries++;
        }
        
        if (retries >= 10) {
            std::cerr << "Failed to push image to buffer after retries: " << filename << std::endl;
        } else {
            loadedCount++;
        }
    }
    
    // Send termination signals (empty images with sequence number -1)
    for (int i = 0; i < ctx->totalImages; i++) {
        ImageTask termSignal;
        termSignal.isValid = false;
        termSignal.sequenceNumber = -1;
        ctx->outputBuffer->push(termSignal);
    }
    
    ctx->completedCount->store(loadedCount);
    std::cout << "Loader completed. Loaded " << loadedCount << " images" << std::endl;
    
    return nullptr;
}