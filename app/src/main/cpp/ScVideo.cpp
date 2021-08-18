//
// Created by SuperChen on 8/6/21.
//

#include <pthread.h>
#include "ScLog.h"
#include "ScManager.h"
#include "ScVideo.h"


ScVideo::ScVideo() = default;

ScVideo::~ScVideo() = default;


void ScVideo::startPlay() {
    //拿到视频流
    AVStream *avStream = ScManager::getInstance().getAvstream(ScManager::getInstance().streamIndexVideo);
    //拿到解码器的参数信息
    AVCodecParameters *parameters = avStream->codecpar;
    //根据视频数据的编码信息来生成解码器
    const AVCodec *videoDec = avcodec_find_decoder(parameters->codec_id);
    //实例化视频流上下文
    avCodecContext = avcodec_alloc_context3(videoDec);

    //将流的参数信息数据填充到解码器上下文
    avcodec_parameters_to_context(avCodecContext, parameters);

    //开始让解码器工作
    avcodec_open2(avCodecContext, videoDec, 0);

    //视频的具体宽高
    rgbWidth = avCodecContext->width;
    rgbHeight = avCodecContext->height;

    if (LOGS_ENABLED) {
        LOGI("视频index=%d", ScManager::getInstance().streamIndexVideo);
        LOGI("视频类型codec_id=%d, 是否是h264=%d", parameters->codec_id,
             parameters->codec_id == AV_CODEC_ID_H264);
        //这里帧数打印出来的为0，是因为必须要解码时才能读
        LOGI("视频总帧数frame_size=%d", parameters->frame_size);
        LOGI("视频延迟时间video_delay=%d", parameters->video_delay);
        LOGI("采样率bit_rate=%ld", (long) parameters->bit_rate);
        LOGI("视频宽度width=%d", rgbWidth);
        LOGI("视频宽度height=%d", rgbHeight);
    }

    //回调方法
    if (ScManager::getInstance().env != nullptr && ScManager::getInstance().joController != nullptr && ScManager::getInstance().methodOnSizeChange != nullptr) {
        if (LOGS_ENABLED) LOGI("video start callback onSizeChange() method");
        ScManager::getInstance().env->CallVoidMethod(ScManager::getInstance().joController, ScManager::getInstance().methodOnSizeChange, rgbWidth, rgbHeight);
    }

    //获得单帧时间序号
    singleVideoTime = av_q2d(avStream->time_base);
    //帧率
    double fps = (double) avStream->avg_frame_rate.num / (double) avStream->avg_frame_rate.den;
    //初始化常规视频速度
    defaultVideoSpeed = 1 / fps;
    if (LOGS_ENABLED) {
        LOGET("视频singleVideoTime=%f, defaultVideoSpeed=%f, fps=%f", singleVideoTime, defaultVideoSpeed, fps);
    }

    //转换成rgb容器的大小
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, rgbWidth, rgbHeight, 1);
    outBuffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avFrame = av_frame_alloc();
    //在将实例化的缓冲区和avFrame关联起来
    av_image_fill_arrays(avFrame->data, avFrame->linesize, outBuffer, AV_PIX_FMT_RGBA, rgbWidth, rgbHeight, 1);

    //转换上下文
    swsContext = sws_getContext(rgbWidth, rgbHeight, avCodecContext->pix_fmt, rgbWidth, rgbHeight,AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);

    if (ScManager::getInstance().env != nullptr && ScManager::getInstance().joSurface != nullptr) {
        //初始化渲染相关
        //1、通过surface拿到nativeWindow
        nativeWindow = ANativeWindow_fromSurface(ScManager::getInstance().env, ScManager::getInstance().joSurface);
        //设置渲染参数
        ANativeWindow_setBuffersGeometry(nativeWindow, rgbWidth, rgbHeight,WINDOW_FORMAT_RGBA_8888);
    }

    //获取时间基,根据哥伦布编码存的时间序号需要依赖于timeBase
    AVRational timeBase = avStream->time_base;
    singleVideoTime = av_q2d(timeBase);
    LOGI("decodeAudio ===> singleVideoTime=%f", singleVideoTime);

    pthread_create(&thread_decode_video,nullptr, decodeVideo, nullptr);
}

