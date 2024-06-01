cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(st CXX)

find_package(spdlog REQUIRED HINTS /opt/spdlog )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(spdtest main.cpp)
target_link_libraries(spdtest spdlog::spdlog)
