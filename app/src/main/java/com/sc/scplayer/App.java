package com.sc.scplayer;

import android.app.Application;

public class App extends Application {

    private static volatile Application application;

    @Override
    public void onCreate() {
        super.onCreate();
        application = this;
        System.loadLibrary("scplayer");
        ScFile.check(this);
    }

    public static Application getApplication() {
        return application;
    }
}
