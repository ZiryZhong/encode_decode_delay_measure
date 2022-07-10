cmake_minimum_required(VERSION 3.16)

project(delay_measure)

include_directories(/home/zzy/workspace/code/c++/h264_delay_exp/simplest_ffmpeg_video_encoder/encode_decode_delay_measure/3rdParty/)

add_subdirectory(utils)

add_executable(delay_measure main.cpp)

target_link_libraries(delay_measure libcoreutils)