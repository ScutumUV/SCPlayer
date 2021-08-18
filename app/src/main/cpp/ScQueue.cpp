//
// Created by SuperChen on 8/4/21.
//

#include <iostream>
#include "ScQueue.h"
#include "ScLog.h"

ScQueue::ScQueue() {
    pthread_mutex_init(&mutexPacket, nullptr);
    pthread_cond_init(&condPacket, nullptr);
}

ScQueue::~ScQueue() {
    clearAvPacket();
}

int ScQueue::push(AVPacket *packet) {
    LOGQ("ScQueue====> push size1");
    LOGQ("ScQueue====> push size2=%d", size());
    queuePacket.push_back(packet);
    return 0;
}

AVPacket *ScQueue::getAvPacket() {
    AVPacket *packet = nullptr;
    if (!queuePacket.empty()) {
//        LOGQ("ScQueue====> size1=%d", size());
        packet = queuePacket.front();
        queuePacket.pop_front();
//        LOGQ("ScQueue====> size2=%d", size());
//        LOGQ("ScQueue ===> packet=%p, packet.size=%d", packet, packet->size);
    }
    return packet;
}

void ScQueue::getAvPacket(AVPacket *packet) {
    if (!queuePacket.empty()){
        LOGQ("ScQueue====> size1 - 1=%d", size());
        packet =  queuePacket.front();
//        queuePacket.pop();
        LOGQ("ScQueue====> size2 - 2=%d", size());
        LOGQ("ScQueue ===> packet=%p, packet.size=%d", packet, packet->size);
    }
}

int ScQueue::size() {
    return queuePacket.size();
}

void ScQueue::clearAvPacket() {
    queuePacket.clear();
//    std::queue<AVPacket *> empty;
//    swap(empty, queuePacket);
//    pthread_exit(nullptr);
}

void ScQueue::release() {
    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&condPacket);
}
