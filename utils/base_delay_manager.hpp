/*
 * @Descripttion: 
 * @version: 
 * @Author: zzy
 * @Date: 2022-07-30 11:25:23
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-07-31 10:27:26
 */



#ifndef BASE_DELAY_MANAGER_HPP
#define BASE_DELAY_MANAGER_HPP


extern "C" {
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libswscale/swscale.h>

}

// 时延测量抽象类
class BaseDelayManager
{
public:

    virtual int run(std::string config_file_path) = 0;

};

#endif // BASE_DELAY_MANAGER_HPP