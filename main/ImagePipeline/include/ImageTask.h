#ifndef IMAGETASK_H
#define IMAGETASK_H


#include <string>
#include <opencv2/opencv.hpp>

struct ImageTask {
    cv::Mat image;
    std::string filename;
    int sequenceNumber;
    bool isValid;
    
    ImageTask() : sequenceNumber(-1), isValid(false) {}
    ImageTask(const cv::Mat& img, const std::string& fname, int seqNum) 
        : image(img), filename(fname), sequenceNumber(seqNum), isValid(true) {}
};

#endif