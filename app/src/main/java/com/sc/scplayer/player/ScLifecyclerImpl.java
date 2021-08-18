package com.sc.scplayer.player;

import android.annotation.Nullable;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.lang.ref.WeakReference;

/**
 * @author: supre
 * @last-modifier: supre
 * @version: 1.0
 * @create-date: 8/17/21 10:30 AM
 * @last-modified-date: 8/17/21 10:30 AM
 * @description:
 */
public class ScLifecyclerImpl implements ScLifecycler, SurfaceHolder.Callback {

    //默认,停止状态
    public static final int STATUS_DEFALUT = 0;
    //播放
    public static final int STATUS_PLAYING = 1;
    //恢复
    public static final int STATUS_REAUME = 2;
    //暂停
    public static final int STATUS_PAUSE = 3;
    //用户手动继续播放
    public static final int STATUS_RESUME_BY_USER = 4;
    //用户手动暂停
    public static final int STATUS_PAUSE_BY_USER = 5;
    //缓冲中
    public static final int STATUS_LOADING = 6;


    private WeakReference<SurfaceView> surfaceViewWeak;
    private WeakReference<Surface> surfaceWeak;
    protected int status;


    public ScLifecyclerImpl(SurfaceView surfaceView) {
        surfaceViewWeak = new WeakReference<>(surfaceView);
        surfaceView.getHolder().addCallback(this);
        status = STATUS_DEFALUT;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        surfaceWeak = new WeakReference<>(holder.getSurface());
        native_setSurface(holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
    }

    @Nullable
    protected SurfaceView getSurfaceView() {
        return surfaceViewWeak != null ? surfaceViewWeak.get() : null;
    }

    @Nullable
    protected Surface getSurface() {
        return surfaceWeak != null ? surfaceWeak.get() : null;
    }

    @Override
    public void statusHasChanged(int newStatus) {
        if (newStatus == status) return;
        status = newStatus;
    }

    @Override
    public int getStatus() {
        return status;
    }

    @Override
    public void startPlay() {
        if (status != STATUS_PLAYING) {
            native_startPlay();
        }
    }

    @Override
    public void stopPlay() {
        if (status != STATUS_DEFALUT) {
            native_stopPlay();
        }
    }

    @Override
    public void resumePlay() {
        if (status != STATUS_REAUME) {
            native_resumePlay();
        }
    }

    @Override
    public void pausePlay() {
        if (status != STATUS_PAUSE) {
            native_pausePlay();
        }
    }

    @Override
    public void resumePlayByUser() {
        if (status != STATUS_RESUME_BY_USER) {
            native_resumePlayByUser();
        }
    }

    @Override
    public void pausePlayByUser() {
        if (status != STATUS_PAUSE_BY_USER) {
            native_pausePlayByUser();
        }
    }

    @Override
    public void release() {
        if (surfaceViewWeak != null) {
            surfaceViewWeak.clear();
            surfaceViewWeak = null;
        }
        if (surfaceWeak != null) {
            surfaceWeak.clear();
            surfaceWeak = null;
        }
        native_release();
    }


    protected native void native_setSurface(Surface surface);

    protected native void native_startPlay();

    protected native void native_stopPlay();

    protected native void native_resumePlay();

    protected native void native_pausePlay();

    protected native void native_resumePlayByUser();

    protected native void native_pausePlayByUser();

    protected native void native_release();
}
