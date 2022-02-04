cmake_minimum_required(VERSION 3.18)

project(ConsoleCoroutine CXX)
message(STATUS ${PROJECT_NAME})

add_executable(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_sources(${PROJECT_NAME} PRIVATE
"main.cpp"
)