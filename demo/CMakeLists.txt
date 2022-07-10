cmake_minimum_required(VERSION 3.16)

project(demo)

include_directories("/usr/include")
# include_directories("/usr/local/ffmpeg/include")
# link_directories("/usr/local/ffmpeg/lib/")

add_executable(demo encode.cpp)

target_link_libraries(demo avcodec avutil avformat swscale)