void *ScVideo::decodeVideo(void *pVoid) {
    LOGE("decodeVideo ===> start");
    AVFrame *videoFrame;
    while (ScManager::getInstance().isStart) {
        //如果正在拖拽中,进行休眠100ms
        if (ScManager::getInstance().isSeek) {
            av_usleep(1000 * 100);
            continue;
        }
        //从队列中取出数据 h264数据
        AVPacket *videoPacket = ScManager::getInstance().videoQueue->getAvPacket();
        if (videoPacket == nullptr) {
            av_packet_free(&videoPacket);
            videoPacket = nullptr;
//            LOGE("decodeVideo ===> videoPacket is null");
            continue;
        }
        LOGE("decodeVideo ===> videoPacket.size=%d ,index=%d ,data=%s", videoPacket->size, videoPacket->stream_index, videoPacket->data);
        //解码
        int ret = avcodec_send_packet(ScManager::getInstance().scVideo->avCodecContext, videoPacket);
        LOGE("decodeVideo ===> ret1=%d", ret);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
            LOGEERROR("decodeAudio ===> 解码出错");
            av_packet_free(&videoPacket);
            videoPacket = nullptr;
            continue;
        }
        videoFrame = av_frame_alloc();
        LOGE("decodeVideo ===> videoFrame!=null=%d", videoFrame != nullptr);
        //返回结果==0，代表解码成功，但并不代表渲染成功
        ret = avcodec_receive_frame(ScManager::getInstance().scVideo->avCodecContext, videoFrame);
        LOGE("decodeVideo ===> ret2=%d", ret);
        //videoFrame.data 为原始数据
        LOGE("decodeVideo ===> avFrame.data=%d", videoFrame->linesize[0]);
//        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        if (ret == AVERROR_EOF) {
            //已经读完了
            break;
        }
        if (ret != 0) {
            av_packet_free(&videoPacket);
            videoPacket = nullptr;
            av_free(videoFrame);
            videoFrame = nullptr;
            LOGE("decodeVideo ===> release");
            continue;
        }

        //获取时间pts序号
        double pts = videoFrame->best_effort_timestamp;
        //不是指时间  指的序号
        ScManager::getInstance().scVideo->videoNowTime = pts * ScManager::getInstance().scVideo->singleVideoTime;
        if (ScManager::getInstance().scAudio != nullptr) {
            //算出音视频相差的时间（保证在3ms以内，就认为音视频是同步的）
            //正数：音频超前   负数：视频超前
            double diff = ScManager::getInstance().scAudio->audioNowTime - ScManager::getInstance().scVideo->videoNowTime;
            //计算出对应的延迟时间
            double delayTime = ScManager::getInstance().scVideo->getDelayTime(diff);
            //换算成微秒
            double sleepTime = delayTime * AV_TIME_BASE;
            LOGET("decodeVideo ===> defaultVideoSpeed=%f, pts=%f, singleVideoTime=%f, audioNowTime=%f, videoNowTime=%f, diff=%f, delayTime=%f, sleepTime=%f", ScManager::getInstance().scVideo->defaultVideoSpeed,
                    pts, ScManager::getInstance().scVideo->singleVideoTime, ScManager::getInstance().scAudio->audioNowTime, ScManager::getInstance().scVideo->videoNowTime, diff, delayTime, sleepTime);
            //音视频同步进行动态休眠s
            av_usleep(sleepTime);
        }

        //相当于将起始数据 copy 到 终止数据中，在copy的过程中就已经yuv数据转换成rgb数据，然后就可以渲染到surfaceView中了
        //        转换上下文    起始数据           行数                  0：(从第0行开始)  结束行(多少行结束)    终止数据            终止数据行数
        sws_scale(ScManager::getInstance().scVideo->swsContext, videoFrame->data, videoFrame->linesize, 0, ScManager::getInstance().scVideo->avCodecContext->height,
                  ScManager::getInstance().scVideo->avFrame->data, ScManager::getInstance().scVideo->avFrame->linesize);

        //渲染到屏幕中
        //2、根据nativeWidow去拿到缓冲区
        //开启锁，如果不锁则没有办法进行渲染
        ANativeWindow_lock(ScManager::getInstance().scVideo->nativeWindow, &ScManager::getInstance().scVideo->windowBuffer, nullptr);
        //获取缓冲区
        uint8_t *dstWindow = static_cast<uint8_t *>(ScManager::getInstance().scVideo->windowBuffer.bits);
        for (int i = 0; i < ScManager::getInstance().scVideo->rgbHeight; ++i) {
            //copy每一行数据
            //     目的数据    原数据
            memcpy(dstWindow + i * ScManager::getInstance().scVideo->windowBuffer.stride * 4, ScManager::getInstance().scVideo->outBuffer + i * ScManager::getInstance().scVideo->avFrame->linesize[0],
                   ScManager::getInstance().scVideo->avFrame->linesize[0]);
        }
        //解锁  代表像素数据会实时的刷新到屏幕上去，如果不解锁则没有办法刷新
        ANativeWindow_unlockAndPost(ScManager::getInstance().scVideo->nativeWindow);
