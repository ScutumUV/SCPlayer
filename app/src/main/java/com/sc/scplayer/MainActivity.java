package com.sc.scplayer;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Environment;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private SurfaceView surfaceView;
    private SurfaceHolder.Callback callback;
    private Surface surface;

    private ScPlayer scPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = findViewById(R.id.surfaceView);
        if (callback == null) {
            callback = new SurfaceHolder.Callback() {
                @Override
                public void surfaceCreated(@NonNull SurfaceHolder holder) {
                    surface = holder.getSurface();
                    scPlayer = new ScPlayer(surface);
                }

                @Override
                public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

                }

                @Override
                public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

                }
            };
        }
        surfaceView.getHolder().addCallback(callback);
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());

    }

    public native String stringFromJNI();

    public void startPlay(View view) {
        String p1 = Environment.getDataDirectory().getAbsolutePath();
        String p2 = Environment.getRootDirectory().getAbsolutePath();
        String p3 = Environment.getExternalStorageDirectory().getAbsolutePath();
        File f1 = new File(p1);
        File f2 = new File(p2);
        File f3 = new File(p3);
        if (f1.exists() && f1.isDirectory()) {
            File[] fs = f1.listFiles();
            if (fs != null) {

            }
        }
        if (f2.exists() && f2.isDirectory()) {
            File[] fs = f2.listFiles();
            if (fs != null) {

            }
        }
        if (f3.exists() && f3.isDirectory()) {
            File[] fs = f3.listFiles();
            if (fs != null) {

            }
        }
        String path = Environment.getDataDirectory().getAbsolutePath() + File.separator + "system" + File.separator + "test.mp4";
        File file = new File(path);
        if (!file.exists()) {
            return;
        }
        if (scPlayer != null) {
            scPlayer.setUrlOrPath(path);
        }
    }
}
