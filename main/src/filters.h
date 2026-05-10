#ifndef FILTERS_H
#define FILTERS_H

#include <opencv2/opencv.hpp>

// ─────────────────────────────────────────
// All filter functions your pipeline uses
// Each takes an image, returns processed image
// ─────────────────────────────────────────

// Converts image to grayscale (black & white)
cv::Mat apply_grayscale(cv::Mat& img);

// Blurs the image (smoothing effect)
cv::Mat apply_blur(cv::Mat& img, int strength = 5);

// Detects edges in the image (outline effect)
cv::Mat apply_edge_detection(cv::Mat& img);

// Adjusts brightness and contrast
// alpha = contrast (1.0 = normal, 1.5 = more contrast)
// beta  = brightness (0 = normal, 50 = brighter)
cv::Mat apply_brightness(cv::Mat& img, double alpha = 1.2, int beta = 10);

// Sharpens the image
cv::Mat apply_sharpen(cv::Mat& img);

// Applies all filters one after another (full pipeline)
cv::Mat apply_all_filters(cv::Mat& img);

#endif
