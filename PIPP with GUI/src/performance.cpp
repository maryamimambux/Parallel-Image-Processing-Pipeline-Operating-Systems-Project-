#include "performance.h"
#include <opencv2/opencv.hpp>

// ─────────────────────────────────────────
// SEQUENTIAL MODE
// Processes images one by one, no threads
// Used to compare against parallel pipeline
// ─────────────────────────────────────────
BenchmarkResult run_sequential(
    std::vector<std::string>& image_files,
    std::string output_folder)
{
    std::cout << "\n[Sequential] Starting sequential processing...\n";

    auto start = std::chrono::high_resolution_clock::now();

    int processed = 0;

    for (int i = 0; i < (int)image_files.size(); i++) {

        // Load
        cv::Mat img = cv::imread(image_files[i]);
        if (img.empty()) {
            std::cout << "[Sequential] Skipping invalid image: "
                      << image_files[i] << "\n";
            continue;
        }

        // Filter
        img = apply_grayscale(img);

        // Enhance
        img = apply_brightness(img, 1.2, 10);

        // Save
        std::string filename = image_files[i].substr(
            image_files[i].find_last_of("/\\") + 1
        );
        std::string out_path = output_folder + "/seq_" + filename;
        cv::imwrite(out_path, img);

        std::cout << "[Sequential] Processed image " << i
                  << " → " << filename << "\n";
        processed++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();

    std::cout << "[Sequential] Done. Time: " << elapsed << "s\n";

    BenchmarkResult result;
    result.sequential_time = elapsed;
    result.parallel_time   = 0; // filled later
    result.image_count     = processed;
    result.speedup         = 0; // filled later
    return result;
}

// ─────────────────────────────────────────
// PRINT COMPARISON TABLE
// Shows sequential vs parallel side by side
// ─────────────────────────────────────────
void print_comparison(BenchmarkResult& result) {
    double speedup = result.sequential_time / result.parallel_time;
    result.speedup = speedup;

    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║     PERFORMANCE COMPARISON RESULTS       ║\n";
    std::cout << "╠══════════════════════════════════════════╣\n";
    std::cout << "║  Images Processed : "
              << std::setw(5) << result.image_count
              << "                   ║\n";
    std::cout << "╠══════════════════════════════════════════╣\n";
    std::cout << "║  Sequential Time  : "
              << std::setw(7) << std::fixed << std::setprecision(4)
              << result.sequential_time << "s                ║\n";
    std::cout << "║  Parallel Time    : "
              << std::setw(7) << std::fixed << std::setprecision(4)
              << result.parallel_time   << "s                ║\n";
    std::cout << "╠══════════════════════════════════════════╣\n";
    std::cout << "║  Speedup          : "
              << std::setw(6) << std::fixed << std::setprecision(2)
              << speedup << "x faster              ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";
}
