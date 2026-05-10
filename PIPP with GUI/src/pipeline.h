#ifndef PIPELINE_H
#define PIPELINE_H

#include "buffer.h"
#include <string>
#include <vector>
#include <atomic>

// Buffer capacity between each stage
#define BUFFER_SIZE 5

// Number of threads per stage
/* --- I THINK THIS IS GIVING ERROR
#define LOADER_THREADS   2
#define FILTER_THREADS   3
#define ENHANCER_THREADS 3
#define SAVER_THREADS    2
*/
#define LOADER_THREADS   1
#define FILTER_THREADS   1
#define ENHANCER_THREADS 1
#define SAVER_THREADS    1

// Shared buffers between stages
extern SharedBuffer loader_to_filter;    // Stage 1 → Stage 2
extern SharedBuffer filter_to_enhancer; // Stage 2 → Stage 3
extern SharedBuffer enhancer_to_saver;  // Stage 3 → Stage 4

// Shared counters (atomic = thread-safe without mutex)
extern std::atomic<int> images_loaded;
extern std::atomic<int> images_filtered;
extern std::atomic<int> images_enhanced;
extern std::atomic<int> images_saved;
extern std::atomic<bool> pipeline_done;

// Output folder path
extern std::string output_folder;

// Thread functions for each stage
void* loader_thread(void* arg);
void* filter_thread(void* arg);
void* enhancer_thread(void* arg);
void* saver_thread(void* arg);

// Main pipeline runner
void run_pipeline(std::vector<std::string>& image_files, std::string out_folder);

#endif
