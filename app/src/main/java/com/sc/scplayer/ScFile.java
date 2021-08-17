package com.sc.scplayer;

import android.content.Context;

import java.io.File;

/**
 * @author: supre
 * @last-modifier: supre
 * @version: 1.0
 * @create-date: 8/4/21 3:39 PM
 * @last-modified-date: 8/4/21 3:39 PM
 * @description:
 */
public class ScFile {

    private static volatile String VIDEO_PATH;

    public static void check(Context context) {
        if (context == null) return;
        try {
            File file = context.getApplicationContext().getExternalFilesDir("Video");
            if (!file.exists()) {
                file.mkdirs();
            }
            VIDEO_PATH = file.getAbsolutePath();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static String getVideoPath() {
        return VIDEO_PATH;
    }
}
