package com.sc.scplayer;

import android.app.Application;

public class App extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        System.loadLibrary("scplayer");
    }
}
