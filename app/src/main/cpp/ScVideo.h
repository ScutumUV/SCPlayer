//
// Created by SuperChen on 8/6/21.
//
#ifndef SCPLAYER_SCVIDEO_H
#define SCPLAYER_SCVIDEO_H


#include <jni.h>
#include <android/native_window.h>

extern "C" {
#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "include/libavutil/imgutils.h"
#include "include/libswscale/swscale.h"
#include "include/libavutil/time.h"
#include "include/libswresample/swresample.h"
}


class ScVideo {


public:
    ScVideo();

    ~ScVideo();

    //视频上下文
    AVCodecContext *avCodecContext;
    //缓冲区
    uint8_t *outBuffer;
    //RGB的AvFrame
    AVFrame *avFrame;
    //转换上下文
    SwsContext *swsContext;
    //拿到nativeWindow，渲染时使用
    ANativeWindow *nativeWindow;
    //
    ANativeWindow_Buffer windowBuffer;
    //rgb宽度
    int rgbWidth;
    //rgb高度
    int rgbHeight;
    //当前帧时间
    double videoNowTime;
    //视频帧的单位时间
    double singleVideoTime;
    //视频常规速度 = 1s / 帧率
    double defaultVideoSpeed;

    //开启渲染线程
    pthread_t thread_decode_video;

    void startPlay();

    void stopPlay();

    void realease();

    double getDelayTime(double diff);


private:

    static void *decodeVideo(void *pVoid);
};

#endif //SCPLAYER_SCVIDEO_H
