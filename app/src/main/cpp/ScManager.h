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
    jobject joLifecyclerImpl;
    jobject joController;
    jobject joSurface;
    //反射得到需要回调的java方法
    jmethodID methodStatusHasChanged;
    jmethodID methodOnSizeChange;
    jmethodID methodCreateAudioTrack;
    jmethodID methodPlayAudio;

    void setUrl(JNIEnv *e, jobject thiz, const char *url);

    void setController(JNIEnv *env, jobject thiz);

    void setSurface(JNIEnv *env, jobject thiz, jobject surface);


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

    //视频总时长
    long duration;

    pthread_t threadDecodePacket;

    AVStream *getAvstream(int index);

    void startPlay();

    void stopPlay();

    void release();

    void forward();

    void rewind();

    void doubleSpeed(float doubleB);

    void seek(int progress);

private:

    bool netWorkIsInit = false;

    static void *startDecodePacket(void *pVoid);
};

#endif //SCPLAYERDEMO_SCMANAGER_H
