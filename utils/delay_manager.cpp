/*
 * @Descripttion: 
 * @version: 
 * @Author: congsir
 * @Date: 2022-07-07 19:47:23
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-07-15 14:49:19
 */

#include "delay_manager.hpp"
#include <chrono>


extern "C" {

    #include "libavcodec/avcodec.h"
    #include "libavutil/imgutils.h"
    #include "libavutil/opt.h"
    #include "libswscale/swscale.h"
    #include "libavformat/avformat.h"

}


bool DelayManager::parser_config() {
    
    config_file_name = "./config.yaml";
    config = YAML::LoadFile(config_file_name);
    
    return true;
}


bool DelayManager::run_for_one_video(std::string video_path) {
    
    CodecPar codec_par;
    int ret = 0;
    
    // std::cout << "[INFO] " << "in 1" << std::endl;
    std::cout << "[INFO] " <<"resolution cnt:" << config["encode"]["resolution"].size() << std::endl;
    std::cout << "[INFO] " <<"framerate cnt:" << config["encode"]["framerate"].size() << std::endl;
    std::cout << "[INFO] " <<"QP cnt:" << config["encode"]["QP"].size() << std::endl;

    for (int i=0;i<config["encode"]["resolution"].size();i++) {
        // std::cout << "[INFO] " << "in 2" << std::endl;
        std::cout << config["encode"]["resolution"][i][0]
                  << (config["encode"]["resolution"][i][1]) << std::endl;
        codec_par.width = config["encode"]["resolution"][i][0].as<int>();
        codec_par.height = config["encode"]["resolution"][i][1].as<int>();
        
        for( int j=0;j<config["encode"]["framerate"].size();j++) {
            std::cout << "[INFO] " << "in 3" << std::endl;
            codec_par.frame_rate = config["encode"]["framerate"][j].as<int>();
            
            for(int k=0;k<config["encode"]["QP"].size();k++) {
                std::cout << "[INFO] " << "in 4" << std::endl;
                codec_par.qp = config["encode"]["QP"][k].as<int>();
                
                std::cout << "[INFO] " << "start measure delay" << std::endl;
                ret = measure(video_path, codec_par);
                if (!ret) break;
            }
        }
    }

    return ret;
}


