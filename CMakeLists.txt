cmake_minimum_required(VERSION 3.18)

message(STATUS "Using CMake ${CMAKE_VERSION}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# set up project
project(cutecoro)
message(STATUS ${PROJECT_NAME})

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(Qt5_DIR "c:/Qt/Qt5.12.10/5.12.10/msvc2017/lib/cmake/Qt5" CACHE PATH "Initial cache" FORCE)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/Release)

set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(SRC_DIR ${CMAKE_SOURCE_DIR}/source)

set(ALLOW_UNITY_BUILD OFF)
cmake_dependent_option(CMAKE_UNITY_BUILD "Use Unity Build" ON "ALLOW_UNITY_BUILD" OFF)
message(STATUS "Unity builds are ${CMAKE_UNITY_BUILD}")

# Prepare the build
add_subdirectory(source/common)
add_subdirectory(source/consolecoroutine)
add_subdirectory(source/consolesemaphore)
add_subdirectory(source/consolestandard)
add_subdirectory(source/coroconsole)
add_subdirectory(source/coroqtwidget)
add_subdirectory(source/invokewidget)
add_subdirectory(source/modernwidget)
add_subdirectory(source/normalwidget)
add_subdirectory(source/sharedwidget)
add_subdirectory(source/threadwidget)
add_subdirectory(source/workerwidget)
