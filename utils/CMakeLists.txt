
aux_source_directory(. src)

set(SRC_DIR /home/zzy/workspace/code/c++/h264_delay_exp/simplest_ffmpeg_video_encoder/encode_decode_delay_measure)
include_directories(${SRC_DIR}/3rdParty/)

add_library(libcoreutils ${src})

target_link_libraries(libcoreutils ${SRC_DIR}/3rdParty/yaml/lib/libyaml-cpp.so avcodec avformat avutil swscale)