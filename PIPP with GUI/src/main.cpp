#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <pthread.h>
#include "pipeline.h"

#ifdef GUI_ENABLED
#include "gui.h"
#include <QApplication>
#endif

namespace fs = std::filesystem;

// Pipeline runs in a background thread
// so GUI can run on the main thread
struct PipelineArgs {
    std::vector<std::string>* files;
    std::string output_folder;
};

void* run_pipeline_thread(void* arg) {
    auto* args = (PipelineArgs*)arg;
    run_pipeline(*args->files, args->output_folder);
    return nullptr;
}

int main(int argc, char* argv[]) {

    std::string input_folder  =
        "/home/" + std::string(getenv("USER")) + "/OS_Project/input_images";
    std::string output_folder =
        "/home/" + std::string(getenv("USER")) + "/OS_Project/output_images";

    std::cout << "========================================\n";
    std::cout << "   Parallel Image Processing Pipeline   \n";
    std::cout << "========================================\n";

    // Collect all image files
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

#ifdef GUI_ENABLED
    // Launch Qt GUI on main thread
    QApplication app(argc, argv);

    PipelineWindow window(image_files.size());
    window.show();

    // Run pipeline in background thread
    PipelineArgs args = { &image_files, output_folder };
    pthread_t pipeline_thread;
    pthread_create(&pipeline_thread, nullptr, run_pipeline_thread, &args);

    // Qt event loop runs here (keeps window alive)
    int result = app.exec();

    pthread_join(pipeline_thread, nullptr);
    return result;

#else
    // No GUI — just run pipeline directly
    run_pipeline(image_files, output_folder);
    return 0;
#endif
}
