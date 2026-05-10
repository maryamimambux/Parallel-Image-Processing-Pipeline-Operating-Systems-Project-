#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>
#include "filters.h"

// Stores timing results
struct BenchmarkResult {
    double sequential_time;
    double parallel_time;
    int    image_count;
    double speedup;
};

// Run images one by one (no threads)
BenchmarkResult run_sequential(
    std::vector<std::string>& image_files,
    std::string output_folder
);

// Print the comparison table
void print_comparison(BenchmarkResult& result);

#endif
