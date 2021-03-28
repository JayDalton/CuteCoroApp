cmake_minimum_required(VERSION 3.18)

project(CuteWidgetApp CXX)
message(STATUS ${PROJECT_NAME})

add_executable(${PROJECT_NAME} WIN32)

find_package(fmt CONFIG REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Widgets)

target_sources(${PROJECT_NAME} PRIVATE
"common.h"
"commoncute.h"
"commoncoro.h"
"CuteColorApp.cpp"
"CuteColorApp.h"
"CuteWidgetApp.cpp"
"CuteWidgetApp.h"
"CuteWidgetApp.qrc"
"CuteWidgetApp.ui"
"main.cpp"
)