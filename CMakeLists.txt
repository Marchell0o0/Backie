# /path/to/your/project/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

project(Backie VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the required Qt package
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core)

# Include spdlog library
add_subdirectory(spdlog)

# Add the subdirectories for BackieGui and BackieWorker
add_subdirectory(BackieGui)
add_subdirectory(BackieWorker)