//        double time = 40 * 1000;
//        av_usleep(time);
        av_frame_free(&videoFrame);
        av_free(videoFrame);
        videoFrame = nullptr;
        av_packet_free(&videoPacket);
        av_free(videoPacket);
        videoPacket = nullptr;
    }
//    if (videoFrame != nullptr) {
//        av_frame_free(&videoFrame);
//    }
//    ScManager::getInstance().scVideo->stopPlay();
    return nullptr;
}

void ScVideo::stopPlay() {
    LOGE("decodeAudio ===> stopPlay");
    av_freep(avCodecContext);
    avcodec_close(avCodecContext);
    avCodecContext = nullptr;
    sws_freeContext(swsContext);
    av_freep(swsContext);
    swsContext = nullptr;
    av_frame_free(&avFrame);
    avFrame = nullptr;
}

void ScVideo::realease() {

}

/**
 * 根据音视频时间差计算对应的延迟时间, 保证在3ms以内,就认为音视频是同步的
 * @param diff 音视频时间差
 * @return 延迟时间     单位: 秒
 */
double ScVideo::getDelayTime(double diff) {
    double delayTime = 0;
    //音频超前，  视频在常规速度的基础上加快 视频速度在目前的视频速度基础上扩大1.5倍，同时也在  0.5倍常规速度~2倍常规速度之间
    if (diff > 0.003) {
        //在目前的速度上扩大1.5倍  0.5倍常规速度~2倍常规速度之间
        delayTime = diff * 3 / 2;
        //如果小于二分之一，弄成三分之二
        if (delayTime < defaultVideoSpeed / 2) {
            delayTime = defaultVideoSpeed * 3 / 2;
        } else if (delayTime > defaultVideoSpeed * 10) {
            delayTime = defaultVideoSpeed * 10;
        }
        delayTime = - delayTime;
    }
    //视频超前， 视频在常规速度的基础上减慢
    else if (diff < -0.003) {
        //在  0.5倍常规速度~2倍常规速度之间
        //默认减慢为视频速度基础的2/3
        delayTime = -diff * 2 / 3;
        //如果小于二分之一，弄成三分之二
        if (delayTime < defaultVideoSpeed / 2) {
            delayTime = defaultVideoSpeed * 2 / 3;
        } else if (delayTime > defaultVideoSpeed * 10) {
            delayTime = defaultVideoSpeed * 10;
        }
    }
    return delayTime;
}
