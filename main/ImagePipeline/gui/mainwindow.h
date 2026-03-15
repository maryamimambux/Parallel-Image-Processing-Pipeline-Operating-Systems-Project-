#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QThread>
#include <QMutex>
#include "../include/Stages.h"

class PipelineWorker : public QThread {
    Q_OBJECT
public:
    void run() override;
    
signals:
    void stageUpdate(const QString& stage, int count);
    void progressUpdate(int percentage);
    void logMessage(const QString& message);
    void processingComplete(double parallelTime, double sequentialTime);
};

class MainWindow : public QMainWindow {
    Q_OBJECT
    
private:
    // GUI Elements
    QProgressBar* progressBar;
    QLabel* loaderLabel;
    QLabel* filterLabel;
    QLabel* enhancerLabel;
    QLabel* saverLabel;
    QTextEdit* logArea;
    QPushButton* startButton;
    QPushButton* sequentialButton;
    QLabel* timeLabel;
    
    PipelineWorker* worker;
    
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
private slots:
    void startProcessing();
    void runSequential();
    void updateStageStatus(const QString& stage, int count);
    void updateProgress(int percentage);
    void addLogMessage(const QString& message);
    void processingFinished(double parallelTime, double sequentialTime);
};

#endif