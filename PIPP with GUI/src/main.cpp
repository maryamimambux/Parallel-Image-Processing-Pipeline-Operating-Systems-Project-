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
    BenchmarkResult* result;  // filled by pipeline thread
};

void* run_pipeline_thread(void* arg) {
    auto* args = (PipelineArgs*)arg;

    auto start = std::chrono::high_resolution_clock::now();
    run_pipeline(*args->files, args->output_folder);
    auto end = std::chrono::high_resolution_clock::now();

    if (args->result)
        args->result->parallel_time =
            std::chrono::duration<double>(end - start).count();

    return nullptr;
}

int main(int argc, char* argv[]) {

    std::string input_folder  =
        "/home/" + std::string(getenv("USER")) + "/OS_Project/input_images";
    std::string output_folder =
        "/home/" + std::string(getenv("USER")) + "/OS_Project/output_images";

    // ── Check for --benchmark flag ──
    bool benchmark_mode = false;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--benchmark") {
            benchmark_mode = true;
        }
    }

    std::cout << "========================================\n";
    std::cout << "   Parallel Image Processing Pipeline   \n";
    std::cout << "   OS Course Project — Spring 2026      \n";
    std::cout << "========================================\n\n";
    std::cout << "Input  folder : " << input_folder  << "\n";
    std::cout << "Output folder : " << output_folder << "\n\n";

    // ── Collect image files ──
    std::vector<std::string> image_files;
    for (const auto& entry : fs::directory_iterator(input_folder)) {
        std::string ext = entry.path().extension().string();
        if (ext == ".jpg" || ext == ".jpeg" ||
            ext == ".png" || ext == ".bmp") {
            image_files.push_back(entry.path().string());
        }
    }

    if (image_files.empty()) {
        std::cout << "ERROR: No images found in " << input_folder << "\n";
        return 1;
    }

    std::cout << "Found " << image_files.size() << " image(s) to process.\n\n";

    // ── BENCHMARK MODE ──
    if (benchmark_mode) {
        std::cout << "Running in BENCHMARK mode...\n";

        // Step 1: Sequential
        std::cout << "Step 1: Sequential processing\n";
        BenchmarkResult result = run_sequential(image_files, output_folder);

        // Step 2: Parallel pipeline
        std::cout << "\nStep 2: Parallel pipeline processing\n";

#ifdef GUI_ENABLED
        QApplication app(argc, argv);
        PipelineWindow window(image_files.size());
        window.show();

        PipelineArgs args = { &image_files, output_folder, &result };
        pthread_t pipeline_thread;
        pthread_create(&pipeline_thread, nullptr, run_pipeline_thread, &args);

        app.exec();
        pthread_join(pipeline_thread, nullptr);
#else
        auto start = std::chrono::high_resolution_clock::now();
        run_pipeline(image_files, output_folder);
        auto end = std::chrono::high_resolution_clock::now();
        result.parallel_time =
            std::chrono::duration<double>(end - start).count();
#endif

        // Print comparison table
        print_comparison(result);
        return 0;
    }

    // ── NORMAL MODE ──
#ifdef GUI_ENABLED
    QApplication app(argc, argv);

    PipelineWindow window(image_files.size());
    window.show();

    BenchmarkResult dummy;
    PipelineArgs args = { &image_files, output_folder, &dummy };
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

 
