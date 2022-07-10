/*
 * @Descripttion: 
 * @version: 
 * @Author: congsir
 * @Date: 2022-07-06 19:37:19
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-07-07 19:33:35
 */


extern "C" {
    #include "libavcodec/avcodec.h"
    #include "libavutil/opt.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"
};
#include<iostream>

// 老版的API好像还需要知道数据流中每帧的大小
// 直接用新版的提取媒体文件信息
int h264_to_yuv420p(char* input_file, char* output_file);

int main() {
    
    char in_file_name[] = "./results/ds.h264";
    char out_file_name[] = "./results/ds_remake_new.yuv";

    // h264_to_yuv420p(in_file_name,out_file_name);

    FILE * out_file = NULL;
    int ret;
    int in_stream_idx;

    AVCodecID codec_id = AV_CODEC_ID_H264;
    AVCodec * codec_h264 = NULL;
    AVCodecContext * codec_h264_ctx = NULL;

    AVFrame * out_frame;
    AVFrame * out_frame_yuv;
    AVPacket in_packet;

    AVFormatContext * in_fmt_ctx = NULL;
    AVStream * in_stream = NULL;
    
    avcodec_register_all();

    avformat_network_init();

     if (avformat_open_input(&in_fmt_ctx, in_file_name, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", in_fmt_ctx);
        return -1;
    }

    if (avformat_find_stream_info(in_fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        return -1;
    }

    av_dump_format(in_fmt_ctx, 0, in_file_name, 0);

    // 从这个媒体流文件中提取出相对应的流 
    ret = av_find_best_stream(in_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(AVMEDIA_TYPE_VIDEO), in_file_name);
        return ret;
    }

    in_stream_idx = ret;
    in_stream = in_fmt_ctx->streams[in_stream_idx];

    codec_h264 = avcodec_find_decoder(codec_id); 
    if (!codec_h264) {
        return -1;
    }

    codec_h264_ctx = avcodec_alloc_context3(codec_h264);
    if (!codec_h264_ctx) {
        return -1;
    }

    if (avcodec_parameters_to_context(codec_h264_ctx,in_stream->codecpar) < 0) {
        return -1;
    }

    ret = avcodec_open2(codec_h264_ctx,codec_h264,NULL);
    if (ret < 0) {
        return -1;
    }

    out_file = fopen(out_file_name,"wb");
    if (!out_file) {
        return -1;
    }
    
    SwsContext * s_ctx = sws_getContext(in_stream->codecpar->width, 
                                    in_stream->codecpar->height, 
                                    codec_h264_ctx->pix_fmt, 
                                    in_stream->codecpar->width, 
                                    in_stream->codecpar->height, 
                                    AV_PIX_FMT_YUV420P, 
                                    SWS_BICUBIC, 
                                    NULL, NULL, NULL);
    int frame_num = 100;
    int cnt = 0;
    in_packet.data = NULL;
    in_packet.size = 0;
    int size = codec_h264_ctx->width * codec_h264_ctx->height;
    out_frame = av_frame_alloc();
    out_frame_yuv = av_frame_alloc();
    out_frame->width = in_stream->codecpar->width;
    out_frame->height = in_stream->codecpar->height;
    out_frame->format = in_stream->codecpar->format;
    std::cout << "0"<< std::endl;

    uint8_t * out_buffer = (uint8_t *)malloc(codec_h264_ctx->width * codec_h264_ctx->height * 3 / 2);
    std::cout << "1"<< std::endl;
    std::cout << codec_h264_ctx->width<< " " << codec_h264_ctx->height << std::endl;
	
    av_image_fill_arrays(out_frame_yuv->data, out_frame_yuv->linesize,out_buffer,AV_PIX_FMT_YUV420P,codec_h264_ctx->width, codec_h264_ctx->height,1);

    std::cout << out_frame->width <<" " <<out_frame->height<< std::endl;
    int got_picture;

    while (av_read_frame(in_fmt_ctx,&in_packet) >= 0) {
        
        if (in_packet.stream_index != in_stream_idx) {
            av_free_packet(&in_packet);
            continue;
        }
        ret = avcodec_decode_video2(codec_h264_ctx,out_frame,&got_picture,&in_packet);
        if (ret < 0) {
            return -1;
        }
        if (got_picture) {
            sws_scale(s_ctx,out_frame->data,out_frame->linesize,0,out_frame->height,out_frame_yuv->data,out_frame_yuv->linesize);
            fwrite(out_frame_yuv->data[0],1,size,out_file);
            fwrite(out_frame_yuv->data[1],1,size/4,out_file);
            fwrite(out_frame_yuv->data[2],1,size/4,out_file);
            std::cout << "decode : " << cnt << std::endl;
            cnt ++;
        }
        av_free_packet(&in_packet);

    }

    for(got_picture=1;got_picture;) {
        ret = avcodec_decode_video2(codec_h264_ctx,out_frame,&got_picture,&in_packet);
        if (ret < 0) {
            return -1;
        }
        if (got_picture) {
            sws_scale(s_ctx,out_frame->data,out_frame->linesize,1,out_frame->height,out_frame_yuv->data,out_frame_yuv->linesize);
            fwrite(out_frame_yuv->data[0],1,size,out_file);
            fwrite(out_frame_yuv->data[1],1,size/4,out_file);
            fwrite(out_frame_yuv->data[2],1,size/4,out_file);
            
            std::cout << "flush decode : " << cnt << std::endl;
            cnt ++;
        }
    }

    fclose(out_file);
    avformat_close_input(&in_fmt_ctx);
    avcodec_close(codec_h264_ctx);
    avcodec_free_context(&codec_h264_ctx);
    av_frame_free(&out_frame);
    av_frame_free(&out_frame_yuv);



    return 0;
}

