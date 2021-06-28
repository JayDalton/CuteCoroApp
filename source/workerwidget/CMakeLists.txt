cmake_minimum_required(VERSION 3.18)

project(WorkerWidget CXX)
message(STATUS ${PROJECT_NAME})

add_executable(${PROJECT_NAME} WIN32)

find_package(fmt CONFIG REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Widgets)

target_sources(${PROJECT_NAME} PRIVATE
"ContextApp.cpp"
"ContextApp.h"
"main.cpp"
"MainWindow.cpp"
"MainWindow.h"
)