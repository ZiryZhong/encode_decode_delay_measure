<!--
 * @Descripttion: 
 * @version: 
 * @Author: congsir
 * @Date: 2022-07-11 15:15:14
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-07-13 14:32:46
-->
0 简介
====

本项目主要用于测试视频编解码时延的测定，目前仅支持YUV和H264之间的编解码。编解码部分的核心代码在 ./utils/delay_manager.cpp 中的 measure() 函数中。


1 目录结构说明
====
本项目主要采用CMakeLists进行构建。
+ 3rdParty 引用了开源YAML库
+ build 存放编译相关的文件
+ demo 含有编码、尺度变换、解码三种单独操作的基本样例
+ results 用于存储编码后的h264文件
+ utils 用于测定编解码时延的类
+ videos 可以存放需要进行编解码的原始YUV文件


2 运行环境
====
+ ubuntu20.04 或 ubuntu22.04 
+ FFMPEG v4.02
+ cmake、gcc、g++

3 运行方式
====
+ 修改config.yaml中videos字段中的视频路径、以及其他字段的编码参数
+ 如果提示 **FFMPEG** 的动态链接库找不到，先检查系统环境变量是否添加了路径，如果解决不了，则可以修改 **utils** 目录下 **target_link_libraries** 的库文件链接路径。
+ 创建并进入 **build** 文件夹执行下列命令
```
    mkdir build
    cmake ..
    make
```
+ 运行程序请进入到项目根目录**/encode_decode_delay_measure/**执行
```
    ./build/delay_measure
```
+ 输出结果保存在 **./results/delay.txt**
    