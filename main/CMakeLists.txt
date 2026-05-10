cmake_minimum_required(VERSION 3.10)
project(ImagePipeline)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} src)

set(SOURCES
    src/main.cpp
    src/pipeline.cpp
    src/filters.cpp
    src/gui.cpp
    src/performance.cpp
)

add_executable(pipeline ${SOURCES})
target_link_libraries(pipeline ${OpenCV_LIBS} pthread)
