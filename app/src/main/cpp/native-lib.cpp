#include <jni.h>
#include <string>

extern "C" {
#include "libavcodec//avcodec.h"
}

AVCodecContext *avCodecContext;

extern "C" JNIEXPORT jstring JNICALL
Java_com_sc_scplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_ScPlayer_native_1setUrl(JNIEnv *env, jobject thiz, jstring url) {
    // TODO: implement native_setUrl()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_ScPlayer_native_1start(JNIEnv *env, jobject thiz) {
    avCodecContext = new AVCodecContext;
}