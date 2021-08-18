package com.sc.scplayer;

import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.sc.scplayer.player.ScPlayer;

import java.io.File;

public class MainActivity extends AppCompatActivity {

//    private SurfaceView surfaceView;

    private ScPlayer scPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
//        surfaceView = findViewById(R.id.surfaceView);
//        scPlayer = new ScPlayer(this);
        scPlayer = findViewById(R.id.surfaceView);
        TextView tv = findViewById(R.id.sample_text);
//        tv.setText(stringFromJNI());

    }

    public void startPlay(View view) {
//        String path = "http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8";
//        String path = "https://cctvalih5c.v.myalicdn.com/live/cdrmcctv1_1td.m3u8";
        String path = ScFile.getVideoPath() + File.separator + "test1.mp4";
//        String path = ScFile.getVideoPath() + File.separator + "test1.mp3";
//        String path = ScFile.getVideoPath() + File.separator + "test2.mp4";
//        String path = ScFile.getVideoPath() + File.separator + "test3.mp4";
//        String path = ScFile.getVideoPath() + File.separator + "test4.mp4";
//        String path = ScFile.getVideoPath() + File.separator + "test5.mp4";
        if (!path.contains("http") && !path.contains("https")) {
            File file = new File(path);
            if (!file.exists()) {
                Log.e("scme_init", "视频文件不存在");
                return;
            }
        }
        if (scPlayer != null) {
            scPlayer.setUrl(path);
//            scPlayer.setUrl("http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8");
            scPlayer.startPlay();
        }
    }

    public void stopPlay(View view) {
        if (scPlayer != null) {
            scPlayer.stopPlay();
        }
    }
}
