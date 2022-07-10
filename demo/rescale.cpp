/*
 * @Descripttion: 
 * @version: 
 * @Author: congsir
 * @Date: 2022-07-06 19:36:55
 * @LastEditors: zzy
 * @LastEditTime: 2022-07-06 21:56:47
 */

extern "C"
{
    #include "libavutil/opt.h"
    #include "libavcodec/avcodec.h"
    #include "libavutil/imgutils.h"
    #include "libswscale/swscale.h"
};


#include "../utils/log.hpp"


int main () {

    int in_width = 480;
    int in_height = 272;

    int out_width = 240;
    int out_height = 136;

    AVFrame * in_frame = NULL;
    AVFrame * out_frame = NULL;

    FILE * in_file = NULL;
    FILE * out_file = NULL;

    char in_file_name[] = "../videos/ds_480x272.yuv";
    char out_file_name[] = "./results/ds_480x272_rescale.yuv";

    // 打开输入文件
    in_file = fopen(in_file_name, "rb");
    if (!in_file) {
        LOG_E("in_file open failed");
        return -1;
    }

    // 打开输出文件
    out_file = fopen(out_file_name, "wb");
    if (!out_file) {
        LOG_E("out_file open failed");
        return -1;
    }
    
    // 创建scale操作上下文
    SwsContext *sws_ctx = sws_getContext(in_width,in_height,AV_PIX_FMT_YUV420P,
                                        out_width,out_height,AV_PIX_FMT_YUV420P,
                                        SWS_BICUBIC,NULL,NULL,NULL);
    if (!sws_ctx) {
        LOG_E("sws_ctx init failed");
        return -1;
    }
    
    int in_buffer_size = in_width * in_height * 3 / 2;
    int out_buffer_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, out_width, out_height, 1);

    uint8_t * in_buffer = (uint8_t *)malloc(in_buffer_size );
    uint8_t * out_buffer = (uint8_t *)malloc(out_buffer_size );

    // LOG_I(in_buffer_size and out_buffer_size);
    std::cout << in_buffer_size << " " << out_buffer_size  << std::endl;

    if (!in_buffer) {
        LOG_E("in_buffer init failed");
        return -1;
    }

    if (!out_buffer) {
        LOG_E("out_buffer init failed");
        return -1;
    }

    int num = 0;
    while(!feof(in_file)) {
        int ret = fread(in_buffer,1,in_buffer_size,in_file);
        
        if (!ret) {
            LOG_W("fread empty");
            break;
        }

        in_frame = av_frame_alloc();
        out_frame = av_frame_alloc();

        av_image_fill_arrays(in_frame->data,in_frame->linesize,in_buffer,AV_PIX_FMT_YUV420P,in_width,in_height,1);
        av_image_fill_arrays(out_frame->data,out_frame->linesize,out_buffer,AV_PIX_FMT_YUV420P,out_width,out_height,1);

        sws_scale(sws_ctx,in_frame->data,in_frame->linesize,0,in_height,out_frame->data,out_frame->linesize);

        fwrite(out_buffer,1,out_buffer_size,out_file);
        av_frame_free(&in_frame);
        av_frame_free(&out_frame);
        
        num ++ ;
        if (num >= 100) break;
    }    

    LOG_I("finish rescale");

_end:
    fclose(in_file);
    fclose(out_file);
    sws_freeContext(sws_ctx);
    free(in_buffer);
    free(out_buffer);

    return 0;
}