#include <jni.h>
#include <string>
#include "Defines.h"
#include "ScManager.h"
#include "ScLog.h"


JavaVM *javaVm;


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    jint result = -1;
    javaVm = vm;
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    ScManager::getInstance().vm = vm;
    //返回最低支持版本
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScControllerImpl_native_1setController(JNIEnv *env, jobject thiz) {
    ScManager::getInstance().setController(env, thiz);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScControllerImpl_native_1setUrl(JNIEnv *env, jobject thiz, jstring _url) {
    const char *url = env->GetStringUTFChars(_url, nullptr);
    ScManager::getInstance().setUrl(env, thiz, url);
    env->ReleaseStringUTFChars(_url, url);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScLifecyclerImpl_native_1setSurface(JNIEnv *env, jobject thiz,jobject surface) {
    ScManager::getInstance().setSurface(env, thiz, surface);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScLifecyclerImpl_native_1startPlay(JNIEnv *env, jobject thiz) {
    ScManager::getInstance().startPlay();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScLifecyclerImpl_native_1resumePlay(JNIEnv *env, jobject thiz) {
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScLifecyclerImpl_native_1pausePlay(JNIEnv *env, jobject thiz) {
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScLifecyclerImpl_native_1resumePlayByUser(JNIEnv *env, jobject thiz) {
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScLifecyclerImpl_native_1pausePlayByUser(JNIEnv *env, jobject thiz) {
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScLifecyclerImpl_native_1stopPlay(JNIEnv *env, jobject thiz) {
    ScManager::getInstance().stopPlay();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScLifecyclerImpl_native_1release(JNIEnv *env, jobject thiz) {
    ScManager::getInstance().release();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScControllerImpl_native_1forward(JNIEnv *env, jobject thiz) {
    ScManager::getInstance().forward();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScControllerImpl_native_1rewind(JNIEnv *env, jobject thiz) {
    ScManager::getInstance().rewind();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScControllerImpl_native_1doubleSpeed(JNIEnv *env, jobject thiz, jfloat double_b) {
    ScManager::getInstance().doubleSpeed(double_b);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_player_ScSeekImpl_native_1seek(JNIEnv *env, jobject thiz, jint _progress) {
    ScManager::getInstance().seek(_progress);
}