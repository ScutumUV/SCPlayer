//
// Created by SuperChen on 8/4/21.
//


#ifndef SCPLAYER_SCQUEUE_H
#define SCPLAYER_SCQUEUE_H

#include "queue"
#include "pthread.h"

extern "C" {

#include "include/libavcodec/avcodec.h"
#include "include/libavcodec/packet.h"

}

class ScQueue {

public:
    ScQueue();
    ~ScQueue();
    int push(AVPacket *packet);
    AVPacket* getAvPacket();
    void getAvPacket(AVPacket *packet);
    int size();
    void clearAvPacket();
    void release();
    std::deque<AVPacket *> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
    bool playStatus = false;
};

#endif //SCPLAYER_SCQUEUE_H
