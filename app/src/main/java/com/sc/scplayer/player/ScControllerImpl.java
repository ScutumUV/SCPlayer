package com.sc.scplayer.player;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.text.TextUtils;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;

import com.sc.scplayer.App;

import java.util.ArrayList;
import java.util.List;

/**
 * @author: supre
 * @last-modifier: supre
 * @version: 1.0
 * @create-date: 8/17/21 10:29 AM
 * @last-modified-date: 8/17/21 10:29 AM
 * @description:
 */
public class ScControllerImpl extends ScLifecyclerImpl implements ScController, ScSeek {

    //音频
    protected AudioTrack audioTrack;
    protected ScSeek seekController;
    //是否静音
    protected boolean audioIsMute;
    protected List<Callback> callbackList;

    protected String url;


    public ScControllerImpl(SurfaceView surfaceView) {
        super(surfaceView);
        native_setController();
        setSeekController(new ScSeekImpl());
    }

    public void setUrl(String url) {
        this.url = url;
        if (!TextUtils.isEmpty(url)) {
            native_setUrl(url);
        }
    }

    @Override
    public void startPlay() {
        if (getSurface() == null || TextUtils.isEmpty(url)) {
            return;
        }
        super.startPlay();
    }

    @Override
    public void stopPlay() {
        super.stopPlay();
        if (audioTrack != null && audioTrack.getState() != AudioTrack.PLAYSTATE_STOPPED) {
            audioTrack.stop();
        }
    }

    @Override
    public void resumePlay() {
        super.resumePlay();
        if (audioTrack.getState() != AudioTrack.PLAYSTATE_PLAYING) {
            audioTrack.play();
        }
    }

    @Override
    public void pausePlay() {
        super.pausePlay();
        if (audioTrack != null && audioTrack.getState() != AudioTrack.PLAYSTATE_PAUSED) {
            audioTrack.pause();
        }
    }

    @Override
    public void release() {
        super.release();
        if (audioTrack != null) {
            audioTrack.release();
            audioTrack = null;
        }
        if (callbackList != null) {
            callbackList.clear();
            callbackList = null;
        }
    }

    @Override
    public void forward() {
        native_forward();
    }

    @Override
    public void rewind() {
        native_rewind();
    }

    @Override
    public void doubleSpeed(float doubleB) {
        native_doubleSpeed(doubleB);
    }

    @Override
    public void mute(boolean mute) {
        if (audioIsMute == mute) return;
        audioIsMute = mute;
        if (audioTrack != null) {
            if (audioIsMute) {
                audioTrack.play();
            } else {
                audioTrack.stop();
            }
        }
    }

    @Override
    public void showSubTitle(boolean show) {
    }

    @Override
    public <S extends ScSeek> void setSeekController(S seekController) {
        this.seekController = seekController;
    }

    @Override
    public <S extends ScSeek> S getSeekController() {
        return (S) seekController;
    }

    @Override
    public void addCallback(Callback callback) {
        if (callbackList == null) {
            callbackList = new ArrayList<>();
        }
        if (callback != null && !callbackList.contains(callback)) {
            callbackList.add(callback);
        }
    }

    @Override
    public void removeCallback(Callback callback) {
        if (callbackList != null && callback != null) {
            callbackList.remove(callback);
        }
    }

    @Override
    public View seekCreater(@NonNull FrameLayout parent) {
        return getSeekController().seekCreater(parent);
    }

    @Override
    public void seek(int progress) {
        getSeekController().seek(progress);
    }

    /**
     * 回调，获取视频对应的宽高
     *
     * @param width  视频宽度
     * @param height 视频高度
     */
    protected void onSizeChange(int width, int height) {
        if (getSurfaceView() == null) return;
        Log.i("scme_init", "ScPlayer.java onSizeChange ===> " + "width=" + width + " ,height=" + height);
        //计算视频的宽高比
        float ratio = (float) width / (float) height;
        //获取屏幕的宽度
        int screenWidth = getSurfaceView().getContext().getResources().getDisplayMetrics().widthPixels;
        int h = (int) ((float) screenWidth / ratio);

        boolean tag = Thread.currentThread() == App.getApplication().getMainLooper().getThread();
        Log.i("scme_init", "ScPlayer.java onSizeChange ===> " + "tag=" + tag);
        ViewGroup.LayoutParams lp = getSurfaceView().getLayoutParams();
        if (lp == null) {
            lp = new ViewGroup.LayoutParams(screenWidth, h);
        } else {
            lp.width = screenWidth;
            lp.height = h;
        }
        getSurfaceView().setLayoutParams(lp);
    }

    /**
     * 创建音频解析器
     *
     * @param sampleRateInHz 采样频率 从c层传入
     * @param nbChannels     采样通道数
     */
    protected void createAudioTrack(int sampleRateInHz, int nbChannels) {
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
    protected void playAudio(byte[] buffer, int length) {
        int ret = -1;
        if (audioTrack != null) {
            ret = audioTrack.write(buffer, 0, length);
        }
        Log.i("scme_decode_a", "ScPlayer.java ===> playAudio " + "buffer=" + buffer + " ,length=" + length + " ,audioTrack=" + audioTrack + " ,ret=" + ret);
    }


    protected native void native_setController();

    protected native void native_setUrl(String url);

    protected native void native_forward();

    protected native void native_rewind();

    protected native void native_doubleSpeed(float doubleB);
}
