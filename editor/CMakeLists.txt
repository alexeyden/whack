cmake_minimum_required(VERSION 2.8.11)

project(level_editor)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)
find_package(Threads)

file(GLOB EDITOR_UI_FILES *.ui)
file(GLOB EDITOR_SRC_FILES *.cpp)
file(GLOB EDITOR_QRC_FILES *.qrc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

qt5_wrap_ui(UI_HEADERS ${EDITOR_UI_FILES})
qt5_add_resources(RESOURCES ${EDITOR_QRC_FILES})

add_executable(level_editor ${EDITOR_SRC_FILES} ${UI_HEADERS} ${RESOURCES})
target_link_libraries(level_editor Qt5::Widgets resources graphics level behaviour objects behaviour util ${CMAKE_THREAD_LIBS_INIT} msgpackc)
