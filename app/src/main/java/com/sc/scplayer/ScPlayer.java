package com.sc.scplayer;

import android.view.Surface;

/**
 * @author: SuperChen
 * @last-modifier: SuperChen
 * @version: 1.0
 * @create-date: 7/29/21 3:23 PM
 * @last-modified-date: 7/29/21 3:23 PM
 * @description:
 */
public class ScPlayer {

    private Surface surface;

    public ScPlayer(Surface surface) {
        this.surface = surface;
    }

    private void setUrl(String urlOrPath) {

    }

    private void start() {

    }

    private void resume() {

    }

    private void pause() {

    }

    private void destroy() {

    }

    public native void native_setUrl(String url);

    public native void native_start();
}
