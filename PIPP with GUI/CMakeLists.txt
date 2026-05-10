cmake_minimum_required(VERSION 3.10)
project(ImagePipeline)

set(CMAKE_CXX_STANDARD 17)

# to resolve error - enable Qt's automatic MOC processing.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find packages
find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Widgets QUIET)

include_directories(${OpenCV_INCLUDE_DIRS} src)

set(SOURCES
    src/main.cpp
    src/pipeline.cpp
    src/filters.cpp
    src/performance.cpp
    src/gui.cpp
    src/gui.h         
)

add_executable(pipeline ${SOURCES})

if(Qt5_FOUND)
    message(STATUS "Qt5 found — GUI enabled")
    target_compile_definitions(pipeline PRIVATE GUI_ENABLED)
    target_link_libraries(pipeline ${OpenCV_LIBS} pthread Qt5::Widgets)
else()
    message(STATUS "Qt5 not found — running without GUI")
    target_link_libraries(pipeline ${OpenCV_LIBS} pthread)
endif()
