#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <pthread.h>
#include "pipeline.h"
#include "performance.h"

#ifdef GUI_ENABLED
#include "gui.h"
#include <QApplication>
#endif

namespace fs = std::filesystem;

struct PipelineArgs {
    std::vector<std::string>* files;
    std::string output_folder;
};

void* run_pipeline_thread(void* arg) {
    auto* args = (PipelineArgs*)arg;
    run_pipeline(*args->files, args->output_folder);
    return nullptr;
}

// Collect images from a folder
std::vector<std::string> collect_images(const std::string& folder) {
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(folder)) {
        std::string ext = entry.path().extension().string();
        if (ext == ".jpg" || ext == ".jpeg" ||
            ext == ".png" || ext == ".bmp") {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

int main(int argc, char* argv[]) {

    std::string user          = std::string(getenv("USER"));
    std::string input_folder  = "/home/" + user + "/OS_Project/input_images";
    std::string output_folder = "/home/" + user + "/OS_Project/output_images";

    // ── Check for --benchmark flag ──
    bool benchmark_mode = false;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--benchmark") {
            benchmark_mode = true;
        }
    }

    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║   Parallel Image Processing Pipeline     ║\n";
    std::cout << "║   OS Course Project — Spring 2026        ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";

    std::cout << "Input  folder : " << input_folder  << "\n";
    std::cout << "Output folder : " << output_folder << "\n\n";

    // Collect images
    std::vector<std::string> image_files = collect_images(input_folder);

    if (image_files.empty()) {
        std::cout << "ERROR: No images found in " << input_folder << "\n";
        std::cout << "Add .jpg or .png images to the input folder.\n";
        return 1;
    }

    std::cout << "Found " << image_files.size() << " image(s) to process.\n\n";

    // ── BENCHMARK MODE ──
    if (benchmark_mode) {
        std::cout << "Running in BENCHMARK mode...\n";
        std::cout << "Step 1: Sequential processing\n";

        // Run sequential first
        BenchmarkResult result = run_sequential(image_files, output_folder);

        std::cout << "\nStep 2: Parallel pipeline processing\n";

        // Reset counters
        images_loaded   = 0;
        images_filtered = 0;
        images_enhanced = 0;
        images_saved    = 0;
        pipeline_done   = false;

        // Time the parallel run
        auto par_start = std::chrono::high_resolution_clock::now();
        run_pipeline(image_files, output_folder);
        auto par_end = std::chrono::high_resolution_clock::now();

        result.parallel_time = std::chrono::duration<double>(
            par_end - par_start).count();

        // Print the comparison table
        print_comparison(result);
        return 0;
    }

    // ── NORMAL MODE (with or without GUI) ──
#ifdef GUI_ENABLED
    QApplication app(argc, argv);
    PipelineWindow window(image_files.size());
    window.show();

    PipelineArgs args = { &image_files, output_folder };
    pthread_t pipeline_thread;
    pthread_create(&pipeline_thread, nullptr, run_pipeline_thread, &args);

    int result = app.exec();
    pthread_join(pipeline_thread, nullptr);
    return result;

#else
    run_pipeline(image_files, output_folder);
    return 0;
#endif
}
