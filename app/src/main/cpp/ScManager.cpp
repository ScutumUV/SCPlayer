//
// Created by SuperChen on 8/7/21.
//

#include <pthread.h>
#include "ScManager.h"
#include "ScLog.h"

extern "C" {
#include <libavformat/avformat.h>
}


ScManager::ScManager() = default;

ScManager::~ScManager() = default;

ScManager &ScManager::getInstance() {
    static ScManager instance;
    return instance;
}


AVStream *ScManager::getAvstream(int index) {
    return avFormatContext->streams[index];
}

void ScManager::startPlay() {
    LOGI("ScManager.cpp ===> startPlay start");
    isStart = true;
    if (!netWorkIsInit) {
        avformat_network_init();
    }
    //初始化全局上下文
    avFormatContext = avformat_alloc_context();
    // 1.打开url/或者本地文件路径
    // 打开时的参数设置
    /*AVDictionary *opt;
    // 设置超时参数值为3000000,这里单位是微秒,3000000微秒=3s
    av_dict_set(&opt, "timeout", "3000000", 0);*/
    //打开视频文件
    int isSuccess = avformat_open_input(&avFormatContext, url, nullptr, nullptr);
    LOGI("ScManager.cpp ===> startPlay isSuccess = %d", isSuccess);
    if (isSuccess < 0) {
        LOGERR("open input error %d", isSuccess);
        stopPlay();
        return;
    }
    //验证视频文件是否有流
    int index = avformat_find_stream_info(avFormatContext, nullptr);
    if (index < 0) {
        stopPlay();
        LOGERR("the url not contains any stream %d", index);
        return;
    }
    duration = avFormatContext->duration / AV_TIME_BASE;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        AVMediaType type = avFormatContext->streams[i]->codecpar->codec_type;
        if (type == AVMEDIA_TYPE_VIDEO) {
            streamIndexVideo = i;
        } else if (type == AVMEDIA_TYPE_AUDIO) {
            streamIndexAudio = i;
        } else if (type == AVMEDIA_TYPE_SUBTITLE) {
            streamIndexSubtitle = i;
        }
    }
    if (videoQueue == nullptr) {
        videoQueue = new ScQueue;
    }
    if (audioQueue == nullptr) {
        audioQueue = new ScQueue;
    }
    if (subtitleQueue == nullptr) {
        subtitleQueue = new ScQueue;
    }
    LOGI("ScManager.cpp ===> startPlay start create thread, streamIndexVideo=%d, streamIndexAudio=%d, streamIndexSubtitle=%d", streamIndexVideo, streamIndexAudio, streamIndexSubtitle);
    isStart = true;
    pthread_create(&threadDecodePacket, nullptr, startDecodePacket, nullptr);
    if (scVideo == nullptr) {
        scVideo = new ScVideo;
    }
    scVideo->startPlay();
    if (scAudio == nullptr) {
        scAudio = new ScAudio;
    }
    scAudio->startPlay();
}

void *ScManager::startDecodePacket(void *pVoid) {
    LOGD("decodePacket ===> start");
    while (getInstance().isStart) {
        //如果正在拖拽中,进行休眠100ms
        if (ScManager::getInstance().isSeek) {
            av_usleep(1000 * 100);
            continue;
        }
        //缓冲到一定的地步了,让它空跑
        if (ScManager::getInstance().audioQueue->size() >= 50 && ScManager::getInstance().videoQueue->size() >= 50){
            continue;
        }
        //数据包
        AVPacket *avPacket = av_packet_alloc();
        //读取帧数据，成功会吧avPacket 中的data（压缩数据）填满
        int ret = av_read_frame(getInstance().avFormatContext, avPacket);
        //不等于0代表已经读完或者读取失败了
        if (ret != 0) {
            break;
        }
        //将不同的流存放到对应的队列当中去
        //存放视频流到视频队列中
        if (avPacket->stream_index == getInstance().streamIndexVideo) {
            getInstance().videoQueue->push(avPacket);
//            LOGD("decodePacket ===> videoQueue.size=%d", getInstance().videoQueue->size());
        }
            //存放音频流到音频队列中
        else if (avPacket->stream_index == getInstance().streamIndexAudio) {
            getInstance().audioQueue->push(avPacket);
//            LOGD("decodePacket ===> audioQueue.size=%d", getInstance().audioQueue->size());
        }
            //存放字幕流到字幕队列中
        else if (avPacket->stream_index == getInstance().streamIndexSubtitle) {
            getInstance().subtitleQueue->push(avPacket);
//            LOGD("decodePacket ===> subtitleQueue.size=%d", getInstance().subtitleQueue->size());
        }
    }
    return nullptr;
}

void ScManager::stopPlay() {
    isStart = false;
    if (scVideo != nullptr) {
        scVideo->stopPlay();
    }
    if (scAudio != nullptr) {
        scAudio->stopPlay();
    }
    avformat_free_context(avFormatContext);
    avformat_close_input(&ScManager::getInstance().avFormatContext);
}

void ScManager::forward() {

}

void ScManager::rewind() {

}

void ScManager::doubleSpeed(float doubleB) {

}

void ScManager::seek(int progress) {
    isSeek = true;
    //单位:微秒
    long seconds = duration * ((double) progress / 100) * AV_TIME_BASE;
    LOGI("seek ===> duration=%ld, seconds=%ld, progress=%d", duration, seconds, progress);
    //进行拖动          总上下文          -1:全部,也可以单独视频流or音频流 最小值   时间戳    最大值
    avformat_seek_file(avFormatContext, -1, INT64_MIN, seconds, INT64_MAX, 0);
    //清理已经缓冲的数据
    audioQueue->clearAvPacket();
    avcodec_flush_buffers(scAudio->avCodecContext);
    videoQueue->clearAvPacket();
    avcodec_flush_buffers(scVideo->avCodecContext);
    isSeek = false;
}

void ScManager::release() {
    LOGD("decodePacket ===> release");
    if (scAudio != nullptr) {
        scAudio->realease();
        scAudio = nullptr;
    }
    if (scVideo != nullptr) {
        scVideo->realease();
        scVideo = nullptr;
    }
    if (audioQueue != nullptr) {
        audioQueue->clearAvPacket();
        audioQueue->release();
        audioQueue = nullptr;
    }
    if (videoQueue != nullptr) {
        videoQueue->clearAvPacket();
        videoQueue->release();
        videoQueue = nullptr;
    }
}


void ScManager::setUrl(JNIEnv *e, jobject thiz, const char *_url) {
    LOGI("ScManager.cpp setUrl, url=%s", _url);
    url = _url;
}

void ScManager::setController(JNIEnv *env, jobject thiz) {
    jclass controllerClz = env->GetObjectClass(thiz);
    joController = env->NewGlobalRef(thiz);
    methodOnSizeChange = env->GetMethodID(controllerClz, "onSizeChange", "(II)V");
    methodCreateAudioTrack = env->GetMethodID(controllerClz, "createAudioTrack", "(II)V");
    methodPlayAudio = env->GetMethodID(controllerClz, "playAudio", "([BI)V");
}

void ScManager::setSurface(JNIEnv *e, jobject thiz, jobject surface) {
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
    }
    jclass lifecyclerImplClz = e->GetObjectClass(thiz);
    joLifecyclerImpl = e->NewGlobalRef(thiz);
    joSurface = e->NewGlobalRef(surface);
    methodStatusHasChanged = e->GetMethodID(lifecyclerImplClz, "statusHasChanged", "(I)V");
}


