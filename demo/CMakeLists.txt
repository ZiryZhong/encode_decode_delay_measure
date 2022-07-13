cmake_minimum_required(VERSION 3.16)

include_directories("/usr/include")
# include_directories("/usr/local/ffmpeg/include")
# link_directories("/usr/local/ffmpeg/lib/")

# 开发者可以通过option调整编译选项
option(ENCODE "option for encoding" ON)
option(DECODE "option for decoding" ON)
option(RESCALE "option for rescaling" OFF)

if (ENCODE) 
    project(demo_encode)
    add_executable(demo_encode encode.cpp)
    target_link_libraries(demo_encode avcodec avutil avformat swscale)
    message("-- ENCODE configure !")

elseif()

    message("-- ENCODE disabled. Edit CMakeLists.txt to change")

endif()

if (DECODE) 
    project(demo_decode)
    add_executable(demo_decode decode_new.cpp)
    target_link_libraries(demo_decode avcodec avutil avformat swscale)
    message("-- DECODE configure !")

elseif()

    message("-- DECODE disabled. Edit CMakeLists.txt to change")

endif()


if (RESCALE) 
    project(demo_rescale)
    add_executable(demo_rescale rescale.cpp)
    target_link_libraries(demo_rescale avcodec avutil avformat swscale)
    message("-- RESCALE configure !")

elseif()

    message("-- RESCALE disabled. Edit CMakeLists.txt to change")

endif()


