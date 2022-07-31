/*
 * @Descripttion: 
 * @version: 
 * @Author: congsir
 * @Date: 2022-07-07 19:47:02
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-07-31 13:46:50
 */


#ifndef DELAY_MANAGER_HPP
#define DELAY_MANAGER_HPP

#include <stdio.h>
#include "yaml-cpp/yaml.h"
#include "string"
#include "iostream"
#include "fstream"
#include "base_delay_manager.hpp"



enum CODEC_TYPE{
    CODEC_H264_CPU = 0,
    CODEC_H265_GPU = 1
};



struct CodecPar{

    int codec_type;
    int qp;
    int width;
    int height;
    int frame_rate;
    int pix_fmt;
    
    CodecPar():
        codec_type(CODEC_H264_CPU),
        qp(30),
        width(480),
        height(272),
        frame_rate(20)
        {}

    CodecPar(int in_codec_type, int in_qp,int in_width,int in_height,int in_frame_rate):
        codec_type(in_codec_type),
        qp(in_qp),
        width(in_width),
        height(in_height),
        frame_rate(in_frame_rate)
        {}

};


class DelayManager : public BaseDelayManager {
private:
    
    // 输入和输出文件流
    // FILE * in_handler = NULL;
    // FILE * out_handler = NULL;
    
    // 编码器
    AVCodec * m_encoder = NULL;
    AVCodecContext * m_encoder_ctx = NULL;
    
    // 解码器
    AVCodec * m_decoder = NULL;
    AVCodecContext * m_decoder_ctx = NULL;
    AVStream * m_stream = NULL;
    int m_stream_id;
    // 尺度变换上下文
    SwsContext * m_sws_ctx = NULL;

    char * in_file_name;
    char * out_file_name;
    // char * config_file_name;

    

    // 记录相关
    std::fstream record_fst;

    YAML::Node config;
    // 解析总的配置文件
    bool parser_config(std::string config_file_path);
    
    // 编码
    bool encode_config(CodecPar in_codec_par);
    bool measure_encode(FILE* in_video, FILE* out_video, CodecPar in_codec_par);
    
    // 解码
    bool decode_config(AVFormatContext * fmt_ctx, AVCodecID in_codec_id);
    bool measure_decode(AVFormatContext * fmt_ctx, FILE* out_video, AVCodecID in_codec_id);

    bool encode_for_one_video(std::string video_path, std::string codec_type);
    bool decode_for_one_video(std::string video_path, std::string codec_type);
    // deprecated:
    // bool measure(std::string video_path, CodecPar codec_par);
    

public:

    // @TODO 进行时延计算 明天把整个时延测试部分的程序搭建完毕
    
    // 将配置文件写好即可进行时延测量
    int run(std::string config_file_path) override;

};




#endif // DELAY_MANAGER_HPP