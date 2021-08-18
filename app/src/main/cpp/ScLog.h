//
// Created by SuperChen on 8/4/21.
//

#ifndef SCPLAYER_SCLOG_H
#define SCPLAYER_SCLOG_H

#include "Defines.h"
#include <android/log.h>
#include <android/native_window_jni.h>


extern bool LOGS_ENABLED;

class ScLog {

public:
    ScLog();

    static ScLog &getInstance();

    static void d(const char *message, ...) __attribute__((format (printf, 1, 2)));

    static void w(const char *message, ...) __attribute__((format (printf, 1, 2)));

    static void e(const char *message, ...) __attribute__((format (printf, 1, 2)));
};


#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"scme_init", __VA_ARGS__)
#define LOGIA(...) __android_log_print(ANDROID_LOG_INFO,"scme_init_a", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_INFO,"scme_h264", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_INFO,"scme_decode_v", __VA_ARGS__)
#define LOGET(...) __android_log_print(ANDROID_LOG_INFO,"scme_decode_vT", __VA_ARGS__)
#define LOGEERROR(...) __android_log_print(ANDROID_LOG_ERROR,"scme_decode_v", __VA_ARGS__)
#define LOGEA(...) __android_log_print(ANDROID_LOG_INFO,"scme_decode_a", __VA_ARGS__)
#define LOGEAERROR(...) __android_log_print(ANDROID_LOG_ERROR,"scme_decode_a", __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_INFO,"scme_sync", __VA_ARGS__)
#define LOGQ(...) __android_log_print(ANDROID_LOG_INFO,"scme_quene", __VA_ARGS__)
#define LOGERR(...) __android_log_print(ANDROID_LOG_ERROR,"scme_quene", __VA_ARGS__)

#define DEBUG_E ScLog::getInstance().e
#define DEBUG_W ScLog::getInstance().w
#define DEBUG_D ScLog::getI

#endif //SCPLAYER_SCLOG_Hnstance().d