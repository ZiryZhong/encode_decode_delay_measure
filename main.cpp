/*
 * @Descripttion: 
 * @version: 
 * @Author: congsir
 * @Date: 2022-07-05 11:58:12
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-07-10 13:43:44
 */

#include<iostream>
#include "./utils/log.hpp"
#include "./utils/delay_manager.hpp"
#include <chrono>
#include <thread>
// 整体框架
// 编码和解码部分
// 实验配置文件读入
// 1 可变编码参数
// 2 视频列表
// --> 这边需要对每个yuv流，应用80种组合的视频参数
// 明天早上力扣 + 看论文 + ffmpeg 编码端的基本框架写完
// 明天问一下测定编码时间的问题

int main() {

    DelayManager dm;

    dm.run();


    
    // auto t1 = std::chrono::high_resolution_clock::now();

    // //运行代码段
    // std::this_thread::sleep_for(std::chrono::seconds(1));

    // auto t2 = std::chrono::high_resolution_clock::now();

    // auto time_used = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() * 1e-9; 
    // std::cout << "代码段运行用时" << time_used << "s" << std::endl;

    return 0;
}