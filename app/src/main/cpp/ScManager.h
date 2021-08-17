//
// Created by SuperChen on 8/7/21.
//

#ifndef SCPLAYERDEMO_SCMANAGER_H
#define SCPLAYERDEMO_SCMANAGER_H

#include <pthread.h>
#include <android/native_window.h>
#include <jni.h>
#include "ScVideo.h"
#include "ScQueue.h"
#include "ScAudio.h"

extern "C" {
#include <libavformat/avformat.h>
}


class ScManager {

public:
    ScManager();

    ~ScManager();

    static ScManager &getInstance();

    JavaVM *vm;
    JNIEnv *env;
    jobject joPlayer;
    jobject joSurface;
    //反射得到需要回调的java方法
    jmethodID methodOnSizeChange;
    jmethodID methodCreateAudioTrack;
    jmethodID methodPlayAudio;;

    //是否已经开始解码
    bool isStart;
    //是否正在拖动进度条
    bool isSeek;

    const char *url;
    AVFormatContext *avFormatContext;

    ScQueue *videoQueue;
    ScVideo *scVideo;
    AVInputFormat *avConfing;
    int streamIndexVideo;

    ScAudio *scAudio;
    ScQueue *audioQueue;
    int streamIndexAudio;

    //是否显示字幕
    bool showSubtitle;
    int streamIndexSubtitle;
    ScQueue *subtitleQueue;

    pthread_t threadDecodePacket;

    void setJava(JNIEnv *e, jobject javaPlayer, jobject surface, const char *url);

    void startPlay();

    void stopPlay();

    AVStream *getAvstream(int index);

private:

    bool netWorkIsInit = false;

    static void *startDecodePacket(void *pVoid);
};

#endif //SCPLAYERDEMO_SCMANAGER_H
