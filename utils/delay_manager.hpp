/*
 * @Descripttion: 
 * @version: 
 * @Author: congsir
 * @Date: 2022-07-07 19:47:02
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-07-09 23:00:29
 */


#ifndef DELAY_MANAGER_HPP
#define DELAY_MANAGER_HPP

#include <stdio.h>
#include "yaml-cpp/yaml.h"
#include "string"
#include "iostream"
#include "fstream"


struct CodecPar{

    int qp;
    int width;
    int height;
    int frame_rate;
    
    CodecPar():
        qp(30),
        width(480),
        height(272),
        frame_rate(20)
        {}

    CodecPar(int in_qp,int in_width,int in_height,int in_frame_rate):
        qp(in_qp),
        width(in_width),
        height(in_height),
        frame_rate(in_frame_rate)
        {}

};


class DelayManager {
private:
    
    FILE * in_handler = NULL;
    FILE * out_handler = NULL;

    char * in_file_name;
    char * out_file_name;
    char * config_file_name;


    // 记录相关
    std::fstream record_fst;

    YAML::Node config;
    // 解析实验配置
    bool parser_config();
    bool run_for_one_video(std::string video_path);
    bool measure(std::string video_path, CodecPar codec_par);

public:

    // @TODO 进行时延计算 明天把整个时延测试部分的程序搭建完毕
    int run();

};




#endif // DELAY_MANAGER_HPP