cmake_minimum_required(VERSION 3.10)
project(lecture_2)
set(CMAKE_CXX_STANDARD 11)
include_directories(homework)
include_directories(tools.hpp)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(fmt REQUIRED CONFIG)
find_package(OpenCV REQUIRED)

add_executable(fmt_test fmt_test.cpp)
add_executable(main main.cpp tools.cpp)

target_link_libraries(fmt_test PRIVATE fmt::fmt)
target_link_libraries(main ${OpenCV_LIBS})

