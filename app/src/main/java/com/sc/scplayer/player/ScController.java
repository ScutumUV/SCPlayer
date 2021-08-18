package com.sc.scplayer.player;

import java.util.Map;

/**
 * @author: supre
 * @last-modifier: supre
 * @version: 1.0
 * @create-date: 8/17/21 9:43 AM
 * @last-modified-date: 8/17/21 9:43 AM
 * @description:
 */
public interface ScController extends ScLifecycler {

    /**
     * 快进一次,直播中不可用
     */
    void forward();

    /**
     * 快退一次,直播中不可用
     */
    void rewind();

    /**
     * 倍速播放
     *
     * @param doubleB 倍速
     */
    void doubleSpeed(float doubleB);

    /**
     * 开启或关闭静音
     */
    void mute(boolean mute);

    /**
     * 是否显示字幕
     *
     * @param show 是否显示
     */
    void showSubTitle(boolean show);

    /**
     * 返回的进度控制器
     */
    <S extends ScSeek> void setSeekController(S seekController);

    <S extends ScSeek> S getSeekController();

    void addCallback(Callback callback);

    void removeCallback(Callback callback);


    interface Callback {

        void videoInfoGetSuccess(Map<String, Object> videoInfo);

        void progressChanged(int progress, boolean seekDrag);

        void progressChangedBySeek(int progress);

        void exception(int errorCode, String errorDescription);

    }

}
