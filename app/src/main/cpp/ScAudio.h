//
// Created by SuperChen on 8/6/21.
//

#ifndef SCPLAYER_SCAUDIO_H
#define SCPLAYER_SCAUDIO_H

#include <jni.h>

extern "C" {
#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "include/libavutil/imgutils.h"
#include "include/libswscale/swscale.h"
#include "include/libavutil/time.h"
#include "include/libswresample/swresample.h"
}

class ScAudio {

public:

    //音频上下文
    AVCodecContext *avCodecContext;
    //音频转换上下文
    SwrContext *swrContext;
    //音频帧的单位时间
    double singleAudioTime;
    //当前帧时间
    //音视频同步时使用
    //不是指时间  指的序号
    double audioNowTime;

    void startPlay();

    void stopPlay();

    void realease();


    //开启线程
    pthread_t thread_decode_audio;



private:

    static void *decodeAudio(void *pVoid);
};

#endif //SCPLAYER_SCAUDIO_H
