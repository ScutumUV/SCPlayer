#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window_jni.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"初始化层", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_INFO,"h264层", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_INFO,"解码层", __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_INFO,"同步层", __VA_ARGS__)
#define LOGQ(...) __android_log_print(ANDROID_LOG_INFO,"队列层", __VA_ARGS__)

extern "C" {
#include "libavcodec//avcodec.h"
#include "libavformat//avformat.h"
}


AVFormatContext *avFormatContext;
AVCodecContext *avCodecContext;
int videoIndex = -1;
int audioIndex = -1;

extern "C" JNIEXPORT jstring JNICALL
Java_com_sc_scplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_ScPlayer_native_1setUrl(JNIEnv *env, jobject thiz, jstring _url) {
    //转化url string
    const char *url = env->GetStringUTFChars(_url, 0);
    avformat_network_init();
    //初始化全局上下文
    if (avFormatContext == nullptr) {
        avFormatContext = avformat_alloc_context();
    }

    //打开视频文件
    int isSuccess = avformat_open_input(&avFormatContext, url, NULL, NULL);
    //验证视频文件是否有流
    int index = avformat_find_stream_info(avFormatContext, NULL);

    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        //拿到解码器的参数信息
        AVCodecParameters *parameters = avFormatContext->streams[i]->codecpar;
        AVMediaType type = parameters->codec_type;
        //视频流
        if (type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
            //根据视频数据的编码信息来生成解码器
            const AVCodec *dec = avcodec_find_decoder(parameters->codec_id);
            //实例化视频流上下文
            AVCodecContext *videoContext = avcodec_alloc_context3(dec);

            int a = avcodec_parameters_to_context(videoContext, parameters);
            LOGI("视频%d", i);
        } else if (type == AVMEDIA_TYPE_AUDIO) {
            audioIndex = i;
        }
        if (videoIndex != -1 && audioIndex != -1) {
            break;
        }
    }

    env->ReleaseStringUTFChars(_url, url);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_ScPlayer_native_1play(JNIEnv *env, jobject thiz) {
    // TODO: implement native_play()
}