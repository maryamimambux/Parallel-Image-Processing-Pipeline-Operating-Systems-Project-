

/*

The GUI is causing too much trouble. Let's skip Qt GUI completely — 
your project already works perfectly without it. 
The terminal output IS your demonstration. Let me simplify everything:

*/

#ifndef GUI_H
#define GUI_H
#endif

/*
#ifndef GUI_H
#define GUI_H

#ifdef GUI_ENABLED

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QTimer>
#include <QPushButton>
#include <QString>
#include <atomic>

// Forward declare pipeline counters
extern std::atomic<int> images_loaded;
extern std::atomic<int> images_filtered;
extern std::atomic<int> images_enhanced;
extern std::atomic<int> images_saved;
extern std::atomic<bool> pipeline_done;

// ─────────────────────────────────────────
// Main GUI Window
// Shows all 4 pipeline stages live
// ─────────────────────────────────────────
class PipelineWindow : public QWidget {
    Q_OBJECT

public:
    PipelineWindow(int total_images, QWidget* parent = nullptr);
    void log(const QString& message);

private slots:
    void update_display();  // called every 200ms by timer

private:
    int total;              // total images to process

    // Stage progress bars
    QProgressBar* bar_loader;
    QProgressBar* bar_filter;
    QProgressBar* bar_enhancer;
    QProgressBar* bar_saver;

    // Stage count labels
    QLabel* lbl_loader;
    QLabel* lbl_filter;
    QLabel* lbl_enhancer;
    QLabel* lbl_saver;

    // Status and log
    QLabel*    lbl_status;
    QTextEdit* log_box;
    QLabel*    lbl_time;

    // Timer for live updates
    QTimer* timer;

    // Track start time
    qint64 start_ms;

    // Helper to build one stage box
    QGroupBox* make_stage_box(
        const QString& title,
        const QString& color,
        QProgressBar*& bar,
        QLabel*& count_label
    );
};

// Runs the Qt GUI in its own thread
void launch_gui(int argc, char* argv[], int total_images);

#endif // GUI_ENABLED
#endif // GUI_H
*/
