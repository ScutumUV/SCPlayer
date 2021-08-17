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
    queuePacket.push(packet);
    return 0;
}

AVPacket *ScQueue::getAvPacket() {
    AVPacket *packet = nullptr;
    if (!queuePacket.empty()) {
//        LOGQ("ScQueue====> size1=%d", size());
        packet = queuePacket.front();
        queuePacket.pop();
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
    std::queue<AVPacket *> empty;
    swap(empty, queuePacket);
    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&condPacket);
//    pthread_exit(nullptr);
}
