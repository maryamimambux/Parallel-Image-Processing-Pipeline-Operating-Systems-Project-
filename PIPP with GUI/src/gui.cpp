#include "gui.h"

#ifdef GUI_ENABLED

#include <QDateTime>
#include <QScrollBar>
#include <QFont>
#include <QThread>
#include <iostream>

// ─────────────────────────────────────────
// Constructor — builds the window layout
// ─────────────────────────────────────────
PipelineWindow::PipelineWindow(int total_images, QWidget* parent)
    : QWidget(parent), total(total_images)
{
    setWindowTitle("Parallel Image Processing Pipeline");
    setMinimumSize(700, 600);
    setStyleSheet("background-color: #1e1e2e; color: #cdd6f4;");

    // Record start time
    start_ms = QDateTime::currentMSecsSinceEpoch();

    // ── Title ──
    QLabel* title = new QLabel("Parallel Image Processing Pipeline");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 18px; font-weight: bold;"
        "color: #cba6f7; padding: 10px;"
    );

    // ── Status label ──
    lbl_status = new QLabel("Status: Running...");
    lbl_status->setAlignment(Qt::AlignCenter);
    lbl_status->setStyleSheet("font-size: 13px; color: #a6e3a1; padding: 4px;");

    // ── Timer label ──
    lbl_time = new QLabel("Time: 0.0s");
    lbl_time->setAlignment(Qt::AlignCenter);
    lbl_time->setStyleSheet("font-size: 12px; color: #fab387; padding: 2px;");

    // ── Four stage boxes ──
    QGroupBox* box_loader   = make_stage_box("Stage 1 — Loader",   "#89b4fa", bar_loader,   lbl_loader);
    QGroupBox* box_filter   = make_stage_box("Stage 2 — Filter",   "#a6e3a1", bar_filter,   lbl_filter);
    QGroupBox* box_enhancer = make_stage_box("Stage 3 — Enhancer", "#fab387", bar_enhancer, lbl_enhancer);
    QGroupBox* box_saver    = make_stage_box("Stage 4 — Saver",    "#cba6f7", bar_saver,    lbl_saver);

    // ── Log box ──
    QLabel* log_title = new QLabel("Live Log:");
    log_title->setStyleSheet("font-size: 12px; color: #cdd6f4; padding-top: 6px;");

    log_box = new QTextEdit();
    log_box->setReadOnly(true);
    log_box->setMaximumHeight(140);
    log_box->setStyleSheet(
        "background-color: #181825;"
        "color: #cdd6f4;"
        "font-family: monospace;"
        "font-size: 11px;"
        "border: 1px solid #45475a;"
        "border-radius: 6px;"
        "padding: 6px;"
    );

    // ── Main layout ──
    QVBoxLayout* main_layout = new QVBoxLayout();
    main_layout->setSpacing(8);
    main_layout->setContentsMargins(16, 12, 16, 12);

    main_layout->addWidget(title);
    main_layout->addWidget(lbl_status);
    main_layout->addWidget(lbl_time);
    main_layout->addSpacing(6);
    main_layout->addWidget(box_loader);
    main_layout->addWidget(box_filter);
    main_layout->addWidget(box_enhancer);
    main_layout->addWidget(box_saver);
    main_layout->addWidget(log_title);
    main_layout->addWidget(log_box);

    setLayout(main_layout);

    // ── Timer: refresh every 200ms ──
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PipelineWindow::update_display);
    timer->start(200);
}

// ─────────────────────────────────────────
// Builds one colored stage box with
// progress bar + count label inside
// ─────────────────────────────────────────
QGroupBox* PipelineWindow::make_stage_box(
    const QString& title,
    const QString& color,
    QProgressBar*& bar,
    QLabel*& count_label)
{
    QGroupBox* box = new QGroupBox(title);
    box->setStyleSheet(QString(
        "QGroupBox {"
        "  font-size: 13px; font-weight: bold;"
        "  color: %1;"
        "  border: 1px solid %1;"
        "  border-radius: 8px;"
        "  margin-top: 6px;"
        "  padding: 8px;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 10px;"
        "  padding: 0 4px;"
        "}"
    ).arg(color));

    bar = new QProgressBar();
    bar->setMinimum(0);
    bar->setMaximum(total);
    bar->setValue(0);
    bar->setTextVisible(true);
    bar->setStyleSheet(QString(
        "QProgressBar {"
        "  border: 1px solid #45475a;"
        "  border-radius: 5px;"
        "  background: #181825;"
        "  height: 22px;"
        "  text-align: center;"
        "  color: #cdd6f4;"
        "}"
        "QProgressBar::chunk {"
        "  background-color: %1;"
        "  border-radius: 4px;"
        "}"
    ).arg(color));

    count_label = new QLabel("0 / " + QString::number(total) + " images");
    count_label->setStyleSheet("font-size: 11px; color: #bac2de;");

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(bar);
    layout->addWidget(count_label);
    box->setLayout(layout);

    return box;
}

// ─────────────────────────────────────────
// Called every 200ms — reads atomic counters
// and updates all progress bars live
// ─────────────────────────────────────────
void PipelineWindow::update_display() {
    int loaded   = images_loaded.load();
    int filtered = images_filtered.load();
    int enhanced = images_enhanced.load();
    int saved    = images_saved.load();

    // Update progress bars
    bar_loader->setValue(loaded);
    bar_filter->setValue(filtered);
    bar_enhancer->setValue(enhanced);
    bar_saver->setValue(saved);

    // Update count labels
    lbl_loader->setText(  QString::number(loaded)   + " / " + QString::number(total) + " images");
    lbl_filter->setText(  QString::number(filtered) + " / " + QString::number(total) + " images");
    lbl_enhancer->setText(QString::number(enhanced) + " / " + QString::number(total) + " images");
    lbl_saver->setText(   QString::number(saved)    + " / " + QString::number(total) + " images");

    // Update elapsed time
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    double elapsed = (now - start_ms) / 1000.0;
    lbl_time->setText(QString("Time elapsed: %1s").arg(elapsed, 0, 'f', 1));

    // Check if pipeline finished
    if (pipeline_done.load()) {
        timer->stop();
        lbl_status->setText("Status: COMPLETE ✓");
        lbl_status->setStyleSheet(
            "font-size: 13px; color: #a6e3a1;"
            "font-weight: bold; padding: 4px;"
        );
        log("Pipeline finished successfully!");
    }
}

// ─────────────────────────────────────────
// Adds a timestamped message to the log box
// ─────────────────────────────────────────
void PipelineWindow::log(const QString& message) {
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    double elapsed = (now - start_ms) / 1000.0;
    QString line = QString("[%1s] %2")
                       .arg(elapsed, 0, 'f', 1)
                       .arg(message);
    log_box->append(line);

    // Auto scroll to bottom
    QScrollBar* sb = log_box->verticalScrollBar();
    sb->setValue(sb->maximum());
}

#endif // GUI_ENABLED
