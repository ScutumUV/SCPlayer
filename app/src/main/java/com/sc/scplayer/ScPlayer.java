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

    public void setUrlOrPath(String urlOrPath) {
        native_setUrl(urlOrPath);
    }

    public void start() {

    }

    public void resume() {

    }

    public void pause() {

    }

    public void destroy() {

    }

    public native void native_setUrl(String url);

    public native void native_play();
}
