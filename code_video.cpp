#include "code_video.h"
#include<QString>
#include<QDebug>
#include<QFile>


extern "C"{
    #include<libavdevice/avdevice.h>
    #include<libswscale/swscale.h>
    #include<libavcodec/avcodec.h>
    #include<libavformat/avformat.h>
    #include<libavutil/avutil.h>
    #include<libavutil/imgutils.h>
    #include<SDL2/SDL.h>
}


code_video::code_video() {


}

int code_video::video_play(const QString video_path, const QString output_path)
{
    //打开写入文件
    QFile file(output_path);

    if(!file.open(QIODevice::WriteOnly)){
        qDebug()<<"无法打开写入文件";
        return -1;
    }

    //初始化
    avformat_network_init();


    //打开输入文件
    AVFormatContext *pFormatCtx;

    if(avformat_open_input(&pFormatCtx, video_path.toUtf8().data(), NULL, NULL) < 0){
        qDebug()<<"视频文件打开失败";
        return -1;
    }

    if(avformat_find_stream_info(pFormatCtx, NULL) < 0){
        qDebug()<<"视频流信息获取失败";
        return -1;
    }

    //索引视频流位置
    int videoStreamIndex = -1;
    for(int i=0; i < static_cast<int>(pFormatCtx->nb_streams); i++){
        if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStreamIndex = i;
            break;
        }
    }
    if(videoStreamIndex == -1) {
        qDebug()<<"找不到视频流";
        return -1;
    }

    //根据输入的视频流的编码器参数初始化编解码器的上下文
    AVCodecContext *pCodecCtx = avcodec_alloc_context3(NULL);
    if(avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStreamIndex]->codecpar) < 0){
        qDebug()<<"初始化解码器上下文错误";
        return -1;
    }


    //初始化编解码器
    const AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec == NULL){
        qDebug()<<"找不到支持的解码器";
        return -1;
    }

    if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0){
        qDebug()<<"打不开解码器";
        return -1;
    }

    //创建帧和包，用于存储
    AVFrame *pFrame = av_frame_alloc();
    AVPacket *pPacket = av_packet_alloc();

    //设置缩放器的上下文
    struct SwsContext *pSwsCtx = NULL;

    pSwsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                             pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24,
                             SWS_BILINEAR, NULL, NULL, NULL);





    //解码，，循环读取数据包，将数据包发送到解码器进行解码，得到解码后的帧
    while(av_read_frame(pFormatCtx, pPacket) >= 0){
        if(pPacket->stream_index == videoStreamIndex){
            //Decode video frame
            int response = avcodec_send_packet(pCodecCtx, pPacket);
            if(response < 0){
                qDebug()<<"错误解码帧";
                break;
            }

            while(response >= 0){
                response = avcodec_receive_frame(pCodecCtx, pFrame);
                if(response == AVERROR(EAGAIN) || response == AVERROR_EOF){
                    break;
                }else if(response < 0){
                    qDebug()<<"帧解码错误";
                    break;
                }
            }

            //将帧转换成RGB
            uint8_t *buffer = NULL;
            int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

            // buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
            buffer = static_cast<uint8_t*>(av_malloc(numBytes * sizeof(uint8_t)));

            //将解码后的帧数据填充到缓存
            av_image_fill_arrays(pFrame->data, pFrame->linesize, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

            //缩放器缩放帧
            sws_scale(pSwsCtx, (uint8_t const * const *)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrame->data, pFrame->linesize);
            qDebug()<<"p6"<<"\t"<<pCodecCtx->width<<" "<<"\t"<<pCodecCtx->height;



            //将uint8_t缓存区转换成QByteArray并写入文件
            QByteArray byteArray(reinterpret_cast<const char*>(buffer), numBytes);
            qint64 bytesWritten = file.write(byteArray);

            //检查是否所有数据都写入成功
            if(bytesWritten != numBytes){
                qDebug()<<"写入文件错误，只写入了"<<bytesWritten<<"字节";
                file.close();
                break;
            }

            qDebug() << "数据已成功写入文件";

        }
        av_packet_unref(pPacket);
    }

    //释放资源
    avformat_close_input(&pFormatCtx);
    avcodec_free_context(&pCodecCtx);
    av_frame_free(&pFrame);
    av_packet_free(&pPacket);
    sws_freeContext(pSwsCtx);

    //关闭文件
    file.close();



    return 0;
}