bool DelayManager::measure(std::string video_path, CodecPar codec_par) {


// ----------------------------
// |        编码部分           |
// ---------------------------- 
    double file_size = 0;
    int in_width = 1920;
    int in_height = 1080;
    int down_sample_factor = 1;

    int64_t encode_duration = 0;
    int64_t decode_duration = 0;

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;

    std::cout << "[INFO] " << "视频文件:"<< video_path << std::endl;
    std::cout << "[INFO] " << "编码后输出路径:"<< "./results/one.h264" << std::endl;
    std::cout << "[INFO] " << "开始编码" << std::endl;

    in_handler = fopen(video_path.c_str(),"rb");
    if (!in_handler) {
        fclose(in_handler);
        return false;
    }
    out_handler = fopen("./results/one.h264","wb");
    if (!out_handler) {
        fclose(in_handler);
        fclose(out_handler);
        return false;
    }
    
    AVCodecID codec_id = AV_CODEC_ID_H264;
    AVCodec * encoder_h264 = NULL;
    AVCodecContext * encoder_h264_ctx = NULL;

    encoder_h264 = avcodec_find_encoder(codec_id);
    if (!encoder_h264) {
        // ...
        std::cout << "[ERROR] " << "encoder_h264 error" << std::endl;
        return false;
    }
    encoder_h264_ctx = avcodec_alloc_context3(encoder_h264);
    if (!encoder_h264_ctx) {
        // ...
        std::cout << "[ERROR] " << "encoder_h264_ctx error" << std::endl;
        return false;
    }

    // 设置编码相关的参数
    encoder_h264_ctx->pix_fmt = AV_PIX_FMT_YUV420P; // 设置像素格式为YUV420P 
    encoder_h264_ctx->width = codec_par.width; // 不同尺寸编码 
    encoder_h264_ctx->height = codec_par.height; // 不同尺寸编码 
    
    encoder_h264_ctx->time_base.num = 1;  
    encoder_h264_ctx->time_base.den = codec_par.frame_rate; // 设置帧率大小

    encoder_h264_ctx->qmax = codec_par.qp; // 固定量化参数QP的大小
    encoder_h264_ctx->qmin = codec_par.qp; // 固定量化参数QP的大小
    
    encoder_h264_ctx->max_b_frames = 0; // 不需要B帧
    encoder_h264_ctx->gop_size = 120; // GOP设置为 120 保证GOP的格式为IPPP... 只含一个I帧

    av_opt_set(encoder_h264_ctx->priv_data,"preset","ultrafast",0); // 对应极速（ultrafast）
    av_opt_set(encoder_h264_ctx->priv_data,"tune","zerolatency",0); // 对应零延迟（zerolatency）

     
    if(avcodec_open2(encoder_h264_ctx,encoder_h264,NULL)<0) {
        // ...
        std::cout << "[ERROR] " << "encoder open error" << std::endl;
        return false;
    }

    // 初始化 Frame 和 Packet 容器
    AVFrame * in_frame = NULL;
    AVFrame * rescale_frame = NULL;

    int in_buffer_size = in_width * in_height * 3 / 2;
    int out_buffer_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, codec_par.width, codec_par.height, 1);
    uint8_t * in_buffer = (uint8_t *)malloc(in_buffer_size );
    uint8_t * out_buffer = (uint8_t *)malloc(out_buffer_size );

    AVPacket out_pkt;
    av_init_packet(&out_pkt);
    out_pkt.data = NULL;
    out_pkt.size = 0;

    // 初始化 尺度变换 上下文
    SwsContext * sws_ctx = sws_getContext(in_width,in_height,AV_PIX_FMT_YUV420P,
                                        codec_par.width,codec_par.height,AV_PIX_FMT_YUV420P,
                                        SWS_BICUBIC,NULL,NULL,NULL);


    int one_frame_size = codec_par.width * codec_par.height;
    int got_picture = 1;
    int cnt = 0;
    int frame_num = 0;
    int ret = 0;
    
    down_sample_factor = 120 / codec_par.frame_rate;
    // 进入编码循环
    while (cnt < 120) {
        
        // if (fread(in_frame->data[0],1,one_frame_size,in_handler) <= 0 ||
        //     fread(in_frame->data[1],1,one_frame_size,in_handler) <= 0 ||
        //     fread(in_frame->data[2],1,one_frame_size,in_handler) <= 0 ) {
        //     // ...

        //     return false;
        // } else if (feof(in_handler)) {
        //     break;
        // }
        
        // @TODO: 在正式编码前还需要分辨率的转换 sws_scale
        int ret = fread(in_buffer,1,in_buffer_size,in_handler);

        if (!ret) {
            // LOG_W("fread empty");
            break;
        }

        // 以120FPS为标准 降采样
        if (cnt % down_sample_factor != 0) {
            cnt ++;
            continue;
        }

        in_frame = av_frame_alloc();
        rescale_frame = av_frame_alloc();
        //
        av_image_fill_arrays(in_frame->data,in_frame->linesize,in_buffer,AV_PIX_FMT_YUV420P,in_width,in_height,1);
        av_image_fill_arrays(rescale_frame->data,rescale_frame->linesize,out_buffer,AV_PIX_FMT_YUV420P,codec_par.width,codec_par.height,1);
        in_frame->format = encoder_h264_ctx->pix_fmt;
        in_frame->width = in_width;
        in_frame->height = in_height;

        rescale_frame->format = encoder_h264_ctx->pix_fmt;
        rescale_frame->width = encoder_h264_ctx->width;
        rescale_frame->height = encoder_h264_ctx->height;
        // 进行尺度变换
        sws_scale(sws_ctx,in_frame->data,in_frame->linesize,0,in_height,rescale_frame->data,rescale_frame->linesize);

        // 记录编码时间
        start = std::chrono::high_resolution_clock::now(); 
        ret = avcodec_encode_video2(encoder_h264_ctx,&out_pkt,rescale_frame,&got_picture);
        end = std::chrono::high_resolution_clock::now();

        encode_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        if(ret < 0) {
            // ...
            return false;
        }
        in_frame->pts = cnt;
        
        if(got_picture) {
            fwrite(out_pkt.data,1,out_pkt.size,out_handler);
            // std::cout << "[INFO] " << "success encode:" << frame_num << std::endl;
            file_size += out_pkt.size;
            frame_num ++ ;
        }
        cnt ++;
        av_frame_free(&in_frame);
        av_frame_free(&rescale_frame);
        av_packet_unref(&out_pkt);
    }

    // 缓冲区清空
    for(got_picture=1;got_picture;) {
        
        ret = avcodec_encode_video2(encoder_h264_ctx,&out_pkt,NULL,&got_picture);
        
        if(ret < 0) {
            // ...
            std::cout << "[WARN] " << "flush encode fail" << std::endl;
            break;
        } else {
            // ... 

        }
        if(got_picture) {

            fwrite(out_pkt.data,1,out_pkt.size,out_handler);
            std::cout << "[INFO] " << "success flush encode:"<<frame_num << std::endl;
            av_packet_unref(&out_pkt);
        }
    }
    
    fclose(in_handler);
    fclose(out_handler);
    avcodec_close(encoder_h264_ctx);
    av_free(encoder_h264_ctx);
    // av_freep(&in_frame->data[0]);
    // av_frame_free(&in_frame);
    sws_freeContext(sws_ctx);

