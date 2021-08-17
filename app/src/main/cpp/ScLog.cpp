//
// Created by SuperChen on 8/4/21.
//

#include <cstdio>
#include <cstdarg>
#include <ctime>
#include "ScLog.h"

#ifdef ANDROID

#include <android/log.h>

#endif

#ifdef DEBUG_VERSION
bool LOGS_ENABLED = true;
#else
bool LOGS_ENABLED = false;
#endif

ScLog &ScLog::getInstance() {
    static ScLog instance;
    return instance;
}

ScLog::ScLog() {

}

void ScLog::d(const char *message, ...) {
    if (!LOGS_ENABLED) {
        return;
    }
    va_list argptr;
    va_start(argptr, message);
    time_t t = time(0);
    struct tm *now = localtime(&t);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_DEBUG, "scplayer_native", message, argptr);
    va_end(argptr);
    va_start(argptr, message);
#endif
}

void ScLog::w(const char *message, ...) {
    if (!LOGS_ENABLED) {
        return;
    }
    va_list argptr;
    va_start(argptr, message);
    time_t t = time(0);
    struct tm *now = localtime(&t);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_WARN, "scplayer_native", message, argptr);
    va_end(argptr);
    va_start(argptr, message);
#endif
}

void ScLog::e(const char *message, ...) {
    if (!LOGS_ENABLED) {
        return;
    }
    va_list argptr;
    va_start(argptr, message);
    time_t t = time(0);
    struct tm *now = localtime(&t);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_ERROR, "scplayer_native", message, argptr);
    va_end(argptr);
    va_start(argptr, message);
#endif
}

