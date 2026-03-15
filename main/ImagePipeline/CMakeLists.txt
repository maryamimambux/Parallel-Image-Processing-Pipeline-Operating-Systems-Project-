cmake_minimum_required(VERSION 3.10)
project(ImageProcessingPipeline)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# For Qt GUI (optional - comment out if you don't need GUI)
# pkg_check_modules(QT5 REQUIRED Qt5Widgets Qt5Core Qt5Gui)

# Include directories
include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS})

# Collect all source files
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(pipeline ${SOURCES})

# Link libraries
target_link_libraries(pipeline 
    ${OpenCV_LIBS}
    Threads::Threads
)

# For GUI version (uncomment if you want to build GUI separately)
# add_executable(pipeline_gui gui/main.cpp gui/mainwindow.cpp ${SOURCES})
# target_link_libraries(pipeline_gui 
#     ${OpenCV_LIBS}
#     Threads::Threads
#     ${QT5_LIBRARIES}
# )