cmake_minimum_required(VERSION 3.18)

project(Common CXX)
message(STATUS ${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC)

find_package(fmt CONFIG REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Widgets)

target_sources(${PROJECT_NAME} PRIVATE
"common.h"
"commoncute.h"
"commoncoro.h"
)