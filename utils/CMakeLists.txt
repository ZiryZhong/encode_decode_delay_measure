
aux_source_directory(. src)

#set(SRC_DIR ${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/3rdParty/)

add_library(libcoreutils ${src})

target_link_libraries(libcoreutils ${PROJECT_SOURCE_DIR}/3rdParty/yaml/lib/libyaml-cpp.so avcodec avformat avutil swscale)