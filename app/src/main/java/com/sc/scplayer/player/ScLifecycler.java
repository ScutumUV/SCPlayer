package com.sc.scplayer.player;

/**
 * @author: supre
 * @last-modifier: supre
 * @version: 1.0
 * @create-date: 8/17/21 10:31 AM
 * @last-modified-date: 8/17/21 10:31 AM
 * @description:
 */
public interface ScLifecycler {

    void statusHasChanged(int newStatus);

    int getStatus();

    void startPlay();

    void stopPlay();

    void resumePlay();

    void pausePlay();

    void resumePlayByUser();

    void pausePlayByUser();

    void release();
}
