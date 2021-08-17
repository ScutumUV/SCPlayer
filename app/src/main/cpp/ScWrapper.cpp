#include <jni.h>
#include <string>
#include "Defines.h"
#include "ScManager.h"
#include "ScLog.h"


JavaVM *javaVm;
jobject joPlayer;
jobject joSurface;

jmethodID methodOnSizeChange;
jmethodID methodCreateAudioTrack;
jmethodID methodPlayAudio;


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
Java_com_sc_scplayer_ScPlayer_native_1setUrl(JNIEnv *env, jobject thiz, jobject surface, jstring _url) {
    LOGI("native_1setUrl ===> start");
    joPlayer = env->NewGlobalRef(thiz);
    joSurface = env->NewGlobalRef(surface);
    jclass playClz = env->GetObjectClass(joPlayer);
    methodOnSizeChange = env->GetMethodID(playClz, "onSizeChange", "(II)V");
    methodCreateAudioTrack = env->GetMethodID(playClz, "createAudioTrack", "(II)V");
    methodPlayAudio = env->GetMethodID(playClz, "playAudio", "([BI)V");

    const char *url = env->GetStringUTFChars(_url, nullptr);

    ScManager::getInstance().setJava(env, joPlayer, joSurface, url);

    env->ReleaseStringUTFChars(_url, url);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_ScPlayer_native_1startPlay(JNIEnv *env, jobject thiz) {
    ScManager::getInstance().startPlay();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sc_scplayer_ScPlayer_native_1stop(JNIEnv *env, jobject thiz) {
    ScManager::getInstance().stopPlay();
}