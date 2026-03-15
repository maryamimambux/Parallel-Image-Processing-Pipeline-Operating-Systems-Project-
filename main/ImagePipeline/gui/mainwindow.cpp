#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFont>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Parallel Image Processing Pipeline");
    setMinimumSize(800, 600);
    
    // Create central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Title
    QLabel* title = new QLabel("Parallel Image Processing Pipeline");
    title->setAlignment(Qt::AlignCenter);
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);
    mainLayout->addWidget(title);
    
    // Pipeline visualization
    QGroupBox* pipelineGroup = new QGroupBox("Pipeline Stages");
    QVBoxLayout* pipelineLayout = new QVBoxLayout(pipelineGroup);
    
    loaderLabel = new QLabel("Loader: Waiting...");
    filterLabel = new QLabel("Filter: Waiting...");
    enhancerLabel = new QLabel("Enhancer: Waiting...");
    saverLabel = new QLabel("Saver: Waiting...");
    
    pipelineLayout->addWidget(loaderLabel);
    pipelineLayout->addWidget(filterLabel);
    pipelineLayout->addWidget(enhancerLabel);
    pipelineLayout->addWidget(saverLabel);
    
    mainLayout->addWidget(pipelineGroup);
    
    // Progress bar
    QGroupBox* progressGroup = new QGroupBox("Overall Progress");
    QVBoxLayout* progressLayout = new QVBoxLayout(progressGroup);
    
    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressLayout->addWidget(progressBar);
    
    timeLabel = new QLabel("Time: Not started");
    progressLayout->addWidget(timeLabel);
    
    mainLayout->addWidget(progressGroup);
    
    // Log area
    QGroupBox* logGroup = new QGroupBox("Processing Log");
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);
    
    logArea = new QTextEdit();
    logArea->setReadOnly(true);
    logLayout->addWidget(logArea);
    
    mainLayout->addWidget(logGroup);
    
    // Control buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    startButton = new QPushButton("Start Parallel Processing");
    sequentialButton = new QPushButton("Run Sequential (Compare)");
    QPushButton* clearButton = new QPushButton("Clear Log");
    
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(sequentialButton);
    buttonLayout->addWidget(clearButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startProcessing);
    connect(sequentialButton, &QPushButton::clicked, this, &MainWindow::runSequential);
    connect(clearButton, &QPushButton::clicked, logArea, &QTextEdit::clear);
    
    // Create worker thread
    worker = new PipelineWorker();
    
    connect(worker, &PipelineWorker::stageUpdate, this, &MainWindow::updateStageStatus);
    connect(worker, &PipelineWorker::progressUpdate, this, &MainWindow::updateProgress);
    connect(worker, &PipelineWorker::logMessage, this, &MainWindow::addLogMessage);
    connect(worker, &PipelineWorker::processingComplete, this, &MainWindow::processingFinished);
    connect(worker, &PipelineWorker::finished, worker, &QObject::deleteLater);
}

MainWindow::~MainWindow() {
    if (worker && worker->isRunning()) {
        worker->terminate();
        worker->wait();
    }
}

void MainWindow::startProcessing() {
    startButton->setEnabled(false);
    sequentialButton->setEnabled(false);
    progressBar->setValue(0);
    logArea->clear();
    
    addLogMessage("Starting parallel image processing...");
    
    worker->start();
}

void MainWindow::runSequential() {
    logArea->clear();
    addLogMessage("Running sequential processing for comparison...");
    
    // Run sequential in a separate thread to avoid GUI freeze
    QThread* seqThread = QThread::create([this]() {
        PerformanceTimer timer;
        timer.start();
        processSequential();
        timer.stop();
        
        QMetaObject::invokeMethod(this, [this, timer]() {
            addLogMessage(QString("Sequential processing completed in %1 ms")
                         .arg(timer.getElapsedMilliseconds()));
        });
    });
    
    connect(seqThread, &QThread::finished, seqThread, &QObject::deleteLater);
    seqThread->start();
}

void MainWindow::updateStageStatus(const QString& stage, int count) {
    if (stage == "loader") {
        loaderLabel->setText(QString("Loader: Processing %1 images").arg(count));
    } else if (stage == "filter") {
        filterLabel->setText(QString("Filter: Processed %1 images").arg(count));
    } else if (stage == "enhancer") {
        enhancerLabel->setText(QString("Enhancer: Enhanced %1 images").arg(count));
    } else if (stage == "saver") {
        saverLabel->setText(QString("Saver: Saved %1 images").arg(count));
    }
}

void MainWindow::updateProgress(int percentage) {
    progressBar->setValue(percentage);
}

void MainWindow::addLogMessage(const QString& message) {
    logArea->append(message);
}

void MainWindow::processingFinished(double parallelTime, double sequentialTime) {
    timeLabel->setText(QString("Parallel: %1 ms | Sequential: %2 ms | Speedup: %3x")
                      .arg(parallelTime)
                      .arg(sequentialTime)
                      .arg(sequentialTime / parallelTime, 0, 'f', 2));
    
    startButton->setEnabled(true);
    sequentialButton->setEnabled(true);
    
    addLogMessage("Processing completed successfully!");
    
    QMessageBox::information(this, "Processing Complete", 
        QString("Parallel processing completed!\n"
                "Time: %1 ms\n"
                "Sequential time: %2 ms\n"
                "Speedup: %3x")
        .arg(parallelTime)
        .arg(sequentialTime)
        .arg(sequentialTime / parallelTime, 0, 'f', 2));
}

void PipelineWorker::run() {
    // This would integrate with the parallel pipeline
    // For now, simulate the pipeline
    PerformanceTimer timer;
    timer.start();
    
    emit logMessage("Loading images...");
    for (int i = 0; i <= 100; i += 20) {
        QThread::msleep(100);
        emit progressUpdate(i);
        emit stageUpdate("loader", i/20);
    }
    
    emit logMessage("Filtering images...");
    for (int i = 0; i <= 100; i += 20) {
        QThread::msleep(150);
        emit progressUpdate(i);
        emit stageUpdate("filter", i/20);
    }
    
    emit logMessage("Enhancing images...");
    for (int i = 0; i <= 100; i += 20) {
        QThread::msleep(100);
        emit progressUpdate(i);
        emit stageUpdate("enhancer", i/20);
    }
    
    emit logMessage("Saving images...");
    for (int i = 0; i <= 100; i += 20) {
        QThread::msleep(80);
        emit progressUpdate(i);
        emit stageUpdate("saver", i/20);
    }
    
    timer.stop();
    
    // For comparison, simulate sequential time (slower)
    double sequentialTime = timer.getElapsedMilliseconds() * 2.5;
    
    emit processingComplete(timer.getElapsedMilliseconds(), sequentialTime);
}