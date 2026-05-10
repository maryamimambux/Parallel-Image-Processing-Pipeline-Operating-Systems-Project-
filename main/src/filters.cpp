 #include "filters.h"
 #include <iostream>
 
 // ─────────────────────────────────────────
 // FILTER 1: GRAYSCALE
 // Turns color image into black & white
 // ─────────────────────────────────────────
 cv::Mat apply_grayscale(cv::Mat& img) {
     if (img.empty()) {
         std::cout << "[Filter] WARNING: Empty image passed to grayscale\n";
         return img;
     }
 
     cv::Mat result;
     cv::cvtColor(img, result, cv::COLOR_BGR2GRAY);
 
     // Convert back to 3 channels so all filters stay compatible
     cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
 
     return result;
 }
 
 // ─────────────────────────────────────────
 // FILTER 2: BLUR
 // Smooths the image (removes noise)
 // strength must be odd number: 3, 5, 7, 9...
 // ─────────────────────────────────────────
 cv::Mat apply_blur(cv::Mat& img, int strength) {
     if (img.empty()) {
         std::cout << "[Filter] WARNING: Empty image passed to blur\n";
         return img;
     }
 
     // Make sure strength is odd (OpenCV requirement)
     if (strength % 2 == 0) strength++;
 
     cv::Mat result;
     cv::GaussianBlur(img, result, cv::Size(strength, strength), 0);
 
     return result;
 }
 
 // ─────────────────────────────────────────
 // FILTER 3: EDGE DETECTION
 // Finds and highlights edges in the image
 // ─────────────────────────────────────────
 cv::Mat apply_edge_detection(cv::Mat& img) {
     if (img.empty()) {
         std::cout << "[Filter] WARNING: Empty image passed to edge detection\n";
         return img;
     }
 
     cv::Mat gray, edges, result;
 
     // Step 1: convert to grayscale for edge detection
     cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
 
     // Step 2: apply Canny edge detector
     // (80 and 150 are threshold values — tune if needed)
     cv::Canny(gray, edges, 80, 150);
 
     // Step 3: convert back to 3 channels
     cv::cvtColor(edges, result, cv::COLOR_GRAY2BGR);
 
     return result;
 }
 
 // ─────────────────────────────────────────
 // FILTER 4: BRIGHTNESS & CONTRAST
 // alpha → contrast multiplier (1.0 = no change)
 // beta  → brightness added to each pixel
 // ─────────────────────────────────────────
 cv::Mat apply_brightness(cv::Mat& img, double alpha, int beta) {
     if (img.empty()) {
         std::cout << "[Filter] WARNING: Empty image passed to brightness\n";
         return img;
     }
 
     cv::Mat result;
 
     // convertTo applies: result = img * alpha + beta
     img.convertTo(result, -1, alpha, beta);
 
     return result;
 }
 
 // ─────────────────────────────────────────
 // FILTER 5: SHARPEN
 // Makes edges crisper and image clearer
 // ─────────────────────────────────────────
 cv::Mat apply_sharpen(cv::Mat& img) {
     if (img.empty()) {
         std::cout << "[Filter] WARNING: Empty image passed to sharpen\n";
         return img;
     }
 
     float k[9] = {
          0, -1,  0,
         -1,  9, -1,
          0, -1,  0
     };
     cv::Mat kernel(3, 3, CV_32F, k);
 
     cv::Mat result;
     cv::filter2D(img, result, img.depth(), kernel);
 
     return result;
 }
 
 // ─────────────────────────────────────────
 // APPLY ALL FILTERS IN SEQUENCE
 // Used for performance comparison testing
 // ─────────────────────────────────────────
 cv::Mat apply_all_filters(cv::Mat& img) {
     cv::Mat result = img.clone();
 
     result = apply_blur(result, 3);         // smooth first
     result = apply_brightness(result, 1.2, 10); // brighten
     result = apply_sharpen(result);         // then sharpen
 
     return result;
 }
