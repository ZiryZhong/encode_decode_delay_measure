/*
 * @Descripttion: 
 * @version: 
 * @Author: congsir
 * @Date: 2022-07-06 19:37:11
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-07-13 10:13:19
 */


// 老版的编码方式，新版FFMPEG中某些编码的函数修改了
// @TODO:时间充裕的话写一下新版的FFMPEG编码
#include<iostream>
extern "C"
{
    #include "libavutil/opt.h"
    #include "libavcodec/avcodec.h"
    #include "libavutil/imgutils.h"
    #include "libswscale/swscale.h"
};

int main(int argc, char* argv[])
{
    // 初始化变量
    char in_file_name[] = "../videos/ds_480x272.yuv";
    char out_file_name[] = "./results/ds.h264";

    FILE * in_file = NULL;
    FILE * out_file = NULL;

    int frame_num = 100;
    int in_width = 480;
    int in_height = 272;
    int one_frame_buffer_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,in_width,in_height,1);
    int got_output;

    AVCodecID codec_id = AV_CODEC_ID_H264;
    AVCodec * codec_h264 = NULL;
    AVCodecContext * codec_h264_ctx = NULL;

    AVFrame * in_frame = NULL;
    AVPacket out_pkt;

	avcodec_register_all();

    // 寻找编码器
    codec_h264 = avcodec_find_encoder(codec_id);
    if (!codec_h264) {
        std::cout << "1" << std::endl;
        return -1;
    }
    codec_h264_ctx = avcodec_alloc_context3(codec_h264);
    if (!codec_h264_ctx) {
        std::cout << "2" << std::endl;
        return -1;
    }

    // 编码参数设置
    // codec_h264_ctx->bit_rate = 400000;
    codec_h264_ctx->width = in_width;
    codec_h264_ctx->height = in_height;
    codec_h264_ctx->qmax = 30;
    codec_h264_ctx->qmin = 30;
    codec_h264_ctx->time_base.num = 1;
    codec_h264_ctx->time_base.den = 25;
    // codec_h264_ctx->framerate = 30;
    // codec_h264_ctx->gop_size = 10;
    codec_h264_ctx->max_b_frames = 0;
    codec_h264_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
        
    av_opt_set(codec_h264_ctx->priv_data, "preset", "ultrafast", 0);
    av_opt_set(codec_h264_ctx->priv_data, "tune", "zerolatency", 0);

    int ret = avcodec_open2(codec_h264_ctx,codec_h264,NULL);
    if (ret < 0) {
        std::cout << "3" << std::endl;
        return -1;
    }

    // 初始化帧容器
    in_frame = av_frame_alloc();

    in_frame->format = codec_h264_ctx->pix_fmt;
    in_frame->width = in_width;
    in_frame->height = in_height;

    ret = av_image_alloc(in_frame->data, in_frame->linesize, codec_h264_ctx->width, codec_h264_ctx->height,
                    codec_h264_ctx->pix_fmt, 16);
    if (ret < 0) {
        std::cout << "4" << std::endl;
        return -1;
    }

    in_file = fopen(in_file_name, "rb");
    out_file = fopen(out_file_name, "wb");
    
    // 开启输入输出文件流
    if (!in_file) {
        std::cout << "5" << std::endl;
        return -1;
    }

    if (!out_file) {
        std::cout << "6" << std::endl;
        return -1;
    }

    int frame_cnt = 0;
    int size = codec_h264_ctx->height * codec_h264_ctx->width;
    int down_sample_rate = 120 / codec_h264_ctx->time_base.den;

    for (int i=0;i<frame_num;i++) {
        av_init_packet(&out_pkt);
        out_pkt.data = NULL;    // packet data will be allocated by the encoder
        out_pkt.size = 0;
		std::cout << "i:" << i << std::endl;
        //Read raw YUV data
        // 这边要注意frame不能直接用fread读入 fread 只能写入uint——8类型的对象
        // 因此要不然就要借用imagutil去读入 要不然就要分通道读取

		if (fread(in_frame->data[0],1,size,in_file) <=0 ||
            fread(in_frame->data[1],1,size/4,in_file) <=0 ||
            fread(in_frame->data[2],1,size/4,in_file) <=0 ){
            std::cout << "7" << std::endl;
            std::cout << one_frame_buffer_size << std::endl;
            goto _end;
			return -1;
		} else if(feof(in_file)){
			break;
		}

        in_frame->pts = i;
        // @TODO 直接对源序列降采样 节省编码时间

        if (i % down_sample_rate != 0) continue;
        
        /* encode the image */
        ret = avcodec_encode_video2(codec_h264_ctx, &out_pkt, in_frame, &got_output);
        if (ret < 0) {
            printf("Error encoding frame\n");
            goto _end;
            return -1;
        }
        if (got_output) {
            printf("Succeed to encode frame: %5d\tsize:%5d\n",frame_cnt,out_pkt.size);
			frame_cnt++;
            std::cout << "pkt.pts:" << out_pkt.pts << std::endl;
            std::cout << "pkt.pts:" << out_pkt.duration << std::endl;
            std::cout << "frame rate:" << codec_h264_ctx->framerate.den << " "
                                        << codec_h264_ctx->frame_number << std::endl;
            
            fwrite(out_pkt.data, 1, out_pkt.size, out_file);
            av_free_packet(&out_pkt);
        }

    }
    
    for (got_output = 1; got_output;) {
        ret = avcodec_encode_video2(codec_h264_ctx, &out_pkt, NULL, &got_output);
        if (ret < 0) {
            printf("Error encoding frame\n");
            goto _end;
            return -1;
        }
        if (got_output) {
            printf("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n",out_pkt.size);
            fwrite(out_pkt.data, 1, out_pkt.size, out_file);
            av_free_packet(&out_pkt);
        }
    }

_end:

    fclose(in_file);
    fclose(out_file);
    avcodec_close(codec_h264_ctx);
    av_free(codec_h264_ctx);
    av_freep(&in_frame->data[0]);
    av_frame_free(&in_frame);

	return 0;
}

