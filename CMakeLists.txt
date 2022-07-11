cmake_minimum_required(VERSION 3.16)

project(delay_measure)
PROJECTSOUR
include_directories(${PROJECT_SOURCE_DIR}/3rdParty/)

add_subdirectory(utils)

add_executable(delay_measure main.cpp)

target_link_libraries(delay_measure libcoreutils)