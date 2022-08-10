
aux_source_directory(. src)


# set(SRC_DIR ${PROJECT_SOURCE_DIRS})
message("PROJECT_SOURCE_DIR= ${PROJECT_SOURCE_DIR}")
include_directories(${PROJECT_SOURCE_DIR}/3rdParty/)
set(L_PATH /usr/local/ffmpeg-x265/lib)
#link_directories(/usr/local/ffmpeg/lib/)
add_library(libcoreutils ${src})

target_link_libraries(libcoreutils ${PROJECT_SOURCE_DIR}/3rdParty/yaml/lib/libyaml-cpp.so ${L_PATH}/libavcodec.so ${L_PATH}/libavformat.so ${L_PATH}/libavutil.so ${L_PATH}/libswscale.so)
#target_link_libraries(libcoreutils ${PROJECT_SOURCE_DIR}/3rdParty/yaml/lib/libyaml-cpp.so avcodec avformat avutil swscale)
