#include "../include/Stages.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <thread>
#include <chrono>

// Utility function to create directories if they don't exist
void createDirectoryIfNotExists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        #ifdef _WIN32
            mkdir(path.c_str());
        #else
            mkdir(path.c_str(), 0755);
        #endif
    }
}

// Function to count images in input folder
int countImagesInInputFolder() {
    DIR* dir;
    struct dirent* ent;
    int count = 0;
    
    if ((dir = opendir("./input")) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string filename = ent->d_name;
            if (filename.length() > 4 && 
                (filename.substr(filename.length() - 4) == ".jpg" ||
                 filename.substr(filename.length() - 4) == ".png" ||
                 filename.substr(filename.length() - 5) == ".jpeg")) {
                count++;
            }
        }
        closedir(dir);
    }
    
    return count;
}

// Note: processSequential() is NOT here - it's in main.cpp