// ----------------------------
// |        解码部分           |
// ---------------------------- 

    std::cout << "[INFO] " << "start decoding!" << std::endl;

    FILE * decode_out_handler = NULL;
    decode_out_handler = fopen("./results/one.yuv","wb");

    if (! decode_out_handler) {
        // ...
        std::cout << "[ERROR] " << "decode output handler open fail" << std::endl;
        return false;
    }

    AVFormatContext * fmt_ctx = NULL;
    AVStream * in_stream = NULL;
    AVCodec * decoder_h264 = NULL;
    AVCodecContext * decoder_h264_ctx = NULL;

    if (avformat_open_input(&fmt_ctx,"./results/one.h264",NULL,NULL) < 0) {
        // ...
        std::cout << "[ERROR] " << "avformat open fail:"<< "./results/one.h264" << std::endl;
        return false;
    }

    if (avformat_find_stream_info(fmt_ctx,NULL) < 0) {
        // ...
        std::cout << "[ERROR] " << "find stream info error:" << std::endl;
        return false;
    }

    ret = av_find_best_stream(fmt_ctx,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);
    if( ret < 0) {
        // ...
        std::cout << "[INFO] " << "find best stream error" << std::endl;
        return false;
    }


    int st_idx = ret;
    in_stream =  fmt_ctx->streams[st_idx];

    decoder_h264 = avcodec_find_decoder(codec_id);
    decoder_h264_ctx = avcodec_alloc_context3(decoder_h264);

    if (avcodec_parameters_to_context(decoder_h264_ctx,in_stream->codecpar) < 0) {
        // ...
        std::cout << "[ERROR] " << "codec parameter copy error" << std::endl;
        return false;
    }

    if (avcodec_open2(decoder_h264_ctx,decoder_h264,NULL) < 0) {
        // ...  
        std::cout << "[ERROR] " << "decoder open error" << std::endl;
        return false;
    }
    
    sws_ctx = sws_getContext(in_stream->codecpar->width, 
                                in_stream->codecpar->height, 
                                decoder_h264_ctx->pix_fmt, 
                                in_stream->codecpar->width, 
                                in_stream->codecpar->height, 
                                AV_PIX_FMT_YUV420P, 
                                SWS_BICUBIC, 
                                NULL, NULL, NULL);

    AVFrame * rescale_frame_yuv;
    AVPacket in_pkt;
    in_pkt.data = NULL;
    in_pkt.size = 0;
    int size = decoder_h264_ctx->width * decoder_h264_ctx->height;
    rescale_frame = av_frame_alloc();
    rescale_frame_yuv = av_frame_alloc();
    rescale_frame->width = in_stream->codecpar->width;
    rescale_frame->height = in_stream->codecpar->height;
    rescale_frame->format = in_stream->codecpar->format;
    std::cout << "0"<< std::endl;

    out_buffer = (uint8_t *)malloc(decoder_h264_ctx->width * decoder_h264_ctx->height * 3 / 2);
    std::cout << "1"<< std::endl;
    std::cout << decoder_h264_ctx->width<< " " << decoder_h264_ctx->height << std::endl;
	
    av_image_fill_arrays(rescale_frame_yuv->data, rescale_frame_yuv->linesize,out_buffer,AV_PIX_FMT_YUV420P,decoder_h264_ctx->width, decoder_h264_ctx->height,1);

    std::cout << rescale_frame->width <<" " << rescale_frame->height<< std::endl;
    // 开始解码
    cnt = 0;
    

    while (av_read_frame(fmt_ctx,&in_pkt) >= 0) {
        
        if (in_pkt.stream_index != st_idx) {
            av_free_packet(&in_pkt);
            continue;
        }

        // 这边记录解码的时间
        start = std::chrono::high_resolution_clock::now();
        ret = avcodec_decode_video2(decoder_h264_ctx,rescale_frame,&got_picture,&in_pkt);
        end = std::chrono::high_resolution_clock::now();
        decode_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        if (ret < 0) {
            return -1;
        }
        if (got_picture) {
            // sws_scale(sws_ctx,rescale_frame->data,rescale_frame->linesize,0,rescale_frame->height,rescale_frame_yuv->data,rescale_frame_yuv->linesize);
            // fwrite(rescale_frame_yuv->data[0],1,size,decode_out_handler);
            // fwrite(rescale_frame_yuv->data[1],1,size/4,decode_out_handler);
            // fwrite(rescale_frame_yuv->data[2],1,size/4,decode_out_handler);
            // std::cout << "[INFO] " <<"success decode : " << cnt << std::endl;
            cnt ++;
        }
        av_free_packet(&in_pkt);

    }
    

    std::cout << decode_duration*1e-9 << std::endl;
    for(got_picture=1;got_picture;) {

        ret = avcodec_decode_video2(decoder_h264_ctx,rescale_frame,&got_picture,&in_pkt);
        if (ret < 0) {
            return -1;
        }
        if (got_picture) {
            // sws_scale(sws_ctx,rescale_frame->data,rescale_frame->linesize,1,rescale_frame->height,rescale_frame_yuv->data,rescale_frame_yuv->linesize);
            // fwrite(rescale_frame_yuv->data[0],1,size,decode_out_handler);
            // fwrite(rescale_frame_yuv->data[1],1,size/4,decode_out_handler);
            // fwrite(rescale_frame_yuv->data[2],1,size/4,decode_out_handler);
            
            std::cout << "[INFO] " <<"flush decode : " << cnt << std::endl;
            cnt ++;
        }
    }

    fclose(decode_out_handler);
    avformat_close_input(&fmt_ctx);
    avcodec_close(decoder_h264_ctx);
    avcodec_free_context(&decoder_h264_ctx);
    av_frame_free(&rescale_frame);
    av_frame_free(&rescale_frame_yuv);

    // 将最后的时间输出到文件:
    file_size = file_size / 1000.f;
    std::cout << "[INFO] " << "finish one measure !"<< std::endl;
    std::cout << "[INFO] " << "file size:" << file_size << std::endl;
    std::cout << "[INFO] " << "frame num:" << frame_num << std::endl;
    record_fst << video_path <<" "<< file_size << " " <<codec_par.width << " " << codec_par.height << " " << codec_par.qp
                << " " << codec_par.frame_rate <<" " << frame_num  << " "<< (encode_duration + decode_duration) * 1e-9 / (frame_num )
                <<" " <<encode_duration*1e-9 << " " <<decode_duration*1e-9 << " " 
                <<(encode_duration + decode_duration) * 1e-9 <<"\n";
    
    return true;
}


int DelayManager::run() {
    
    parser_config();

    std::cout << "[INFO] " << "finish parser config"<< std::endl;
    record_fst.open(config["output"]["recordpath"].as<std::string>(),std::ios::out);
    
    std::cout << "[INFO] 时延参数输出文件:" << config["output"]["recordpath"].as<std::string>() << std::endl;
    if (!record_fst.is_open()) {
        
        std::cout << "[ERROR] "<< "record file open failed !" << std::endl;
        return false;
    }

    for (int i=0;i<config["videos"].size();i++) {
        
        run_for_one_video(config["videos"][i].as<std::string>());
    
    }

    record_fst.close();
    return 1;
}
