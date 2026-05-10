#include "pipeline.h"
#include "filters.h"
#include <iostream>
#include <chrono>

// Define the shared buffers
SharedBuffer loader_to_filter;
SharedBuffer filter_to_enhancer;
SharedBuffer enhancer_to_saver;

// Define shared counters
std::atomic<int> images_loaded(0);
std::atomic<int> images_filtered(0);
std::atomic<int> images_enhanced(0);
std::atomic<int> images_saved(0);
std::atomic<bool> pipeline_done(false);

std::string output_folder;

// ─────────────────────────────────────────
// STAGE 1: LOADER
// Reads images from disk → puts into buffer
// ─────────────────────────────────────────
void* loader_thread(void* arg) {
    auto* files = (std::vector<std::string>*)arg;

    for (int i = 0; i < (int)files->size(); i++) {
        std::string filepath = (*files)[i];

        // Load the image from disk
        cv::Mat img = cv::imread(filepath);

        if (img.empty()) {
            std::cout << "[Loader] ERROR: Could not load " << filepath << "\n";
            continue; // skip corrupted/invalid images
        }

        ImageTask task;
        task.id       = i;
        task.filename = filepath.substr(filepath.find_last_of("/\\") + 1);
        task.image    = img;
        task.is_done  = false;

        std::cout << "[Loader] Loaded image " << task.id
                  << " → " << task.filename << "\n";

        images_loaded++;
        loader_to_filter.push(task); // send to next stage
    }

    // Send one "done" signal per filter thread
    for (int i = 0; i < FILTER_THREADS; i++) {
        ImageTask done_task;
        done_task.is_done = true;
        loader_to_filter.push(done_task);
    }

    return nullptr;
}

// ─────────────────────────────────────────
// STAGE 2: FILTER
// Applies image filters → passes forward
// ─────────────────────────────────────────
void* filter_thread(void* arg) {
    while (true) {
        ImageTask task = loader_to_filter.pop();

        if (task.is_done) {
            // Forward done signal to next stage
            filter_to_enhancer.push(task);
            break;
        }

        std::cout << "[Filter] Filtering image " << task.id
                  << " → " << task.filename << "\n";

        // Apply grayscale filter (you can change this)
        task.image = apply_grayscale(task.image);

        images_filtered++;
        filter_to_enhancer.push(task);
    }

    return nullptr;
}

// ─────────────────────────────────────────
// STAGE 3: ENHANCER
// Enhances image (brightness/contrast) → passes forward
// ─────────────────────────────────────────
void* enhancer_thread(void* arg) {
    while (true) {
        ImageTask task = filter_to_enhancer.pop();

        if (task.is_done) {
            enhancer_to_saver.push(task);
            break;
        }

        std::cout << "[Enhancer] Enhancing image " << task.id
                  << " → " << task.filename << "\n";

        // Apply brightness enhancement
        task.image = apply_brightness(task.image, 1.2, 10);

        images_enhanced++;
        enhancer_to_saver.push(task);
    }

    return nullptr;
}

// ─────────────────────────────────────────
// STAGE 4: SAVER
// Saves final processed image to output folder
// ─────────────────────────────────────────
void* saver_thread(void* arg) {
    while (true) {
        ImageTask task = enhancer_to_saver.pop();

        if (task.is_done) break;

        std::string out_path = output_folder + "/processed_" + task.filename;
        bool success = cv::imwrite(out_path, task.image);

        if (success) {
            std::cout << "[Saver] Saved image " << task.id
                      << " → " << out_path << "\n";
            images_saved++;
        } else {
            std::cout << "[Saver] ERROR saving image " << task.id << "\n";
        }
    }

    return nullptr;
}

// ─────────────────────────────────────────
// MAIN PIPELINE RUNNER
// Creates all threads and waits for them
// ─────────────────────────────────────────
void run_pipeline(std::vector<std::string>& image_files, std::string out_folder) {
    output_folder = out_folder;

    // Initialize all buffers
    loader_to_filter.init(BUFFER_SIZE);
    filter_to_enhancer.init(BUFFER_SIZE);
    enhancer_to_saver.init(BUFFER_SIZE);

    // Create thread arrays
    pthread_t loader_threads[LOADER_THREADS];
    pthread_t filter_threads[FILTER_THREADS];
    pthread_t enhancer_threads[ENHANCER_THREADS];
    pthread_t saver_threads[SAVER_THREADS];

    auto start_time = std::chrono::high_resolution_clock::now();

    // Start Stage 1 — Loader (only 1 loader needed)
    pthread_create(&loader_threads[0], nullptr, loader_thread, &image_files);

    // Start Stage 2 — Filter threads
    for (int i = 0; i < FILTER_THREADS; i++)
        pthread_create(&filter_threads[i], nullptr, filter_thread, nullptr);

    // Start Stage 3 — Enhancer threads
    for (int i = 0; i < ENHANCER_THREADS; i++)
        pthread_create(&enhancer_threads[i], nullptr, enhancer_thread, nullptr);

    // Start Stage 4 — Saver threads
    for (int i = 0; i < SAVER_THREADS; i++)
        pthread_create(&saver_threads[i], nullptr, saver_thread, nullptr);

    // Wait for all threads to finish (join)
    pthread_join(loader_threads[0], nullptr);

    for (int i = 0; i < FILTER_THREADS; i++)
        pthread_join(filter_threads[i], nullptr);

    for (int i = 0; i < ENHANCER_THREADS; i++)
        pthread_join(enhancer_threads[i], nullptr);

    for (int i = 0; i < SAVER_THREADS; i++)
        pthread_join(saver_threads[i], nullptr);

    auto end_time = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end_time - start_time).count();

    // Clean up buffers
    loader_to_filter.destroy();
    filter_to_enhancer.destroy();
    enhancer_to_saver.destroy();

    pipeline_done = true;

    // Print final summary
    std::cout << "\n========== PIPELINE COMPLETE ==========\n";
    std::cout << "Images Loaded:   " << images_loaded   << "\n";
    std::cout << "Images Filtered: " << images_filtered << "\n";
    std::cout << "Images Enhanced: " << images_enhanced << "\n";
    std::cout << "Images Saved:    " << images_saved    << "\n";
    std::cout << "Total Time:      " << elapsed << " seconds\n";
    std::cout << "=======================================\n";
}
