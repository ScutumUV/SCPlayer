package com.sc.scplayer;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;

/**
 * @author: SuperChen
 * @last-modifier: SuperChen
 * @version: 1.0
 * @create-date: 7/29/21 3:23 PM
 * @last-modified-date: 7/29/21 3:23 PM
 * @description:
 */
public class ScPlayer implements SurfaceHolder.Callback {

    private String url;
    private SurfaceView surfaceView;
    private Surface surface;

    //音频
    private AudioTrack audioTrack;

    public ScPlayer(SurfaceView surfaceView) {
        this.surfaceView = surfaceView;
        if (surfaceView != null) {
            surfaceView.getHolder().addCallback(this);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        surface = holder.getSurface();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
    }

    public void start() {
        if (surface == null || TextUtils.isEmpty(url)) {
            return;
        }
        native_startPlay();
    }

    /**
     * 回调，获取视频对应的宽高
     *
     * @param width  视频宽度
     * @param height 视频高度
     */
    public void onSizeChange(int width, int height) {
        Log.i("scme_init", "ScPlayer.java onSizeChange ===> " + "width=" + width + " ,height=" + height);
        //计算视频的宽高比
        float ratio = (float) width / (float) height;
        //获取屏幕的宽度
        int screenWidth = surfaceView.getContext().getResources().getDisplayMetrics().widthPixels;
        int h = (int) ((float) screenWidth / ratio);

        boolean tag = Thread.currentThread() == App.getApplication().getMainLooper().getThread();
        Log.i("scme_init", "ScPlayer.java onSizeChange ===> " + "tag=" + tag);
        ViewGroup.LayoutParams lp = surfaceView.getLayoutParams();
        if (lp == null) {
            lp = new ViewGroup.LayoutParams(screenWidth, h);
        } else {
            lp.width = screenWidth;
            lp.height = h;
        }
        surfaceView.setLayoutParams(lp);
    }

    /**
     * 创建音频解析器
     *
     * @param sampleRateInHz 采样频率 从c层传入
     * @param nbChannels     采样通道数
     */
    public void createAudioTrack(int sampleRateInHz, int nbChannels) {
        int channelConfig;
        if (nbChannels == 1) {
            channelConfig = AudioFormat.CHANNEL_OUT_MONO;
        } else if (nbChannels == 2) {
            channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
        } else {
            channelConfig = AudioFormat.CHANNEL_OUT_MONO;
        }
        //确定一帧的数据量
        int bufferSize = AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig, AudioFormat.ENCODING_PCM_16BIT);
        //                          type                       采样频率         采样通道数       采样位数 16位                     一帧的大小
        audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRateInHz, channelConfig, AudioFormat.ENCODING_PCM_16BIT, bufferSize, AudioTrack.MODE_STREAM);
        int type = audioTrack.getStreamType();
        Log.i("scme_init", "ScPlayer.java ===> createAudioTrack " + "sampleRateInHz=" + sampleRateInHz
                + " ,nbChannels=" + nbChannels + " ,bufferSize=" + bufferSize + " ,type=" + type);
        audioTrack.play();
    }

    /**
     * 回调，获取音频数据和长度
     *
     * @param buffer 音频数据
     * @param length 音频数据长度
     */
    public void playAudio(byte[] buffer, int length) {
        int ret = -1;
        if (audioTrack != null) {
            ret = audioTrack.write(buffer, 0, length);
        }
        Log.i("scme_decode_a", "ScPlayer.java ===> playAudio " + "buffer=" + buffer + " ,length=" + length + " ,audioTrack=" + audioTrack+ " ,ret=" + ret);
    }

    public void stop() {
        native_stop();
    }

    public void resume() {

    }

    public void pause() {

    }

    public void destroy() {

    }

    public void setUrlOrPath(String urlOrPath) {
        this.url = urlOrPath;
        native_setUrl(surface, url);
    }

    public native void native_setUrl(Surface surface, String url);

    public native void native_startPlay();

    public native void native_stop();
}
