cmake_minimum_required(VERSION 3.18)

message(STATUS "Using CMake ${CMAKE_VERSION}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# set up project
project(cutecoro)
message(STATUS ${PROJECT_NAME})

enable_testing()

option(UPDATE_ALL_PROJECTS "Update all sources lists" OFF)
if (${UPDATE_ALL_PROJECTS})
    message(STATUS "Updating sources lists:")
    update_target_sources(${CMAKE_SOURCE_DIR}/source)
endif()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/Release)

# Generate launch.vs.json
# string(REPLACE "/" "\\\\" CMAKE_RUNTIME_OUTPUT_DIRECTORY_BACKSLASHED ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
# configure_file(launch.vs.json.in "${PROJECT_SOURCE_DIR}/launch.vs.json" @ONLY)

set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(SRC_DIR ${CMAKE_SOURCE_DIR}/source)

set(ALLOW_UNITY_BUILD OFF)
cmake_dependent_option(CMAKE_UNITY_BUILD "Use Unity Build" ON "ALLOW_UNITY_BUILD" OFF)
message(STATUS "Unity builds are ${CMAKE_UNITY_BUILD}")

# Prepare the build
option(SHOW_CONSOLE "Console" ON)
option(SHOW_WIDGET "Widget App" ON)

if (${SHOW_CONSOLE} OR ${SHOW_WIDGET})
    # add_subdirectory(source/common)
    # add_subdirectory(source/coroconsole)
    add_subdirectory(source/coroqtwidget)
endif()