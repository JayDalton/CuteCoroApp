cmake_minimum_required(VERSION 3.18)

project(CuteWidgetApp CXX)
message(STATUS ${PROJECT_NAME})

add_executable(${PROJECT_NAME} WIN32)
#set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${PLUGINS_FOLDER})

#add_dependencies(${PROJECT_NAME} ppluginopcua)
#target_precompile_headers(${PROJECT_NAME} PRIVATE precompheader.h)

find_package(fmt CONFIG REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Widgets)
# target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only) 
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::sqlite3::sqlite3)

target_sources(${PROJECT_NAME} PRIVATE
"ColorRect.cpp"
"ColorRect.h"
"common.h"
"CuteWidgetApp.cpp"
"CuteWidgetApp.h"
"CuteWidgetApp.qrc"
"CuteWidgetApp.ui"
"main.cpp"
"meta.h"
"QtCoro.h"
)