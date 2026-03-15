QT += core gui widgets
CONFIG += c++11

TARGET = ImagePipelineGUI
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ../src/CircularBuffer.cpp \
    ../src/ThreadPool.cpp \
    ../src/Loader.cpp \
    ../src/Filter.cpp \
    ../src/Enhancer.cpp \
    ../src/Saver.cpp \
    ../src/Utils.cpp

HEADERS += \
    mainwindow.h \
    ../include/CircularBuffer.h \
    ../include/ThreadPool.h \
    ../include/ImageTask.h \
    ../include/Stages.h

INCLUDEPATH += ../include

LIBS += `pkg-config --libs opencv4` -lpthread
QMAKE_CXXFLAGS += `pkg-config --cflags opencv4`